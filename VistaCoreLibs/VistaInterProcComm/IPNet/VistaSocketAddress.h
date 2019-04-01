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


#ifndef _VISTASOCKETADDRESS_H
#define _VISTASOCKETADDRESS_H

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
class VistaIPAddress;
struct sockaddr_in;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


/**
 * A socket address combines an IPAddress with a portnumber information.
 * It uses an IPAddress in order to store and transport its information.
 */
class VISTAINTERPROCCOMMAPI VistaSocketAddress
{
public:

	/**
	 * Creates an empty socketaddress that is not really useful for anything,
	 * except as empty container for query functions.
	 */
	VistaSocketAddress();

	VistaSocketAddress(const VistaSocketAddress &);

	/**
	 * Creates a valid socket-address for a given hostname and portnumber.
	 * This is a convinience constructor, as you will ofter have the information
	 * ready as string and int you need e.g. to make a connect to a server or
	 * to bind a server to a specific interface.
	 * @param sHostName the name to query
	 * @param iPort the port to select
	 */
	VistaSocketAddress(const std::string &sHostName, int iPort);

	/**
	 * Constructs a socket address from an ipadress and a port information.
	 * @param rAddress the address which represents the host
	 * @param iPort the port number
	 */
	VistaSocketAddress(const VistaIPAddress &rAddress, int iPort);

	/**
	 * Deconstructs this address.
	 */
	virtual ~VistaSocketAddress();


	/**
	 * You can exchange the ip-address at any time. This does not change the associated
	 * portnumber.
	 * @param rAddr the address to set to
	 */
	void SetIPAddress(const VistaIPAddress &rAddr);

	/**
	 * Returns the current ip-address (might be an empty one) as a stack copy.
	 * @return the address of this socketaddress.
	 */
	VistaIPAddress GetIPAddress() const;

	/**
	 * Stores this ip-address in the given container address.
	 * @param rAddr the sotrage to copy this address to (address part ONLY, no portnumber ;)
	 */
	void GetIPAddress(VistaIPAddress &rAddr);


	/**
	 * Returns the port number of this socket-address. "No" portnumber is symbolized by a portnumber of 0.
	 * @return the port number of this socket, 0 for no port number present
	 */
	int GetPortNumber() const;

	/**
	 * Sets the port number of this socket-address without changing the host information
	 * @param iPort the new port of this socket address.
	 */
	void SetPortNumber(int iPort);


	/**
	 * Returns the binary pattern for this socket-address's ip-address part.
	 * this is mostly used in conjunction with legaci APIs. Try to avoid it as
	 * much as possible. Trade this as an in_addr*
	 */
	void *GetINAddress() const;

	/**
	 * Returns the number of bytes that are used for the binary representation
	 * of this socket's ip-address part. this is mostly used in conjunction with
	 * legaci APIs. Try to avoid it as much as possible. Trade this as a
	 * sizeof(*GetINAddress())
	 * @return the number of bytes for the binary pattern of this socket address's ip-address part
	 */
	int   GetINAddressLength() const;

	/**
	 * Returns whether this socket address is valid or not. Valid means that there is an ip-address
	 * that could be resolved and the port number is >= 0.
	 * @return true iff this address seems to be valid
	 */
	bool GetIsValid() const;

		/**
	 * IP-Addresses can be assigned without drawbacks
	 */
	VistaSocketAddress &operator= (const VistaSocketAddress &);

	VistaSocketAddress &operator= (const sockaddr_in &);

protected:
	/**
	 * the bytepattern from bsd-sockets for the storage of ipv4 addresses.
	 */
	sockaddr_in     *m_myAddress;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

