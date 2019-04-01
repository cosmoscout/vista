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


#ifndef _VISTADATACOLLECTOR_H
#define _VISTADATACOLLECTOR_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataSource.h>
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
* DLVistaDataCollector is a DataLaVista data source. It reads arbitrary data from an TCP/IP connection,
* deserializes data packets using this data and then inserts them into the pipeline. Note that the packet's
* concrete type must match the type sent to the connections since otherwhise the deserialization will end up
* in a mess. Other than that there are no special assumptions made about the packet's content.
* Supports two basic modes of operation:
* In BLOCKING mode (default) a call to FillPacket will block until a data packet can be
* read from the net.
* In NON-BLOCKING mode FillPacket asks for HasPendingData on the connection before going
* into the blocking read call. Therefore FillPacket will not block when there is no data.
* Note, however, that fill packet might take some time since HasPendingData==true only means
* that there are some bytes, but not necessaryly a complete packet yet.
*
* USAGE:
*	-	provide the constructor with a port number. The collector will start a blocking listen
*		on this port upon construction. Also pass along a packet prototype, which will be used in the
*		downstream pipe...
*
* @author	Bernd Hentschel
* @date		May, 2004
*
*/

class VISTAINTERPROCCOMMAPI DLVistaNetDataCollector : public DLVistaDataSource
{
public:
	/**
	* create and open data connection.
	* per default this source operates in BLOCKING MODE
	*/
	DLVistaNetDataCollector( const std::string& sHostName,
					int iPort, IDLVistaDataPacket* pPacketPrototype);
	DLVistaNetDataCollector( VistaConnectionIP* pConnection,
							IDLVistaDataPacket* pPacketPrototype);
	virtual ~DLVistaNetDataCollector();

	/**
	* create a fresh, empty data packet
	*/
	IDLVistaDataPacket* CreatePacket();
	/**
	* fill up a packet i.e. deserialize a packet from the byte stream inbound
	* via the TCP/IP connection
	*/
	void FillPacket(IDLVistaDataPacket* p);
	bool HasPacket() const;
	void DeletePacket(IDLVistaDataPacket* p);
	/**
	* Switch between BLOCKING and NON-BLOCKING modes
	*/
	void SetIsBlocking(bool b);
	/**
	* Inquire current mode of operation
	*/
	bool GetIsBlocking() const;

protected:
private:
	VistaConnectionIP* m_pMasterConnection;
	IDLVistaDataPacket* m_pPacketPrototype;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADATACOLLECTOR_H


