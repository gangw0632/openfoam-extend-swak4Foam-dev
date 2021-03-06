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

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "meshLayersFromFacesPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"
#include "fvsPatchFields.H"

namespace Foam {

defineTypeNameAndDebug(meshLayersFromFacesPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, meshLayersFromFacesPluginFunction , name, meshLayersFromFaces);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

meshLayersFromFacesPluginFunction::meshLayersFromFacesPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    meshLayersGeneralPluginFunction(
        parentDriver,
        name,
        string("blockedFaces internalField surfaceLogicalField")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void meshLayersFromFacesPluginFunction::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0);

    blocked_.set(
        new surfaceScalarField(
            dynamic_cast<const FieldValueExpressionDriver &>(
                driver
            ).getResult<surfaceScalarField>()
        )
    );
}

void meshLayersFromFacesPluginFunction::initFacesAndCells()
{
    labelHashSet facesBlocked;
    const cellList &cells=mesh().cells();

    forAll(blocked_(),faceI) {
        if(CommonValueExpressionDriver::toBool(blocked_()[faceI])) {
            facesBlocked.insert(faceI);
        }
    }
    forAll(blocked_().boundaryField(),patchI) {
        const fvsPatchField<scalar> &p=blocked_().boundaryField()[patchI];
        forAll(p,i) {
            if(CommonValueExpressionDriver::toBool(p[i])) {
                facesBlocked.insert(i+p.patch().patch().start());
            }
        }
    }
    forAll(cells,cellI) {
        bool allBlocked=true;
        forAll(cells[cellI],i) {
            if(!facesBlocked.found(cells[cellI][i])) {
                allBlocked=false;
                break;
            }
        }
        if(allBlocked) {
            cellValues_[cellI]=MeshLayersDistFromPatch(0,true);
        }
    }

    startFaces_=labelList(facesBlocked.toc());
    forAll(startFaces_,i) {
        faceValues_[startFaces_[i]]=MeshLayersDistFromPatch(0,true);
    }
    startValues_=List<MeshLayersDistFromPatch>(
        startFaces_.size(),
        MeshLayersDistFromPatch(1,false)
    );
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
