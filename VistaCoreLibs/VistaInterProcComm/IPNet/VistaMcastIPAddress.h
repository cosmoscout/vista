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


#ifndef _VISTAMCASTIPADDRESS_H
#define _VISTAMCASTIPADDRESS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaIPAddress.h"

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
 * This is a representation for an IPV4 multicast address (Class D). It can be given as
 * arguments to VistaMcastSocketAddresses, but can not be used to do hostname lookup as
 * VistaIPAddress. Therefore, only a numbers-and-dots notation can be gived to the
 * constructor or SetHostName(), and no host name lookup will be done. However, the validation
 * as a multicast group ip address will be checked instead.This class is designed to keep
 * the compatibility of multicast ip address with normal ip address.
 */
class VISTAINTERPROCCOMMAPI VistaMcastIPAddress : public VistaIPAddress
{
private:
protected:
public:

	/**
	 * Constructs an empty address.
	 */
	VistaMcastIPAddress();


	VistaMcastIPAddress(const VistaMcastIPAddress &);

	VistaMcastIPAddress(const VistaIPAddress &);

	/**
	 * Constructs an address for a multicast address given.
	 * The multicast address can only be a string representation for an
	 * ip-address, e.g. "234.5.6.7", and belong to "Class D" ip address.
	 * @param sHostName the multicast address
	 */
	explicit VistaMcastIPAddress(const std::string &sHostName);

	/**
	 * deconstructs this address.
	 */
	virtual ~VistaMcastIPAddress();


	/**
	 * sets the multicast address to the argument given.
	 * This call does validity check of the given multicast address.
	 * @param sHostName the multicast address
	 */
	void SetHostName(const std::string &sHostName);

	/**
	 * sets this address to the given address-string. Note that the address is a binary
	 * representation that you usually get from bsd-methods, simply wrapped in a string
	 * object. Set this when you have a struct hostent entry with the adress you want to query.
	 * @param sAddress the binary representation to query the ip-adress from
	 */
	void SetAddress(const std::string &sAddress);

	/**
	 * Returns a string representation for this ip-addresse's type. Currently this is only IPV4 for
	 * all assigned ip-addresses, and undefined for unassigned addresses.
	 * @return either "IPV4-Multicast" or "UNASSIGNED"
	 */
	void GetAddressTypeString( std::string &sString ) const;


	/**
	 * IP-Addresses can be assigned without drawbacks
	 */
	VistaMcastIPAddress &operator= (const VistaMcastIPAddress &);

	/**
	 * IP-addresses can be explicitly assigned by given a string
	 */
	VistaMcastIPAddress &operator= (const std::string &sHostName);

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMCASTIPADDRESS_H

