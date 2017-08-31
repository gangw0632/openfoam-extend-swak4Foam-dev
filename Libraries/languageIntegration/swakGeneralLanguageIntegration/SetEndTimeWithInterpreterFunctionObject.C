/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2008-2011, 2013-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SetEndTimeWithInterpreterFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "DebugOStream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Wrapper>
SetEndTimeWithInterpreterFunctionObject<Wrapper>::SetEndTimeWithInterpreterFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    TimeManipulationWithInterpreterFunctionObject<Wrapper>(name,t,dict)
{
    this->readParameters(dict);
}

template<class Wrapper>
bool SetEndTimeWithInterpreterFunctionObject<Wrapper>::read(const dictionary& dict)
{
    this->readParameters(dict);
    return TimeManipulationWithInterpreterFunctionObject<Wrapper>::read(dict);
}

template<class Wrapper>
void SetEndTimeWithInterpreterFunctionObject<Wrapper>::readParameters(const dictionary &dict)
{
    this->readCode(dict,"init",initCode_);
    this->readCode(dict,"endTime",endTimeCode_);

    Wrapper::executeCode(
        initCode_,
        false
    );
}

template<class Wrapper>
scalar SetEndTimeWithInterpreterFunctionObject<Wrapper>::endTime()
{
    if(!this->parallelNoRun()) {
        this->setRunTime(this->time());
    }

    if(this->writeDebug()) {
        Pbug << "Evaluating " << endTimeCode_ << endl;
    }
    scalar result=this->evaluateCodeScalar(endTimeCode_,true);
    if(this->writeDebug()) {
        Pbug << "Evaluated to " << result << endl;
    }

    if(result!=this->time().endTime().value()) {
        Info << "Changing end time because " << endTimeCode_
            << " evaluated to " << result << "(current endTime: "
            << this->time().endTime().value() << ") in "
            << this->name() << endl;
    }

    return result;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //