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


#ifndef _VDFNLATESTUPDATENODE_H
#define _VDFNLATESTUPDATENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"
#include "VdfnNodeFactory.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaAspectsConfig.h>

#include <vector>

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
 * Writes the value from the inport with the latest update to the out port.
 * Accepts every in port from type T.
 * In the first run the first port is set as out port value.
 *
 * @ingroup VdfnNodes
 * @outport{out, T, the latest value}
 */
template <class T>
class TVdfnLatestUpdateNode : public IVdfnNode
{
public:
	TVdfnLatestUpdateNode()
	: m_pOut( new TVdfnPort<T> )
	{
		RegisterOutPort( "out", m_pOut );
	}

	bool PrepareEvaluationRun()
	{
		return GetIsValid();
	}

	/**
	 * Register every inport like the shallow node, but with type
	 * checking.
	 */
	bool SetInPort(const std::string &sName, IVdfnPort *pPort)
	{
		if( pPort->GetPortTypeCompare()->IsTypeOf(m_pOut) )
		{
			m_mpInPrototypes[ sName ] = pPort->GetPortTypeCompare();
			return DoSetInPort(sName, pPort);
		}
		else
		{
			return false;
		}
	}

protected:

	/**
	 * Iterates over each in port and set the outports value to the one with the
	 * highest timestamp.
	 */
	bool DoEvalNode()
	{
		double latestStamp = -1;
		IVdfnPort* latestPort = NULL;
		for( PortMap::const_iterator it = m_mpInPorts.begin(); it != m_mpInPorts.end(); ++it )
		{
			double d = it->second->GetUpdateTimeStamp();
			if(d > latestStamp)
			{
				latestStamp = d;
				latestPort = it->second;
			}
		}

		if( latestPort )
			m_pOut->SetValue(dynamic_cast<TVdfnPort<T>*>(latestPort)->GetValueConstRef(),GetUpdateTimeStamp());

		return true;
	}

private:
	TVdfnPort<T> *m_pOut;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNLATESTUPDATENODE_H

