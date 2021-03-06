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

Contributors/Copyright:
    2010-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SwakImplicitSource.H"
#include "polyMesh.H"
#include "cellSet.H"
#include "fvMatrix.H"
#include "fvm.H"

#include "FieldValueExpressionDriver.H"

namespace Foam {

#ifdef FOAM_HAS_FVOPTIONS
    namespace fv {
#endif

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
template<class T>
SwakImplicitSource<T>::SwakImplicitSource
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    SwakBasicSourceCommon<T>(name, modelType, dict, mesh),
    switchExplicitImplicit_(
        readBool(
            this->coeffs().lookup("switchExplicitImplicit")
        )
    )
{
    this->read(dict);

    this->driver().createWriterAndRead(
        dict.name().name()+"_"+this->type()+"<"+
        pTraits<T>::typeName+">"
    );

    if(this->verbose_) {
        WarningIn(
            string("SwakImplicitSource<") + pTraits<T>::typeName +
            ">::SwakImplicitSource"
        )    << "Adding source term to the fields " << this->fieldNames_
            << " to the values " << this->expressions_
            << " will be verbose. To switch this off set the "
            << "parameter 'verbose' to false" << endl;
    }

}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class T>
SwakImplicitSource<T>::~SwakImplicitSource()
{}


//- Add implicit contribution to equation
template<class T>
void SwakImplicitSource<T>::addSup(fvMatrix<T>& eqn, const label fieldI)
{
    if (debug)
    {
        Info<< "SwakImplicitSource<"<< pTraits<T>::typeName
            << ">::addSup for source " << this->name_ << endl;
    }

    this->driver().clearVariables();
    this->driver().parse(this->expressions_[fieldI]);

    if(
        !this->driver().
        FieldValueExpressionDriver::template resultIsTyp<volScalarField>()
    ) {
        FatalErrorIn("SwakImplicitSource<"+word(pTraits<T>::typeName)+">::addSup()")
            << "Result of " << this->expressions_[fieldI] << " is not a "
                << "volScalarField"
                << endl
                << exit(FatalError);
    }

    volScalarField result(
        this->driver().
        FieldValueExpressionDriver::template getResult<volScalarField>()
    );
    result.dimensions().reset(this->dimensions_[fieldI]);
    volScalarField usedResult(result*0);
    forAll(this->cells_,i) {
        label cellI=this->cells_[i];
        usedResult[cellI]=result[cellI];
    }

    if(switchExplicitImplicit_) {
        eqn+=fvm::SuSp(usedResult,eqn.psi());
    } else {
        eqn+=fvm::Sp(usedResult,eqn.psi());
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
template<class T>
void SwakImplicitSource<T>::addSup(
    const volScalarField& rho,
    fvMatrix<T>& eqn,
    const label fieldI
)
{
    this->addSup(eqn,fieldI);
}

template<class T>
void SwakImplicitSource<T>::addSup(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<T>& eqn,
    const label fieldI
)
{
    this->addSup(eqn,fieldI);
}
#endif

#ifdef FOAM_HAS_FVOPTIONS
    }
#endif

} // end namespace

// ************************************************************************* //
