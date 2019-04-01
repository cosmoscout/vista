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


#ifndef _VDFNCONSTANTVALUENODE_H
#define _VDFNCONSTANTVALUENODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnUtil.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * this node provides a constant value of type T. Usually, when declared in
 * an XML file, this node will never trigger an update, except for the first
 * evaluation run. When using the C++ API, an update can be triggered using
 * SetValue().
 * Note that in the template code below, there is no reference to SetValue(),
 * this is done by the node creator when the node is constructed.
 *
 * @ingroup VdfnNodes
 * @outport{value,type T,outputs a value of type T}
 */
template<class T>
class TVdfnConstantValueNode : public IVdfnNode
{
public:
	TVdfnConstantValueNode()
	{
		RegisterOutPort( "value", new TVdfnPort<T> );
	}

	void SetValue( T oValue )
	{
		TVdfnPort<T> *pOut = dynamic_cast<TVdfnPort<T>*>(GetOutPort("value"));
		pOut->SetValue(oValue, GetUpdateTimeStamp());
	}

protected:
	/**
	 * @return true
	 */
	bool DoEvalNode()
	{
		return true; // should be ok
	}
private:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNCONSTANTVALUENODE_H

