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


#ifndef _VDFNTIMERNODE_H
#define _VDFNTIMERNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include <VistaInterProcComm/DataLaVista/Base/VistaDLVTypes.h>
#include <VistaBase/VistaTimer.h>

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
 * the timer node can be used to query the current time and the dt since the
 * last evaluation of the node. Can be handy to trigger timing computation in
 * the network. It does evaluation based on internal state, so it does not have
 * an inport to be set.
 * In cluster mode, this node is evaluated on the cluster master only,
 * reflecting the server clock in a distributed setup.
 * Its API is developed to have an exchangeable timer interface to specify the
 * source of the system clock (CGetTime).
 *
 * @ingroup VdfnNodes
 * @outport{time,double,the current timestamp in seconds since epoch}
 * @outport{dt,double,the timestamp in seconds since last evaluation of this node}
 *
 * the node is evaluating unconditionally, that means it will produce output on
 * every iteration.
 */
class VISTADFNAPI VdfnTimerNode : public IVdfnNode
{
public:
	/**
	 * a timer interface to use for querying the node time
	 */
	class VISTADFNAPI CGetTime
	{
	public:
		virtual ~CGetTime() {}
		/**
		 * get the current time (abstract the timing interface).
		 * the API is not const on purpose, as implementations may need
		 * to conserve state during the operation.
		 * @return the current system time in seconds since epoch
		 */
		virtual double GetTime() = 0;
	};


	/**
	 * @param pTime the timer interface to use, pTime deletion will be managed by Node.
	 * @param bReset controls the behavior of the node upon activation
	          of the graph (OnActivation()). If set to true, the node will
	          reset the dt value to 0, as otherwise dt will contain the
	          timespan between deactivation and activation.
	 */
	VdfnTimerNode( CGetTime *pTime, bool bReset );

	/**
	 * empty
	 */
	virtual ~VdfnTimerNode();

	/**
	 * @return true when a valid timer was given during construction
	 */
	bool GetIsValid() const;

	/**
	 * in case this node is flagged to reset on activation, this node
	 * is reset on activation or not. (surprising, is it not?)
	 * @param dTs the current timestamp
	 */
	void OnActivation( double dTs );

	/**
	 * this is a master sim node, so in case of serialization, it will be
	 * serialized to reflect the master timing on the slave node.
	 * @return true
	 */
	virtual bool GetIsMasterSim() const { return true; }

protected:
	virtual bool   DoEvalNode();

private:
	TVdfnPort<double> *m_pTimer,
					  *m_pDt;

	bool               m_bReset;

	CGetTime          *m_pGetTime;
	double             m_nLastUpdate;
};

/**
 * implementation of a timer interface based on VistaTimer.
 */
class VISTADFNAPI CTimerGetTime : public VdfnTimerNode::CGetTime
{
public:
	CTimerGetTime();

	/**
	 * @return the system time (VistaTimer::GetSystemTime())
	 */
	double GetTime();
private:
	VistaTimer m_oTimer;
};

// #############################################################################


/**
 * a ticktimer node can be used to inject activation at user defined intervals,
 * in a millisecond range. The timespan is configured using an inport.
 *
 * @inport{step,double,mandatory,the timestep in milliseconds for outport triggering}
 * @outport{time,double,the current timestamp at evaluation}
 * @outport{ticktime,double,the time really elapsed
                     between the evaluation runs (can be larger than set using step)}
 * @outport{fraction,double,reflects ticktime-step}
 */
class VISTADFNAPI VdfnTickTimerNode : public IVdfnNode
{
public:
	/**
	 * @param pGetTime the timer interface to use for timings. pGetTime deletion will be managed by Node.
	 */
	VdfnTickTimerNode( VdfnTimerNode::CGetTime *pGetTime );
	~VdfnTickTimerNode();

	/**
	 * @return true when a valid timer interface was specified during construction
	           and the inport <i>step</i> is set.
	 */
	bool GetIsValid() const;

	/**
	 * @return GetIsValid()
	 */
	bool PrepareEvaluationRun();

	/**
	 * in a clustered setup, this node relfects the master's state on all slaves.
	 * @return true
	 */
	virtual bool GetIsMasterSim() const { return true; }

protected:
	bool DoEvalNode() ;

private:
	VdfnTimerNode::CGetTime *m_pGetTime;
	TVdfnPort<double> *m_pTickTime,
					  *m_pFraction,
					  *m_pTimeStep,
					  *m_pTime;
	double             m_nLastUpdate;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNTIMERNODE_H

