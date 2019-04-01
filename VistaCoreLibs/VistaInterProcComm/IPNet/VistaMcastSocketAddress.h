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


#ifndef _VISTAMCASTSOCKETADDRESS_H
#define _VISTAMCASTSOCKETADDRESS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <string>

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaSocketAddress.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaMcastIPAddress;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


/**
 * A multicast socket address combines an multicast group ip address with
 * a port number information.It can be used with VistaMcastIPAddress and
 * VistaMcastSocket.
 */
class VISTAINTERPROCCOMMAPI VistaMcastSocketAddress :  public VistaSocketAddress
{
private:
protected:
public:

	/**
	 * Creates an empty socketaddress that is not really useful for anything,
	 * except as empty container for query functions.
	 */
	VistaMcastSocketAddress();

	/**
	 * copy constructor
	 */
	VistaMcastSocketAddress(const VistaMcastSocketAddress &);

	VistaMcastSocketAddress(const VistaSocketAddress &);

	/**
	 * Creates a valid multicast socket-address for a given multicast group ip
	 * address (only numbers-and-dots notation is valid) and portnumber. The
	 * constructor will check the validation of the given multicast ip address.
	 * @param sHostName multicast ip address (e.g. "234.5.6.7")
	 * @param iPort the port to select
	 */
	VistaMcastSocketAddress(const std::string &sHostName, int iPort);

	/**
	 * Constructs a multicast socket address from an multicast ip adress and
	 * a port information.
	 * @param rAddress the address which represents the multicast group
	 * @param iPort the port number
	 */
	VistaMcastSocketAddress(const VistaMcastIPAddress &rAddress, int iPort);

	/**
	 * Deconstructs this address.
	 */
	virtual ~VistaMcastSocketAddress();


	/**
	 * You can exchange the ip-address at any time. This does not change the associated
	 * portnumber.
	 * @param rAddr the address to set to
	 */
	//void SetIPAddress(const VistaMcastIPAddress &rAddr);

	/**
	 * Returns the current multicast ip-address (might be an empty one) as a stack copy.
	 * @return the address of this socketaddress.
	 */
	VistaMcastIPAddress GetMcastIPAddress() const;

	/**
	 * Stores this multicast ip-address in the given container address.
	 * @param rAddr the sotrage to copy this address to (address part ONLY, no portnumber ;)
	 */
	void GetMcastIPAddress(VistaMcastIPAddress &rAddr);


	/**
	 * IP-Addresses can be assigned without drawbacks
	 */
	VistaMcastSocketAddress &operator= (const VistaMcastSocketAddress &);

	VistaMcastSocketAddress &operator= (const sockaddr_in &);

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMCASTSOCKETADDRESS_H

