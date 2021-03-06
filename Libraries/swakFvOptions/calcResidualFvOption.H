/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::fv::calcResidualFvOption

Description
    Calculate current residual of the matrx/solution

SourceFiles
    calcResidualFvOption.C

Contributors/Copyright:
    2014-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef calcResidualFvOption_H
#define calcResidualFvOption_H

#include "autoPtr.H"
#include "topoSetSource.H"
#include "cellSet.H"
#include "fvMesh.H"
#include "volFields.H"
#include "fvOption.H"

#include "swakFvOption.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
               Class calcResidualFvOption Declaration
\*---------------------------------------------------------------------------*/

class calcResidualFvOption
:
    public option
{
    // Private data

    bool verbose_;

    bool doAddSup_;

    bool doSetValue_;

    // Private Member Functions

    //- Disallow default bitwise copy construct
    calcResidualFvOption(const calcResidualFvOption&);

    //- Disallow default bitwise assignment
    void operator=(const calcResidualFvOption&);

    template<class Type>
    void calcResidual(
        const word &prefix,
        const fvMatrix<Type> &matrix
    );

public:

    //- Runtime type information
    TypeName("calcResidual");


    // Constructors

    //- Construct from explicit source name and mesh
    calcResidualFvOption
    (
        const word& sourceName,
        const word& modelType,
        const dictionary& dict,
        const fvMesh& mesh
    );

    ~calcResidualFvOption();

    // Member Functions

    // Evaluate

    // Correct

    // Add explicit and implicit contributions

    //- Scalar
    virtual void addSup
    (
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void addSup
    (
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void addSup
    (
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void addSup
    (
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void addSup
    (
        fvMatrix<tensor>& eqn,
        const label fieldI
    );

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
    // Add explicit and implicit contributions to compressible equations

    //- Scalar
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<tensor>& eqn,
        const label fieldI
    );

    // Add explicit and implicit contributions to phase equations

    //- Scalar
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<tensor>& eqn,
        const label fieldI
    );
#endif

    // Set values directly

    //- Scalar
    virtual void setValue
    (
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void setValue
    (
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void setValue
    (
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void setValue
    (
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void setValue
    (
        fvMatrix<tensor>& eqn,
        const label fieldI
    );

    //- in newer versions setValue was renamed
    virtual void constrain(fvMatrix<scalar>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }
    virtual void constrain(fvMatrix<vector>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }
    virtual void constrain(fvMatrix<tensor>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }
    virtual void constrain(fvMatrix<symmTensor>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }
    virtual void constrain(fvMatrix<sphericalTensor>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
