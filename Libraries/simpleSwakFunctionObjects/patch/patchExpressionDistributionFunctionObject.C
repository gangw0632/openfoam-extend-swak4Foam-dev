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
    2008-2011, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "patchExpressionDistributionFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"
#include "surfaceFields.H"
#include "pointFields.H"

#include "wordReList.H"
#include "HashSet.H"
#include "stringListOps.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(patchExpressionDistributionFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        patchExpressionDistributionFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

patchExpressionDistributionFunctionObject::patchExpressionDistributionFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    distributionFunctionObject(name,t,dict),
    expression_(
        dict.lookup("expression"),
        dict
    ),
    data_(dict)
{
    const fvMesh &mesh=refCast<const fvMesh>(obr_);

    wordReList newPatches(dict.lookup("patchNames"));
    wordHashSet patchNamesNew;
    wordList allPatches(mesh.boundaryMesh().names());
    forAll(newPatches,i) {
        labelList IDs=findStrings(newPatches[i],allPatches);
        forAll(IDs,j) {
            patchNamesNew.insert(allPatches[IDs[j]]);
        }
    }

    patchNames_=patchNamesNew.toc();
    patchIDs_.setSize(patchNames_.size());

    forAll(patchNames_,i) {
        const word &name=patchNames_[i];
        patchIDs_[i]=mesh.boundaryMesh().findPatchID(name);
        if(patchIDs_[i]<0) {
            FatalErrorIn("patchExpressionDistributionFunctionObject::patchExpressionDistributionFunctionObject")
                << "Patch name " << name << " is not a valid patch"
                    << endl
                    << exit(FatalError);
        }
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

word patchExpressionDistributionFunctionObject::dirName()
{
    word result(typeName);
    forAll(patchNames_,i) {
	result+="_"+patchNames_[i];
    }

    return result;
}

word patchExpressionDistributionFunctionObject::baseName()
{
    word result("expression");
    forAll(patchNames_,i) {
	result+="_"+patchNames_[i];
    }

    return result;
}

void patchExpressionDistributionFunctionObject::getDistribution()
{
    if(drivers_.size()<=0) {
        WarningIn("patchExpressionDistributionFunctionObject::getDistribution")
            << "No drivers/patches specified"
                << endl;
        return;
    }

    getDistributionInternal(distScalar_);
    getDistributionInternal(distVector_);
    getDistributionInternal(distTensor_);
    getDistributionInternal(distSymmTensor_);
    getDistributionInternal(distSphericalTensor_);
}

void patchExpressionDistributionFunctionObject::writeSimple()
{
    forAll(patchIDs_,i) {
        PatchValueExpressionDriver &driver=drivers_[i];

        if(verbose()) {
            Info << "DistributionExpression " << name() << " on " << patchNames_[i] << ": ";
        }

        driver.clearVariables();
        driver.parse(expression_);
    }

    distributionFunctionObject::writeSimple();

    if(verbose()) {
        Info << endl;
    }

    // make sure that the stored Variables are consistently written
    forAll(drivers_,i) {
        drivers_[i].tryWrite();
    }
}

bool patchExpressionDistributionFunctionObject::start()
{
    bool result=distributionFunctionObject::start();
    const fvMesh &mesh=refCast<const fvMesh>(obr_);


    drivers_.clear();
    drivers_.resize(patchIDs_.size());

    forAll(drivers_,i) {
        drivers_.set(
            i,
            new PatchValueExpressionDriver(
                data_,
                mesh.boundary()[patchIDs_[i]]
            )
        );
        drivers_[i].createWriterAndRead(
            name()+"_"+mesh.boundary()[patchIDs_[i]].name()+"_"+type()
        );
    }
    return result;
}

} // namespace Foam

// ************************************************************************* //
