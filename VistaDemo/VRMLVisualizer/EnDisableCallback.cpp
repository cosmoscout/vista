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


#include "EnDisableCallback.h"

#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

EnDisableCallback::EnDisableCallback( const std::vector<VistaTransformNode*> &nodes )
	:	mNodes( nodes )
{}

EnDisableCallback::~EnDisableCallback(){}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool EnDisableCallback::Do()
{
	// iterate over all nodes
	for( std::vector<VistaTransformNode*>::const_iterator it = mNodes.begin();
			it < mNodes.end();
			++it )
	{
		// toggle the enabled status
		(*it)->SetIsEnabled( !(*it)->GetIsEnabled() );
	}
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "MyDemoAppl.cpp"                                              */
/*============================================================================*/
