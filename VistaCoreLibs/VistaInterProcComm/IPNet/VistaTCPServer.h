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


#ifndef _VISTATCPSERVER_H
#define _VISTATCPSERVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <string>
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaTCPServerSocket;
class VistaTCPSocket;
class VistaSocketAddress;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is a convinience class for establishing a basic tcp-server architecture.
 * It utilizes a VistaTCPServerSocket for its work. You do not need this, all it
 * does is using the VistaTCPServerSocket in a way someone would do in order
 * to establish a tcp-server. But as this a often a common routine, this class
 * does this for you in a default like way.
 * Usage is simple:
 * <ul>
 *  <li>VistaTCPServer server("my.computer.com", 6678);
 *  <li>while(1) { VistaTCPSocket *client = GetNextClient(); if(client) fork_away(client); };
 * </ul>
 */
class VISTAINTERPROCCOMMAPI VistaTCPServer
{
private:

	/**
	 * The serversocket that this server is bound to in order to accept
	 * incoming connections.
	 */
	VistaTCPServerSocket *m_pServerSocket;
	bool m_bValid;
	int  m_iBacklog;


	void SetupSocket(const VistaSocketAddress &rAdressToPutSocketOn, bool bForceReuse);
protected:
public:

	/**
	 * Creates the serversocket and binds it to the interface as given in sMyHostName and
	 * the specified port.
	 * @param sMyHostName to interface name to bind this server to (a DNS valid host name or dotted ip of an ethernet interface to listen to)
	 * @param iMyPort the port to setup the server to (e.g. 80 for http-services)
	 */
	//VistaTCPServer(const std::string &sMyHostName, int iMyPort);
	VistaTCPServer(const VistaSocketAddress &, int iBacklog = 0, bool bForceReuse=true);
	VistaTCPServer(const std::string &sMyHostName, int iMyPort, int iBacklog = 0, bool bForceReuse=true);

	/**
	 * Closes the server socket if this is open and deletes is.
	 */
	virtual ~VistaTCPServer();

	/**
	 * Sets the server-socket into listen state and accepts incoming connections. It does
	 * accept |iBacklog|-number of concurrently incoming requests before the clients
	 * get the "connection refused" message on the peer side. This defaults to 1,
	 * as this is "usually" what you want.
	 * @param iBacklog the number of cuncurrently accepted connections, defaults to 1
	 * @return a full-blown and ready tcp-socket to send to and receive from or NULL in case of error (always check!)
	 */
	VistaTCPSocket *GetNextClient( int iBacklog = 1);

	bool GetIsValid() const { return m_bValid; };

	VistaSocketAddress GetServerAddress() const;

	void StopAccept();
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H


