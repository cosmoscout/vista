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


#ifndef _VISTAMASTERDATATUNNEL_H
#define _VISTAMASTERDATATUNNEL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/Cluster/VistaDataTunnel.h>
#include <string>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class DLVistaNetDataDispatcher;
class IDLVistaFilter;
class DLVistaActiveFilter;
class DLVistaRamQueuePipe;
class DLVistaProfilingFilter;
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
* VistaMasterDataTunnel is the IVistaDataTunnel implementation for usage on the cluster master system.
* Basically the upstream is just a direct connection to whatever consumer is connected to its end. Thus the
* data on the upstream is merely forwarded. The trick of VistaMaterDataTunnel lies in the downstream 'tunnel
* tube'. This pipeline contains a T junction which is used in order to mirror any downstream data packets to
* all the clients. The T junction has two outbounds. The first one is a special dispatcher which distributes the
* inbound data. The second one (mind the order) is the standard application defined consumer that yields the data
* to the application. By subtyping and redifinition of the 'connect' routines this behaviour can be changed.
*
* NOTE: Big data packets that need some time to be transmitted might result in cluster display inconsistencies,
* since each client will display the incoming data ASAP. We might consider developing some kind of data sync here.
*
* @todo We might try a CloneTeeFilter instead of the currently use SerialTeeFilter. This might allow faster data
* processing on the master at the cost of additional memory overhead.
*
* @todo I recently added an activator to actively push data through the T -> check if this should be optional (new
* flag for the constructor)
*
*
*/

class VISTAKERNELAPI VistaMasterDataTunnel : public IVistaDataTunnel
{
public:
	/**
	* Construct a data tunnel comprising the clients given in vecHostNames. It is assumed, that each client is
	* listening on the port given in vecClientPorts
	*/
	VistaMasterDataTunnel( const std::vector<std::string>& vecHostNames,
							const std::vector<int>& vecClientPorts );
	/**
	* Construct a data tunnel from existing connections. The connections will be
	* switched to blocking mode. By passing connections, the memory of all of them
	* will be managed (and eventually deleted) by the DataTunnel!
	*/
	VistaMasterDataTunnel( const std::vector<VistaConnectionIP*>& vecConnections );
	virtual ~VistaMasterDataTunnel();

	virtual bool ConnectUpstreamInput(IDLVistaPipeComponent* pInput);
	virtual bool ConnectUpstreamOutput(IDLVistaPipeComponent *pOutput);
	virtual bool ConnectDownstreamInput(IDLVistaPipeComponent* pInput);
	virtual bool ConnectDownstreamOutput(IDLVistaPipeComponent *pOutput);

	virtual bool DisconnectUpstreamInput();
	virtual bool DisconnectUpstreamOutput();
	virtual bool DisconnectDownstreamInput();

	virtual bool DisconnectDownstreamOutput();

	/*
	 * start to (actively) tunnel data (if there is any thread in there)
	 */
	virtual void StartTunneling();
	/**
	 * stop to tunnel data (and make any threads join)
	 * @param bJoin wait until the thread has been finished
	 */
	virtual void StopTunnelingGently(bool bJoin);
	/**
	 * stop tunneling right now (by killing any threads that tunnel data)
	 */
	virtual void HaltTunneling();

	virtual void IndicateTunnelingEnd();
	virtual void SignalPendingRequest();

protected:

private:
	IDLVistaFilter*			m_pTeeFilter;
	DLVistaNetDataDispatcher*	m_pDispatcher;
	/**
	* HACK: Use a profiling filter in the pipe since the T is not pullable
	* and therefore does not work with the activator
	*/
	DLVistaProfilingFilter *m_pProfiler;
	/**
	* actively pump data through the profiling filter
	*/
	DLVistaActiveFilter *m_pActivator;
	/**
	* buffer the downstream output
	*/
	DLVistaRamQueuePipe *m_pQueue;
	/*
	*	It might be useful to make the dispatcher an active component with its own thread. Then we would need
	*	an additional DLVistaActiveDataProducer and a buffering component. This might alleviate timing problems,
	*	when distributing large amounts of data.
	*/
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMASTERDATATUNNEL_H

