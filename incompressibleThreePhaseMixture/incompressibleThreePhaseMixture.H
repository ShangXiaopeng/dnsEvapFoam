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

Class
    Foam::incompressibleThreePhaseMixture

Description

SourceFiles
    incompressibleThreePhaseMixture.C

\*---------------------------------------------------------------------------*/

#ifndef incompressibleThreePhaseMixture_H
#define incompressibleThreePhaseMixture_H

#include "incompressible/transportModel/transportModel.H"
#include "IOdictionary.H"
#include "incompressible/viscosityModels/viscosityModel/viscosityModel.H"
#include "dimensionedScalar.H"
#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class incompressibleThreePhaseMixture Declaration
\*---------------------------------------------------------------------------*/

class incompressibleThreePhaseMixture
:
    public IOdictionary,
    public transportModel
{
    // Private data

        word phase1Name_;
        word phase2Name_;
        word phase3Name_;

        volScalarField alpha1_;
        volScalarField alpha2_;
        volScalarField alpha3_;

        const volVectorField& U_;
        const surfaceScalarField& phi_;

        volScalarField nu_;

        autoPtr<viscosityModel> nuModel1_;
        autoPtr<viscosityModel> nuModel2_;
        autoPtr<viscosityModel> nuModel3_;

        dimensionedScalar rho1_;
        dimensionedScalar rho2_;
        dimensionedScalar rho3_;

        dimensionedScalar MW1_;
        dimensionedScalar MW2_;
        dimensionedScalar MW3_;

        dimensionedScalar a_;
        dimensionedScalar b_;
        dimensionedScalar c_;
        dimensionedScalar d_;

	dimensionedScalar Pc_;
	dimensionedScalar Tc_;

        dimensionedScalar hEvap_;

        dimensionedScalar cp1_;
        dimensionedScalar cp2_;
        dimensionedScalar cp3_;

        dimensionedScalar k1_;
        dimensionedScalar k2_;
        dimensionedScalar k3_;
	
	dimensionedScalar LD_;

    // Private Member Functions

        //- Calculate and return the laminar viscosity
        void calcNu();


public:

    // Constructors

        //- Construct from components
        incompressibleThreePhaseMixture
        (
            const volVectorField& U,
            const surfaceScalarField& phi
        );


    //- Destructor
    ~incompressibleThreePhaseMixture()
    {}


    // Member Functions

        const word phase1Name() const
        {
            return phase1Name_;
        }

        const word phase2Name() const
        {
            return phase2Name_;
        }

        const word phase3Name() const
        {
            return phase3Name_;
        }

        const volScalarField& alpha1() const
        {
            return alpha1_;
        }

        volScalarField& alpha1()
        {
            return alpha1_;
        }

        const volScalarField& alpha2() const
        {
            return alpha2_;
        }

        volScalarField& alpha2()
        {
            return alpha2_;
        }

        const volScalarField& alpha3() const
        {
            return alpha3_;
        }

        volScalarField& alpha3()
        {
            return alpha3_;
        }

        //- Return const-access to phase1 density
        const dimensionedScalar& rho1() const
        {
            return rho1_;
        }

        //- Return const-access to phase2 density
        const dimensionedScalar& rho2() const
        {
            return rho2_;
        };

        //- Return const-access to phase3 density
        const dimensionedScalar& rho3() const
        {
            return rho3_;
        };

        //- Return the velocity
        const volVectorField& U() const
        {
            return U_;
        }

        //- Return the flux
        const surfaceScalarField& phi() const
        {
            return phi_;
        }

        //- Return const-access to phase1 viscosityModel
        const viscosityModel& nuModel1() const
        {
            return nuModel1_();
        }

        //- Return const-access to phase2 viscosityModel
        const viscosityModel& nuModel2() const
        {
            return nuModel2_();
        }

        //- Return const-access to phase3 viscosityModel
        const viscosityModel& nuModel3() const
        {
            return nuModel3_();
        }

        //- Return the dynamic laminar viscosity
        tmp<volScalarField> mu() const;

        //- Return the face-interpolated dynamic laminar viscosity
        tmp<surfaceScalarField> muf() const;

        //- Return the kinematic laminar viscosity
        tmp<volScalarField> nu() const
        {
            return nu_;
        }

        //- Return the laminar viscosity for patch
        tmp<scalarField> nu(const label patchi) const
        {
            return nu_.boundaryField()[patchi];
        }

        //- Return the face-interpolated dynamic laminar viscosity
        tmp<surfaceScalarField> nuf() const;

        //- Correct the laminar viscosity
        void correct()
        {
            calcNu();
        }

        //- Read base transportProperties dictionary
        bool read();

///////////////////////////////////////////////////////////////////

        //- Return const-access to phase1 molecular weight
        const dimensionedScalar& MW1() const
        {
            return MW1_;
        }

        //- Return const-access to phase2 molecular weight
        const dimensionedScalar& MW2() const
        {
            return MW2_;
        };

        //- Return const-access to phase3 molecular weight
        const dimensionedScalar& MW3() const
        {
            return MW3_;
        };

        //- Return const-access to the coefficient a
        const dimensionedScalar& a() const
        {
            return a_;
        }

        //- Return const-access to the coefficient b
        const dimensionedScalar& b() const
        {
            return b_;
        };

        //- Return const-access to the coefficient c
        const dimensionedScalar& c() const
        {
            return c_;
        };

        //- Return const-access to the coefficient d
        const dimensionedScalar& d() const
        {
            return d_;
        };

        //- Return const-access to the critical pressure
        const dimensionedScalar& Pc() const
        {
            return Pc_;
        }

        //- Return const-access to the critical temperature
        const dimensionedScalar& Tc() const
        {
            return Tc_;
        }

        //- Return const-access to the specific enthalpy of evaporation
        const dimensionedScalar& hEvap() const
        {
            return hEvap_;
        }

        //- Return const-access to phase1 thermal capacity
        const dimensionedScalar& cp1() const
        {
            return cp1_;
        }

        //- Return const-access to phase2 thermal capacity
        const dimensionedScalar& cp2() const
        {
            return cp2_;
        };

        //- Return const-access to phase3 thermal capacity
        const dimensionedScalar& cp3() const
        {
            return cp3_;
        };

        //- Return const-access to phase1 thermal conductivity
        const dimensionedScalar& k1() const
        {
            return k1_;
        }

        //- Return const-access to phase2 thermal conductivity
        const dimensionedScalar& k2() const
        {
            return k2_;
        };

        //- Return const-access to phase3 thermal conductivity
        const dimensionedScalar& k3() const
        {
            return k3_;
        };


        const dimensionedScalar& LD() const
        {
            return LD_;
        };
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
