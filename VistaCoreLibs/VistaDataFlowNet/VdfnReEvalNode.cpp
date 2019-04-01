/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/


#include "VdfnReEvalNode.h" 

#include <VistaTools/VistaBasicProfiler.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVdfnReEvalNode::IVdfnReEvalNode()
: IVdfnNode()
{
}

IVdfnReEvalNode::~IVdfnReEvalNode()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool IVdfnReEvalNode::NeedsEval() const
{
	if( IVdfnNode::NeedsEval() )
		return true;
	else
		return GetNeedsReEvaluation();
}

bool IVdfnReEvalNode::EvalNode( double nTimeStamp )
{
	if( GetNeedsReEvaluation() )
	{
		//VistaProfileScope( "Eval[" + GetNameForNameable() + "]" );
		SetUpdateTimeStamp( nTimeStamp );
		return DoEvalNode();
	}
	else
		return IVdfnNode::EvalNode( nTimeStamp );
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


