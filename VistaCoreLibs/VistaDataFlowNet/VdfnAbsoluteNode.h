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


#ifndef _VDFNABSOLUTENODE_H
#define _VDFNABSOLUTENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnSerializer.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"


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
 * Absolute node returning the abs of a value
 *
 * @ingroup VdfnNodes
 * @inport{in,type T,mandatory,the value to invert using this node}
 * @outport{out,type T, the in^-1 (hopefully)}
 */
template<class T>
class TVdfnAbsoluteNode : public IVdfnNode
{
public:
	TVdfnAbsoluteNode()
	: IVdfnNode()
	, m_pInPort( NULL )
	, m_pOutPort( new TVdfnPort<T> )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<T> >);
		RegisterOutPort( "out", m_pOutPort );
	}

	bool PrepareEvaluationRun()
	{
		m_pInPort = dynamic_cast<TVdfnPort<T>*>(GetInPort("in"));
		return GetIsValid();
	}
protected:
	virtual bool DoEvalNode()
	{
		T nValue = m_pInPort->GetValue();
		if( nValue >= 0 )
			m_pOutPort->SetValue( nValue, GetUpdateTimeStamp() );
		else
			m_pOutPort->SetValue( -nValue, GetUpdateTimeStamp() );
		return true;
	}
	
private:
	TVdfnPort<T>* m_pInPort;
	TVdfnPort<T>* m_pOutPort;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VDFNABSOLUTENODE_H

