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


#ifndef _VISTAMCASTSOCKET_H
#define _VISTAMCASTSOCKET_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include "VistaUDPSocket.h"
#include "VistaMcastSocketAddress.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaMcastSocketAddress;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A socket instance for upd multicast either for the "non-member sender" side and
 * for the "member receiver" side. It can be used to send datagrams to multicast
 * group or receiver from it as well.
 */
class VISTAINTERPROCCOMMAPI VistaMcastSocket : public VistaUDPSocket
{
public:

	/**
	 * creates an empty multicast upd-socket
	 */
	VistaMcastSocket();

	/**
	 * deletes this socket, does not call close
	 */
	virtual ~VistaMcastSocket();

	/**
	 * send a datagram to a preset multicast group. If you just want to send to a multicast group
	 * without receiving data from it, it's not necessary to join in the multicast group. There are
	 * two ways to do it as following:
	 * 1)via VistaMcastSocket
	 * The destination multicast group ip address and port number should be set and recorded in
	 * the VistaMcastSocket instance via "SetMulticast(..)" before this function is called.
	 * OpenSocket() -> SetMulticast("multicast ip and port") -> SendtoMulticast().
	 * 2)via VistaUDPSocket
	 * OpenSocket() -> ConnectToAddress("multicast ip and port") -> SendRaw().
	 * @param pvBuffer the memory to send as a datagram |pvBuffer| >= iLength
	 * @param iLength the number of bytes to send
	 * @param flags raw-bsd-udp-sendv flags, you can ignore this
	 */
	int SendtoMulticast(void *pvBuffer, const int iLength, int flags = 0);

	/**
	 * receives a datagram from a multicast group.The socket should join in the multicast group
	 * before it can receive all of the data for this group.To receive the group data, all members
	 * should use the same port for this multicast communication.
	 * OpenSocket() -> SetMulticast("multicast ip and portXX") -> BindToAddress("local ip and portXX")
	 *  -> MulticastJoin() -> ReceivefromMulticast()
	 * @param pvBuffer the memory to receive as a datagram |pvBuffer| >= iLength
	 * @param iLength the number of bytes to receive
	 * @param flags raw-bsd-udp-recv flags, you can ignore this
	 * @param iTimeout the timeout after which this method returns when there is no data, 0 for everlasting wait
	 */
	int ReceivefromMulticast(void *pvBuffer, const int iLength, int iTimeout = 0, int flags=0);

	/**
	 * @return "MULTICAST-UDP"
	 */
	std::string GetSocketTypeString() const;


	bool GetIsJoinedMulticast() const;
	bool GetIsSetMulticast() const;
	int  GetTTL() const;
	bool GetLoopBack() const;

	void SetIsJoinedMulticast( bool bJoin);
	void SetIsSetMulticast( bool bSet);

	bool SetMulticastTTL( int iTTL);
	bool SetMulticastLoopBack( bool bLoop);

	/**
	 * to keep the multicast group address (ip and port) in VistaMcastSocket for further use
	 * @param sAddr valid multicast socket address (its validity is checked by VistaMcastSocketAddress)
	 */
	bool SetMulticast(const VistaMcastSocketAddress &sAddr);

	/**
	 * join in multicast group

	 * @param bLoop Controls loopback of multicast traffic.
	 * In Winsock, the IP_MULTICAST_LOOP option applies only to the receive path.
	 * In the UNIX version, the IP_MULTICAST_LOOP option applies to the send path.
	 * For example, applications ON and OFF (which are easier to track than X and Y)
	 * join the same group on the same interface; application ON sets the
	 * IP_MULTICAST_LOOP option on, application OFF sets the IP_MULTICAST_LOOP option off.
	 * If ON and OFF are Winsock applications, OFF can send to ON, but ON cannot sent to OFF.
	 * In contrast, if ON and OFF are UNIX applications, ON can send to OFF, but OFF cannot send to ON

	 * @param ttl Sets time-to-live, controls scope of a multicast session
	 * TTL values			scope of DUMRP packet
	 *		0			restricted to the same host
	 *		1			restricted to the same subnetwork
	 *		32			restricted to the same site
	 *		64			restricted to the same region
	 *		128			restricted to the same continent
	 *		255			unrestricted in scope
	 */
	bool JoinMulticast( const VistaIPAddress &rNIFAddr);

	bool DropMulticast( );

	//iMode = 0 -- sender; iMode = 1 -- receiver
	bool ActiveMulticast( int iMode, const std::string &sIpLocalString, const std::string &sIpMultiString, int iPort, int iTTL=1, bool bLoop=true);

protected:
private:
	VistaMcastSocketAddress  m_multiAddress;
	int					m_iTTL;
	bool				m_bLoop;
	bool				m_bIsJoin;
	bool				m_bIsSetMulticast;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMCASTSOCKET_H

