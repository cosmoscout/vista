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


#ifndef _VISTASOCKET_H
#define _VISTASOCKET_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaSocketAddress.h"

#include <string>
#include <VistaBase/VistaBaseTypes.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaThreadEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * The base-interface for socket-communication. You should be able to do anything
 * with IVistaSockets that you believe to do with sockets usually ;)
 * Note that there are certain restrictions for UDP sockets that are not for TCPs,
 * but you should be aware of that and use TCPSockets wherever you use TCP streams
 * and the like with UDP. Use IVistaSockets where this does not really care.
 * Sockets are used for low-level transport of information vie user-predefined byte
 * arrays. They do only send and reveice raw-byte-fields. Use Connections in order
 * to have a higher level access to this kind of transport.
 */
class VISTAINTERPROCCOMMAPI IVistaSocket
{
public:
	/**
	 * Returns the socket-descriptor for this socket. You can use this for low-level
	 * operations on this socket.
	 * @param the socket-id for this socket, -1 for a non open socket
	 */
	HANDLE  GetSocketID() const;

	/**
	 * DOES NOT CLOSE THIS SOCKET.
	 * Do call CloseSocket() explicitly BEFORE deleting a socket!
	 */
	virtual ~IVistaSocket();


	/**
	 * Creates a socket-descriptor and NO MORE.<br>
	 * PRECONDITION: IsOpen() == false<br>
	 * OPENING A SOCKET DOES NOT MEAN TO CONNECT IT TO ANYTHING.
	 * If the socket could be opened, the values for buffering and blocking will be set to this
	 * socket.
	 * @return true iff the socket-descriptor could be opened successfully.
	 */
	virtual bool OpenSocket();

	/**
	 * Closes a socket.You can call this more than once, although calling close on a closed socket
	 * returns false.
	 * @return true iff the socket could be closed, false else
	 */
	virtual bool CloseSocket(bool bSkipRead=false);


	/**
	 * Returns whether the socket uses socket-layer-level-buffering
	 * @return true iff the socket uses socket-layer-level buffering
	 */
	bool GetIsBuffering() const;

	/**
	 * Returns whether the socket uses socket-layer-level blocking.
	 * @return true iff the socket blocks on calls to send and receive
	 */
	bool GetIsBlocking() const;


	/**
	 * Turns socket-layer-level buffering to either on or off
	 * @param bBuffering true iff the calls to send/receive should be buffered, false else (direct send/receive)
	 */
	virtual void SetIsBuffering(bool bBuffering);

	/**
	 * Tunrs socket-layer-level blocking to either on or off.
	 * @param bBlocking true if calls to send/receive will block the caller until these calls return, false else.
	 */
	virtual void SetIsBlocking(bool bBlocking);


	/**
	 * Returns whether this socket is bound to an address or if it is still unbound.
	 * @return true iff this socket it bound to an address (this is usually the case for tcp-server-sockets)
	 */
	bool GetIsBoundToAddress() const;

	/**
	 * Returns whether this socket is connected to a peer or nor. A tcp-peer is the other-side-partner
	 * to a bi-directional peer-to-peer communication, while a udp-peer is only a stored address for
	 * send/receive calls.
	 * @return true iff this socket is connected to a peer address
	 */
	bool GetIsConnected() const;

	/**
	 * Returns whether the socket descriptor is valid or not.
	 * @return true after a successfull call to OpenSocket(), false else or after CloseSocket()
	 */
	bool GetIsOpen() const;

	/**
	 * currently unused.
	 * @todo fix me
	 */
	bool GetIsValid() const;


	/**
	 * Returns a human readably representation for this socket's type
	 * @return either ("UDP" or "TCP")
	 */
	virtual std::string GetSocketTypeString() const = 0;

	/**
	 * Binds this socket to the given socketaddress. Only opened sockets can be bound.
	 * This method is used in contet with tcp-server-sockets. Clients use connect. You bind servers
	 * to a valid name for the host that this socket should respond to and an open port-number.<br>
	 * PRECONDITION: IsOpen() == true
	 * @param sAddr the socket-address to bind this socket to.
	 * @return true iff this socket could be bound to the given address
	 */
	virtual bool BindToAddress(const VistaSocketAddress &sAddr);

	/**
	 * Connects this socket to the given SocketAddress as a peer. The semantics are somewhat different.
	 * Clients use connect to connect to server-sockets. Server sockets are bound to an address.
	 * between tcp-sockets and upd-sockets, but still they both can be connected.
	 * @param sAddr the socket-address to connect to
	 */
	virtual bool ConnectToAddress(const VistaSocketAddress &sAddr);


	/**
	 * Returns the SocketAddress of the local socket (this socket itself).
	 * @see GetPeerSockName()
	 * @param sAddr the container to store the information in
	 * @return true if this socket has an address, else iff not
	 */
	virtual bool GetLocalSockName(VistaSocketAddress &sAddr) const;

	/**
	 * Returns the name for the peer socket. The peer has to be present, of course, that is
	 * after an accept or connect.
	 * @param sAddr the container to store the information in
	 * @return true iff this socket has a peer, else iff not.
	 */
	virtual bool GetPeerSockName(VistaSocketAddress &sAddr) const;


	/**
	 * Sends a number of raw bytes to the peer, if present.
	 * @param pvBuffer the memory to send, |pvBuffer| >= iLength
	 * @param iLength the number of bytes to send
	 * @param iFlags raw bsd-tcp/ip-flags to use for sending, usually you can ignore this
	 * @return the number of bytes that were sent
	 */
	virtual int  SendRaw(const void *pvBuffer, const int iLength, int iFlags = 0);

	/**
	 * Receives a number of bytes from the peer and stores them in pvBuffer. Note that the
	 * behaviour of this method is bound to the chosen blocking strategies.
	 * @param pvBuffer the memory storage to write the received bytes to |pvBuffer| >= iLength
	 * @param iLength the number of bytes to receive at maximum
	 * @param iTimout the timeout to wait until either iLength bytes came in or the time is over
	 * @param flags raw bsd-tcp-ip flags to be used with receive, you should normally ignore them
	 * @return the number of bytes actually read.
	 */
	virtual int  ReceiveRaw(void *pvBuffer, const int iLength, int iTimeout=0, int flags = 0);


	/**
	 * indicates whether there are bytes that can be read or not at the interface. As this test
	 * simply tries to get the number of bytes available and compares this to 0 in order to see
	 * whether there are bytes or not, the result of this query is returned for convinience
	 * (querying this can be expensive)
	 * @return the number of bytes ready to be read with receive
	 */
	unsigned long HasBytesPending() const;

	/**
	 * puts the caller to sleep until either the timeout in millisecs is over or there is
	 * data ready to be read from this socket.
	 * A blocking socket should not return 0 here, this indicates that the socket is closed.
	 * @param iTimout the number of millisecs to wait, set to 0 for sleeping ever after or until data is ready.
	 * @return the number of bytes ready (~0 in case of timeout)
	 */
	unsigned long WaitForIncomingData(int iTimeout);

	unsigned long WaitForSendFinish(int iTimeout);


	void SetShowRawSendAndReceive(bool bShowRaw);
	bool GetShowRawSendAndReceive() const;

	/**
	 * currently unused, avoid this
	 * @todo fix me or remove me
	 */
	bool Flush();


	// -------------- new ---------------

	int GetPermitBroadcast() const;
	void SetPermitBroadcast(int iOn);
	int GetDebugInfoRecord() const;
	int GetDontRoute() const;
	void SetDontRoute(int iOn);
	int GetErrorStatus() const;
	int GetKeepConnectionsAlive() const;
	void SetKeepConnectionsAlive(int iOn);
	/**
	 * @return the linger time in secs or 0 iff no lingering enabled.
	 */
	int GetLingerOnClose() const;

	/**
	 *
	 * @param iLingerTime iff > 0 then lingering is set to true and the linger time is set as given
	 */
	 void SetLingerOnClose(int iLingerTimeInSecs);

	int GetReceiveBufferSize() const;
	void SetReceiveBufferSize(int iBufSize);
	int GetSendBufferSize() const;
	void SetSendBufferSize(int iBufSize);
	int GetSocketType() const;
	bool GetSocketReuse() const;
	void SetSocketReuse(bool bReuse);


	bool VerifyBlocking();

#if defined(WIN32)
	void EnableEventSelect(VistaThreadEvent *pEvent, long lEvents);
	void DisableEventSelect(VistaThreadEvent *pEvent);
	VistaThreadEvent *GetWaitEvent() const;
#endif

protected:

	/**
	 * a protected constructor that is fille by subclasses.
	 * @param iDomain has to be PF_INET
	 * @param iType either SOCK_STREAM or SOCK_DGRAM
	 * @param iProtocol depending on iType either IPPROTO_TCP or IPPROTO_UDP
	 */
	IVistaSocket(int iDomain, int iType, int iProtocol);

	/**
	 * low-level call to GetSockOpt. See bsd-information for more.
	 */
	bool GetSockOpt(int iCodeLevel, int iOptionName, void *pOptionData, int &iDataLength) const;

	/**
	 * low-level call to SetSockOpt. See bsd-information for more.
	 */
	bool SetSockOpt(int iCodeLevel, int iOptionName, void *pOptionData, int iDataLength);

	/**
	 * Sets the socket id to this socket. This is useful for situations where socket-descriptors
	 * are present, but no IVistaSocket-instance (e.g. when accepting an incomin connection.
	 * Use with care.
	 * @param iSockID the new sockID to set this socket to
	 * @bool false if this socket already has a valid (!=-1) socket id.
	 */
	bool SetSocketID(HANDLE iSockID);

	/**
	 * helper function that is likely to be removed.
	 * @return true iff the error function indicated WOULDBLOCK
	 */
	static bool PrintErrorMessage(const std::string &sMethodName);

	void SetIsConnected(bool bConnected);

	void SetErrorState(bool bState);
	bool GetErrorState() const;
private:

	/**
	 * This socket's address when bound to one
	 */
	VistaSocketAddress m_rAddress;

	/**
	 * The domain is the used network-layer (PF_INET only, currently).
	 */
	int             m_iDomain;

	/**
	 * The type is either SOCK_STREAM or SOCK_DGRAM, no other type supported
	 * right now.
	 */
	int             m_iType;

	/**
	 * The protocol is either TCP or UDP, currently nothing else supported.
	 */
	int             m_iProtocol;

	/**
	 * indicates whether bound() was called on this socket, e.g. if it is bound
	 * to a specific address.
	 */
	bool            m_bIsBound;

	/**
	 * indicated whether connect was called on this socket. TCP-sockets establish
	 * a connection, UDP sockets store their receiver to send to or to receive from.
	 */
	bool            m_bIsConnected;

	/**
	 * indicates whether this socket has blocking behaviour
	 */
	bool            m_bIsBlocking;

	/**
	 * indicated whether this socket is buffering (ip-layer buffering ONLY)
	 */
	bool            m_bIsBuffering;

	bool            m_bShowRaw;

	bool            m_bErrorState;


	/**
	 * the associated socketid for this socket.
	 */
	HANDLE             m_iSocketID;

	VistaThreadEvent *m_pWaitEvent;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

