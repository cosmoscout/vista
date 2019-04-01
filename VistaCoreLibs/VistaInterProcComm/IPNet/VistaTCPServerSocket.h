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


#ifndef _VISTATCPSERVERSOCKET_H
#define _VISTATCPSERVERSOCKET_H

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
 * A TCP-Socket that can be set into Listen-state in order to accept incoming
 * connections. Note that it has to be bound first! Use VistaTCPServer in case
 * you have simple or common usage for tcp-servers.
 */
class VISTAINTERPROCCOMMAPI VistaTCPServerSocket : public VistaTCPSocket
{
private:
protected:

public:

	/**
	 * constructs a VistaTCPServerSocket.
	 */
	VistaTCPServerSocket();

	/**
	 * deconstructs it, but does NOT close it!
	 */
	virtual ~VistaTCPServerSocket();


	/**
	 * turn into listen-state. The backlog is the number that determines the number of
	 * clients that can request connections at the same time before clients get rejected.
	 * non-rejected clients are queued and returned at the next call to accept
	 * @param iBacklog the number of accepted requests that can be served concurrently
	 * @return true iff there was no error ;)
	 */
	bool Listen(int iBacklog);

	/**
	 * blocks this server and returns when a tcp-client requested a connect.
	 * @return a valid tcp-(client)-socket or NULL in case of ERROR (always check!)
	 */
	VistaTCPSocket *Accept() const;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

