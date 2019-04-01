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


#ifndef _VISTASLAVEDATATUNNEL_H
#define _VISTASLAVEDATATUNNEL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/Cluster/VistaDataTunnel.h>

#include <string>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class DLVistaNetDataCollector;
class DLVistaActiveDataProducer;
class DLVistaRamQueuePipe;
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
* VistaSlaveDataTunnel is a IVistaDataTunnel for usage on a cluster client machine. It implements the following behavior:
* Any packets given to the upstream will be deliberately lost. It is assumed, that only the cluster master handles
* upstream requests. In contrast, the downstream is mirrored from the master. For this purpose VistaSlaveDataTunnel
* interfaces with its master counterpart, the VistaMasterDataTunnel and receives serialized packet data via a dedicated
* TCP/IP connection. This data is then passed downstream where it is eventually made available to the application.
*
* @todo I recently added an activator to actively read data from the net -> check if this should be optional (new
* flag for the constructor)
*
*/
class VISTAKERNELAPI VistaSlaveDataTunnel : public IVistaDataTunnel
{
public:
	/**
	* Create the slave data tunnel with a server socket @ strHostName:iPortNum. There the client will
	* wait for an incoming connection from the master DT.
	* For the producer a iQueueSize many packets will be used in the downstream DLV pipe.
	* The prototype data packet pProto will be used to deserialize incoming data packets.
	*/
	VistaSlaveDataTunnel( const std::string& strHostName, int iPortNum,
							int iQueueSize, IDLVistaDataPacket* pProto );
	/**
	* Construct a data tunnel from an existing connection. The connections will be
	* switched to blocking mode. By passing the connection, the memory of all of them
	* will be managed (and eventually deleted) by the DataTunnel!
	* For the producer a iQueueSize many packets will be used in the downstream DLV pipe.
	* The prototype data packet pProto will be used to deserialize incoming data packets.
	*/
	VistaSlaveDataTunnel( VistaConnectionIP* pConnection,
							int iQueueSize, IDLVistaDataPacket* pProto );
	virtual ~VistaSlaveDataTunnel();

	virtual bool ConnectUpstreamInput(IDLVistaPipeComponent* pInput);
	virtual bool ConnectUpstreamOutput(IDLVistaPipeComponent *pOutput);
	virtual bool ConnectDownstreamInput(IDLVistaPipeComponent* pInput);
	virtual bool ConnectDownstreamOutput(IDLVistaPipeComponent *pOutput);

	virtual bool DisconnectUpstreamInput();
	virtual bool DisconnectUpstreamOutput();
	virtual bool DisconnectDownstreamInput();
	virtual bool DisconnectDownstreamOutput();

	virtual void StartTunneling();
	virtual void StopTunnelingGently(bool bJoin);

	virtual void HaltTunneling();
	virtual void IndicateTunnelingEnd();
	virtual void SignalPendingRequest();


protected:

private:
	class DeadEnd;
	class DeadProd;
	/**
	* the collector is the data source for the downstream pipe i.e. it receives downstream data packets from
	* the cluster-master.
	*/
	DLVistaNetDataCollector* m_pCollector;
	/**
	* activator used for active data tunneling in the downstream
	* it actively fetches data from the net data collector and pushes it to the ramqueue pipe
	* which in turn is connected to the downstream output
	*/
	DLVistaActiveDataProducer *m_pActivator;
	/**
	* this queue stores the packets gathered by the packet pump until they are
	* retrieved via the downstream output
	*/
	DLVistaRamQueuePipe *m_pQueue;
	/**
	* these are simple dead end consumers for the up/downsstream pipes. They basically do nothing but recycle
	* incoming packets. They are used to terminate the pipes correctly in order to prevent any crashes.
	* NOTE that it's generally a bad idea to do anything active on the backend of a VistaDataTunnel (i.e.
	* pull data from the upstream or push data to the downstream
	*/
	DeadEnd* m_pUpstreamDeadEnd;
	DeadEnd* m_pDownstreamDeadEnd;
	/**
	* a dead producer used to terminate the outbound part of the upstream pipe.
	*/
	DeadProd* m_pUpstreamDeadProd;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASLAVEDATATUNNEL_H

