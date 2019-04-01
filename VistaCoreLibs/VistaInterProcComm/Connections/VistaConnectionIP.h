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


#ifndef _VISTACONNECTIONIP_H
#define _VISTACONNECTIONIP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaConnection.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSocket;
class VistaSocketAddress;
class VistaThreadEvent;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaConnectionIP : public VistaConnection
{
public:
	enum VistaProtocol
	{
		CT_NONE = -1,
		CT_TCP = 0,
		CT_UDP
	};

	VistaConnectionIP(IVistaSocket *);
	VistaConnectionIP(IVistaSocket *, const std::string &sPeerName, const int iPeerPort);

	/**
	 * USE THIS FOR UDP RECEVIER CONNECTIONS
	 */
	VistaConnectionIP(const std::string &sMyHost, const int iMyPort, bool bUDPBroadcast=false);

	VistaConnectionIP(const VistaProtocol type);

	/**
	 * Creates a CONNECTED socket to sHost, iPort
	 * USE THIS ONLY FOR TCP CLIENT-CONNECTIONS OR UDP SENDER CONNECTIONS
	 */
	VistaConnectionIP(const VistaProtocol type, const std::string &sHost, const int iPort, bool bUDPBroadcast=false);

	virtual ~VistaConnectionIP();

	bool Open();
	void Close();

	virtual void Close ( bool bSkipData );


	bool Connect(const std::string &sHost, const int iPort, bool bBlockingConnect = true);
	bool Connect(const VistaSocketAddress &, bool bBlockingConnect = true);

	/**
	 * This method tries to connect to the host and port as given with SetPeerName()/SetPeerPort()
	 *
	 */
	bool Connect(bool bBlockingConnect = true);


	/**
	 * Returns true iff the ConnectionIP is connected or not. Note that a connection can be
	 * established for UDP type connections as well, but this does not have the same semantics
	 * as for TCP connections+
	 * @return true iff Connect() returned successfully and nothing indicates that a change occured
	 */
	bool GetIsConnected() const;


	/**
	 * PRECONDITION: GetIsOpen() == FALSE.
	 * The information given here will be used in the call to Connect()
	 * @see Connect()
	 */
	void SetPeerName(const std::string &sHostName);

	/**
	 * Returns the peer name <b>as given</b> by the
	 * constructor/SetPeerName()/SetHostNameAndPort()
	 * @return the peer name as given by the user (not by DNS retrieval)
	 */
	std::string GetPeerName() const;

	/**
	 * PRECONDITION: GetIsOpen() == FALSE
	 * The information given here will be used in the call to Connect()
	 * @see Connect()
	 */
	void SetPeerPort(int iPort);

	/**
	 * Returns the peer port <b>as given</b> by the
	 * constructor/SetPeerPort()/SetHostNameAndPort()
	 * @return the peer port as given by the user (not by DNS retrieval)
	 */
	int GetPeerPort() const;

	/**
	 * PRECONDITION: GetIsOpen() == FALSE
	 * The information given here will be used in the call to Connect()
	 * @see Connect()
	 */
	void SetHostNameAndPort(const std::string &sHostName, int iPort);

	/**
	 * Receive shall read some information from the connection
	 *
	 * @param   length   maximum length of bytes to be received
	 * @param  buffer   the received information
	 *
	 * @return  number of received bytes
	 */
	int  Receive (void *buffer, const int length, int iTimeout =0 );

	/**
	 * Send shall write some information to the connection
	 *
	 * @param   buffer   the information which shall be sent
	 *          length   the length of the information to send
	 *
	 * @return  1   if data was completely sent
	 *          0   otherwise
	 */
	int Send    (const void *buffer, const int length);

	VistaProtocol GetProtocol() const;

	bool          SetConnectionType(const VistaProtocol type);

	VistaSocketAddress GetPeerAddress() const;


	/**
	 * Sometimes you pass Connections around and you do not really
	 * know where the original host and port name are from,
	 * the information
	 * @return the local address for this connection.
	 */
	VistaSocketAddress GetLocalAddress() const;


	void SetLingerTime(int iLingerTimeSec);
	int  GetLingerTime() const;

	virtual bool GetIsFine() const;


	// ############################################################
	// IMPLEMENTATION : VistaConnection
	// ############################################################

	virtual int ReadRawBuffer(void *pBuffer, int iLen) ;


	virtual bool HasPendingData() const ;
	virtual unsigned long WaitForIncomingData(int timeout=0);
	virtual unsigned long WaitForSendFinish(int timeout=0);
	virtual unsigned long PendingDataSize() const;

	virtual void SetIsBlocking(bool bBlocking);
	virtual void SetIsBuffering(bool bBuffering);

	virtual void SetShowRawSendAndReceive(bool bShowRaw);
	virtual bool GetShowRawSendAndReceive() const;

	HANDLE GetSocketID() const;

	virtual HANDLE GetConnectionDescriptor() const;
	virtual HANDLE GetConnectionWaitForDescriptor();
	// for the ConnectionIP, the Descriptor enables the socket's event, which
	// forces blocking to OFF, which can be discruptive.
	// // Thus, you can switch it off temporarily
	virtual bool GetWaitForDescriptorEventSelectIsEnabled() const;
	virtual bool SetWaitForDescriptorEventSelectIsEnabled( const bool bSet );
	virtual eConState DetermineConState() const;
	
	protected:
	VistaConnectionIP();

private:
#if defined(WIN32)
	VistaThreadEvent *m_pWaitEvent;
	bool m_bEventSelectIsEnabled;
#endif

	IVistaSocket *m_pSocket;

	std::string m_sPeerName;
	int m_iPeerPort;
	int m_iLingerTime;

	VistaProtocol m_eProtocol;

	bool DoSetConnectionType(const VistaProtocol type);
	VistaConnectionIP(const VistaConnectionIP &);
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //

