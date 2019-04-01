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


#ifndef _VISTASERVICEINDICATOR_H
#define _VISTASERVICEINDICATOR_H

#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <string>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/Concurrency/VistaThreadTask.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaUDPSocket;
class VistaSocketAddress;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


struct VISTAINTERPROCCOMMAPI VistaAppMsg
{
	char m_acHost[256];
	char m_acPort[10];
	char m_acAck[3];
	char m_cState;
	char m_acCtrl;
};


/**
 * Ping-like service indicator that can be used as a thread or within
 * one's own PC. It uses UDP-datagrams to send msgs from one point to
 * another, then answering.
 */
class VISTAINTERPROCCOMMAPI VistaServiceIndicator : public IVistaThreadedTask
{
public:
	/**
	 * Constructor
	 * @param adr the socket adress for *this* host (UDP)
	 */
	VistaServiceIndicator (const VistaSocketAddress &adr);


	/**
	 * Constructor.
	 * @param sHost name of ethernet segment on *this* host
	 * @param iPort port to bind to
	 */
	VistaServiceIndicator(const std::string &sHost, int iPort);
	virtual ~VistaServiceIndicator();


	/**
	 * In case you did not know in advance or like to change the
	 * address *before* entering Pong
	 * @see Pong()
	 * @param sHost host to bind to during Pong
	 * @param iPort port to bind to during Pong
	 */
	void SetAddress(const std::string &sHost, int iPort);

	/**
	 * Returns the address the UDP-Socket is bound to on *this*
	 * host.
	 * @return the local adress for the socket
	 */
	VistaSocketAddress GetSocketAddress() const;

	/**
	 * Init will be called automatically when used as a thread-task.
	 * If you do not call ThreadWork(), but rather whish to Ping(),
	 * call Init *before* you do this.
	 * @see Ping()
	 * @see ThreadWork()
	 * @return false if the socket could not be bound or opened
	 */
	bool Init();


	/**
	 * @param sPeerName the peer, which should be running Pong() right now
	 * @param iPeerPort the port of the \"listening\" peer
	 * @param bQuit indicate the end the service-indicator on the other side
	 * @param iWaitTime wait time to wait for a Pong() in msecs
	 * @return true iff the Ping() returned a Pong()
	 */
	bool Ping(const std::string &sPeerName, int iPeerPort, bool bQuit = false, int iWaitTime=50);

	/**
	 * Pong is a looped action, it will return on communication error or the arrival of
	 * a quit msg from the Ping() side
	 * @return true quit msg arrived, false else (error)
	 */
	bool Pong();

		void SetControlFlag(char c);
		char GetControlFlag() const;
protected:
	virtual void PreWork();
	virtual void PostWork();
	virtual void DefinedThreadWork();

	virtual bool HandleQuitMessage();

	virtual bool HandleMessage(const std::string &sMsg);
private:
	VistaUDPSocket *m_pServiceSocket;
	VistaSocketAddress *m_pAdr;
	char m_cCtrl;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASERVICEINDICATOR_H
