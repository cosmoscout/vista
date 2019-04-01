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


#ifndef _VISTAACCEPTOR_H
#define _VISTAACCEPTOR_H

#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <string>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/Concurrency/VistaThreadPool.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaThreadEvent;
class VistaTCPSocket;
class VistaTCPServerSocket;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * @ingroup VistaInterProcComm
 * 
 * @brief Asynchronously accepts incoming TCP connections in a separate
 * thread as a specific instance of IVistaThreadPoolWorkInstance.
 *
 * In the thread routine, a TCP server socket is created and bound to
 * the interface and port passed to the IVistaAcceptor constructor. If
 * a client successfully connects to that socket, the
 * HandleIncomingClient() routine is called. Otherwise, if there was
 * any error during the connect or the user explicitly told
 * IVistaAcceptor to abort listening for clients with a call to
 * SetAbortSignal(), the HandleAbortMessage() routine is
 * called. Children of IVistaAcceptor are required to implement the
 * HandleIncomingClient() and HandleAbortMessage() routines to specify
 * what should happen either on a successful connect or on
 * failure/abort.
 *
 * @todo Comment on buffering/blocking states of the created socket.
 * @todo How are multiple subsequent connections handled?
 */
class VISTAINTERPROCCOMMAPI IVistaAcceptor : public IVistaThreadPoolWorkInstance
{
public:
    /**
	 * Creates a new IVistaAcceptor object.
	 * @param sInterfaceName network interface to listen on
	 * @param iPort port number to listen on
	 */
	IVistaAcceptor(const std::string &sInterfaceName, int iPort);
	virtual ~IVistaAcceptor();

	/**
	 * Instructs the IVistaAcceptor to stop listening for client
	 * connections. This, in turn, calls the HandleAbortMessage
	 * routine. 
	 */ 
	void SetAbortSignal();

	bool GetIsConnected() const;

protected:
    // IVistaThreadedTask implementation
	virtual void PreWork();
	virtual void DefinedThreadWork();

	/**
	 * Called upon successful connection. Children of IVistaAcceptor
	 * override this method to handle incoming connections in a
	 * certain way.
	 */
	virtual bool HandleIncomingClient(VistaTCPSocket *pSocket) = 0;

    /**
	 * Called upon connection failure or if the user instructed the
	 * IVistaAcceptor to stop listening for connections via
	 * SetAbortSignal(). 
	 */
	virtual bool HandleAbortMessage() = 0;

	enum eSockState
	{
		SOCK_NONE=-1,
		SOCK_CLIENT,
		SOCK_EXIT
	};

private:
	VistaThreadEvent  *m_pSyncEvent;
	VistaTCPServerSocket *m_pServer;

	std::string   m_sInterfaceName;
	int           m_iInterfacePort;
	bool          m_bConnected;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
