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


#ifndef _VDFNCHANGEDETECTNODE_H
#define _VDFNCHANGEDETECTNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"
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
 * The change detect defines a comparison <b>by value</b> from its inport.
 * This is: it reads the current inport value and compares it to its last value
 * (last evaluation that is). If this is different, it will write the new value
 * to its outport. The first run is special, as this will write the current value
 * to the outport without condition. This is in principle ok, as this indicates
 * the state change from "no state" to "some state" and is a change.
 * This node is a template node, as it passes its type and value from inport
 * to outport.
 * Node that this nodes does make a copy of the last value and calls the equals
 * operator on the value (==). This may be costly.
 *
 * @ingroup VdfnNodes
 * @inport{in,type T,mandatory, the incoming value to check}
 * @outport{out,type T, the outport that contains the new value}
 */
template<class T>
class TVdfnChangeDetectNode : public IVdfnNode
{
	public:
	TVdfnChangeDetectNode()
		: IVdfnNode(),
		  m_pOutPort(new TVdfnPort<T>),
		  m_pInPort(NULL),
		  m_bFirstRun(true)
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<T> > );
		RegisterOutPort( "out", m_pOutPort );
	}

	~TVdfnChangeDetectNode()
	{
	}

	bool PrepareEvaluationRun()
	{
		m_pInPort = VdfnUtil::GetInPortTyped<TVdfnPort<T>*>( "in", this );
		return GetIsValid();
	}
protected:
	/**
	 * on first run, it forwards the inport value to the outport value,
	 * on every later run, it checks whether the <i>value</i> of the inport
	 * has changed compared to the last update. If so, it will forward the new
	 * value to its outport.
	 * @return true
	 */
	bool DoEvalNode()
	{
		if( m_bFirstRun || m_pInPort->GetValue() != m_nLastValue )
		{
			m_nLastValue = m_pInPort->GetValue();
			m_pOutPort->SetValue( m_nLastValue, GetUpdateTimeStamp() );
			m_bFirstRun = false;
		}
		return true;
	}
private:

	T m_nLastValue;
	TVdfnPort<T> *m_pOutPort,
				 *m_pInPort;
	bool m_bFirstRun;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNCHANGEDETECTNODE_H

