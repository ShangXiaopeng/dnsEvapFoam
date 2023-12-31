/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2018 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    dnsEvapFoam

Description
    Solver for 3 incompressible fluids, two of which are miscible, using a VOF
    method to capture the interface, with optional mesh motion and mesh topology
    changes including adaptive re-meshing.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "dynamicFvMesh.H"
#include "MULES.H"
#include "CMULES.H"
#include "localEulerDdtScheme.H"
#include "subCycle.H"
#include "immiscibleIncompressibleThreePhaseMixture.H"
#include "turbulentTransportModel.H"
#include "pimpleControl.H"
#include "fvOptions.H"
#include "CorrectPhi.H"
#include "fvcSmooth.H"

#include "MeshGraph.H"
#include "wallFvPatch.H"
//#include "volPointInterpolation.H"
//#include "interpolatePointToCell.H"
//#include <set>
#include "Pstream.H"
#include "alphaContactAngleFvPatchScalarField.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "postProcess.H"

    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createDynamicFvMesh.H"
    #include "initContinuityErrs.H"
    #include "createDyMControls.H"
    #include "createFields.H"
    #include "initCorrectPhi.H"
    #include "createUfIfPresent.H"

    turbulence->validate();

    if (!LTS)
    {
        #include "readTimeControls.H"
        #include "CourantNo.H"
        #include "setInitialDeltaT.H"
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


    Info<< "\nStarting time loop\n" << endl;

    while (runTime.run())
    {
        #include "readDyMControls.H"
        volScalarField divU("divU0", fvc::div(fvc::absolute(phi, U)));////////////////////////////////////////////
        if (LTS)
        {
            #include "setRDeltaT.H"
        }
        else
        {
            #include "CourantNo.H"
            #include "alphaCourantNo.H"
            #include "setDeltaT.H"
        }

        runTime++;

        Info<< "Time = " << runTime.timeName() << nl << endl;

        // --- Pressure-velocity PIMPLE corrector loop
        while (pimple.loop())
        {
            if (pimple.firstIter() || moveMeshOuterCorrectors)
            {
                mesh.update();

                if (mesh.changing())
                {
                    gh = (g & mesh.C()) - ghRef;
                    ghf = (g & mesh.Cf()) - ghRef;


                    if (correctPhi)
                    {
                        // Calculate absolute flux
                        // from the mapped surface velocity
                        phi = mesh.Sf() & Uf();

                        #include "correctPhi.H"

                        // Make the flux relative to the mesh motion
                        fvc::makeRelative(phi, U);

                        mixture.correct();
                    }

                    if (checkMeshCourantNo)
                    {
                        #include "meshCourantNo.H"
                    }
                }
            }
//------------------------------------------------------------------------//

    const scalar CSK = 0.5;
    const label nSmoothingAlpha(5);
    volScalarField alpha1s(alpha1);

    for (label jj = 1; jj<=nSmoothingAlpha; jj++)
    {
	alpha1s = 
            CSK*(fvc::average(fvc::interpolate(alpha1s))) 
	  + (1.0 - CSK)*alpha1s;
    }

    volVectorField gradAlpha = fvc::grad(alpha1s);
    surfaceVectorField gradAlphaf(fvc::interpolate(gradAlpha));
    surfaceVectorField nHatfv(gradAlphaf/(mag(gradAlphaf) + mixture.deltaN()));
    const surfaceVectorField& Sf = mesh.Sf();

    surfaceScalarField nHatf = nHatfv & Sf;
    nHat = -gradAlpha/(mag(gradAlpha) + mixture.deltaN())*pos(1 - alpha1s - 1e-6);

//    nHat = -fvc::grad(alpha1)/(mag(fvc::grad(alpha1)) + mixture.deltaN());

//    Kappa = -fvc::div(mixture.nHatf());
//    KappaS = -fvc::div(nHatf);
//----------------------------------------------------------------------------//

            divU = fvc::div(fvc::absolute(phi, U));///////////////////////////////////
	    #include "calcSource.H"
            #include "alphaControls.H"
            #include "alphaEqnSubCycle.H"

            mixture.correct();

            #include "UEqn.H"

            // --- Pressure corrector loop
            while (pimple.correct())
            {
                #include "pEqn.H"
            }

            if (pimple.turbCorr())
            {
                turbulence->correct();
            }

	    #include "TEqn.H"
	    J = linearInterpolate(alpha2*U - D23*fvc::grad(alpha2))&mesh.Sf();
        }
//---------------------------------------------------------------------------//
/*

    const scalar CSK = 0.5;
    const label nSmoothingAlpha(5);
    volScalarField alpha1s(alpha1);

    for (label jj = 1; jj<=nSmoothingAlpha; jj++)
    {
	alpha1s = 
            CSK*(fvc::average(fvc::interpolate(alpha1s))) 
	  + (1.0 - CSK)*alpha1s;
    }

    volVectorField gradAlpha = fvc::grad(alpha1s);

    nHat = -gradAlpha/(mag(gradAlpha) + mixture.deltaN());

*/
//---------------------------------------------------------------------------//
//	}

        #include "continuityErrs.H"

        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
