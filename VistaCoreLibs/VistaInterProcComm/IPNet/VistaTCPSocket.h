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


#ifndef _VISTATCPSOCKET_H
#define _VISTATCPSOCKET_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaSocket.h"

#include <string>

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
 * This is the base class for all tcp-based communication.
 * you will use this on a server side, when you accepted an incoming connection.
 * Otherwise you might consider to use the server or client socket directly.
 */
class VISTAINTERPROCCOMMAPI VistaTCPSocket : public IVistaSocket
{
	// TCPServerSockets must have access to SetIsConnected()
	friend class VistaTCPServerSocket;
public:
	/**
	 * Creates a tcp-socket for a given socket descriptor. This is useful
	 * when using legacy API.
	 */
	VistaTCPSocket(HANDLE iSockDesc);

	/**
	 * create an empty unbound nohost-socket that is going to be parameterized
	 * later on.
	 */
	VistaTCPSocket();

	/**
	 * deconstructs this socket. does not call close
	 */
	virtual ~VistaTCPSocket();

	virtual bool CloseSocket(bool bSkipRead=false);

	/**
	 * @return "TCP"
	 */
	std::string GetSocketTypeString() const;

	/**
	 * Turns socket-layer-level buffering to either on or off
	 * @param bBuffering true iff the calls to send/receive should be buffered, false else (direct send/receive)
	 */
	virtual void SetIsBuffering(bool bBuffering);

private:
	bool m_bCloseRecursionFlag;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

