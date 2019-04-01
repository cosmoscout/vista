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


#ifndef _VISTANETDATADISPATCHER_H
#define _VISTANETDATADISPATCHER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataSink.h>
#include <vector>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaConnectionIP;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
* DLVistaNetDataDispatcher is a DataLaVista (DLV) sink, which is meant to connect to n clients and
* distribute any incoming packets to them. There are no special assumptions concerning the packet
* content, since the dispatcher just uses the IVistaSerializable interface. However, for the client side
* this assumes that the receiving client pipe is currently handling the very same type of data packet. If
* this were not the case, serious problems (i.e. crashes) will occur upon deserialization.
*
* NOTE: For now I hard-coded the use of TCP/IP. One might prefer a more general approach, e.g.
*		if one is concerned with transmitting data via UDP or some other protocol.
*
*/
class VISTAINTERPROCCOMMAPI DLVistaNetDataDispatcher : public DLVistaDataSink
{
public:
	DLVistaNetDataDispatcher();
	virtual ~DLVistaNetDataDispatcher();
	/**
	* DataLaVista interface
	* Consuming a packet here means "serialize the packet and send it out to all clients"
	*/
	bool ConsumePacket(IDLVistaDataPacket* p);
	/**
	* Add a client to the dispatcher. The dispatcher will try to get a connection to the client
	* <sHost>, who should have a listening server socket on port <iPort>
	*/
	bool AddClient(const std::string& sHost, const int iPort);
	/**
	* Add a client using an existing IP connection. The connection will be switched to
	* blocking, if it isn't already.
	*/
	bool AddClient( VistaConnectionIP* pConnection );
protected:
private:
	/**
	*	remember all my clients
	*/
	std::vector<VistaConnectionIP*> m_vecClients;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANETDATADISPATCHER_H


