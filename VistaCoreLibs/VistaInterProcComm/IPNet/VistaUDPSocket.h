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


#ifndef _VISTAUDPSOCKET_H
#define _VISTAUDPSOCKET_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaSocket.h"


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
 * A socket instance for upd-type connections either on the "client" side and
 * on the "server" side. It can be used to send datagrams in connectionless
 * topologies (e.g. steady streams of data).
 */
class VISTAINTERPROCCOMMAPI VistaUDPSocket : public IVistaSocket
{

public:

	/**
	 * creates an empty upd-socket
	 */
	VistaUDPSocket();

	/**
	 * deletes this socket, does not call close
	 */
	virtual ~VistaUDPSocket();

	/**
	 * send a datagram to a peer address. If you connect this socket to a peer, you can use
	 * the regular send-method, in case you do not want to switch partners, feel free to do this.
	 * If you CONNECT a UPD-socket, you can use the regular send from IVistaSocket.
	 * SENDDATAGRAM DOES AN IMPLICIT BIND.
	 * @param pvBuffer the memory to send as a datagram |pvBuffer| >= iLength
	 * @param iLength the number of bytes to send
	 * @param peer the peer address to send this data to
	 * @param flags raw-bsd-udp-sendv flags, you can ignore this
	 */
	int SendDatagramRaw(const void *pvBuffer, const int iLength, const VistaSocketAddress &peer, int flags=0);

	/**
	 * receives a datagram from a peer. If you CONNECT this socket to a peer, you can use
	 * the regular receive-method, in case you do not want to switch partners, feel free to do this.
	 * YOU HAVE TO BE BOUND TO AN ADDRESS IN ORDER TO RECEIVE ON A UDP SOCKET
	 * @param pvBuffer the memory to receive as a datagram |pvBuffer| >= iLength
	 * @param iLength the number of bytes to receive
	 * @param peer the peer address to receive this data from
	 * @param flags raw-bsd-udp-recv flags, you can ignore this
	 * @param iTimeout the timeout after which this method returns when there is no data, 0 for everlasting wait
	 */
	int ReceiveDatagramRaw(void *pvBuffer, const int iLength,
						   const VistaSocketAddress &fromAddress,
						   int iTimeout = 0, int flags=0);

	/**
	 * @return "UDP"
	 */
	std::string GetSocketTypeString() const;

	/**
	 * Turns socket-layer-level buffering to either on or off
	 * @param bBuffering true iff the calls to send/receive should be buffered, false else (direct send/receive)
	 */
	virtual void SetIsBuffering(bool bBuffering);
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAUDPSOCKET_H

