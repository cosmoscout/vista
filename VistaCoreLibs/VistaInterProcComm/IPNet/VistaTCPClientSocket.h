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


#ifndef _VISTATCPCLIENTSOCKET_H
#define _VISTATCPCLIENTSOCKET_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaTCPSocket.h"

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
 * This is the counterpart to a VistaTCPServerSocket. It is used to ease client
 * operations in conjucntion with a tcp-based client-server topology.
 * It does define the ConnectToServer() method that simply connects this
 * client to the given host on the given port. TCP-clients can now easily be
 * realized by the following sequence:
 * <ol>
 *  <li>VistaTCPClientSocket client;
 *  <li>if( client.ConnectToServer("www.whatyouwant.com", 80) ) { std::cout << "connected.\n"; client.CloseSocket() };
 * </ol>
 */
class VISTAINTERPROCCOMMAPI VistaTCPClientSocket : public VistaTCPSocket
{
private:
protected:

public:

	/**
	 * Creates a TCP socket that is used to connect to a tcp-server socket
	 * somewhere on the net.
	 */
	VistaTCPClientSocket();

	/**
	 * Does NOT close the socket.
	 */
	virtual ~VistaTCPClientSocket();


	/**
	 * Easy, simple and failsafe way to connect to a tcp-server.
	 * @param sServerHostName the name of the server as a string (either FQDN or dotted number-notation
	 * @param iServerPort the portnumber to connect to (e.g. 80 for http access)
	 * @return false if something went wrong (e.g. connection was refused)
	 */
	bool ConnectToServer(const std::string &sServerHostName, int iServerPort);
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

