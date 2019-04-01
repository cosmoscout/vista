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


#ifndef _VISTAIPADDRESS_H
#define _VISTAIPADDRESS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

struct in_addr;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is a representation for an IPV4 address. It can be used to do hostname
 * lookups or is given as arguments to VistaSocketAddresses etc. It does
 * try to resolve host names when given as ip-adresses or resolve ip-adresses
 * when host names are given.
 */
class VISTAINTERPROCCOMMAPI VistaIPAddress
{
public:

	/**
	 * Constructs an empty address.
	 */
	VistaIPAddress();

	VistaIPAddress(const VistaIPAddress &);

	/**
	 * Constructs an address for the host-name given.
	 * The hostname can either be a dotted name or a string representation for an
	 * ip-address, e.g. "vrmpc04.rz.rwth-aachen.de" or "130.134.64.204" both resolve
	 * to the same address.
	 * @param sHostName the name for the host to do address-lookup for
	 */
	explicit VistaIPAddress(const std::string &sHostName);

	/**
	 * deconstructs this address.
	 */
	virtual ~VistaIPAddress();


	/**
	 * sets the host name to the argument given. Note that a call to this method does do DNS
	 * lookups, if possible.
	 * @param sHostName the name to lookup
	 */
	virtual void SetHostName(const std::string &sHostName);

	/**
	 * sets this address to the given address-string. Note that the address is a binary
	 * representation that you usually get from bsd-methods, simply wrapped in a string
	 * object. Set this when you have a struct hostent entry with the adress you want to query.
	 * Note that this method does to a DNS lookup.
	 * @param sAddress the binary representation to query the ip-adress from
	 */
	virtual void SetAddress(const std::string &sAddress);

	/**
	 * Returns the hostname that is associated to this address by setting it to the
	 * string reference.
	 * @param sHostName the string to place the hostname in
	 */
	void GetHostName(std::string &sHostName) const;

	/**
	 * returns the hostname for this address as a pointer to a C-string. Note that this
	 * value is READ ONLY.
	 * @return a pointer to the c-string value for this ip-address's host name.
	 */
	const char *GetHostNameC() const;

	std::string GetIPAddressC() const;


	/**
	 * returns a pointer to the binary representation for this ip-address. You should normally
	 * do not feel urged to use this method, but if you must, you can. The return value is
	 * READONLY. See this as in_addr
	 * @return a pointer to the binary representation for this ip-address.
	 */
	const void *GetINAddress() const;


	void SetRawINAddress(struct in_addr *, int, bool bSetName=true);

	/**
	 * Returns the size of the byte-pattern that can be queries be GetINAddress(). This might
	 * be useful in order to work with legacy code.
	 * @return the size in bytes of the binary-representation for this ip-address.
	 */
	int   GetINAddressLength() const;


	/**
	 * returns the address-type for this ip-address, which is currently fixed to ipv4 addresses.(AF_INET)
	 * @return AF_INET
	 */
	int   GetAddressType() const;

	/**
	 * Returns the "dotted-ip-representation" for this address that can be used for debugging or logging
	 * output. Set the hostname to "vrmpc04.rz.rwth-aachen.de" and this string is likely to be something
	 * like "134.130.64.204".
	 * @param sString the string to store the resulting ip-address-string to.
	 */
	virtual void  GetAddressString(std::string &sString) const;

	/**
	 * Returns a string representation for this ip-addresse's type. Currently this is only IPV4 for
	 * all assigned ip-addresses, and undefined for unassigned addresses.
	 * @return either "IPV4" or "UNASSIGNED"
	 */
	void  GetAddressTypeString(std::string &sString) const;

	/**
	 * IP-Addresses can be assigned without drawbacks
	 */
	VistaIPAddress &operator= (const VistaIPAddress &);

	/**
	 * IP-addresses can be explicitly assigned by given a string
	 */
	VistaIPAddress &operator= (const std::string &sHostName);

	/**
	 * this indicates an unassigned ip-address.
	 */
	enum
	{
		VIPA_UNASSIGNED = -1
	};

	bool GetIsValid() const;

private:
protected:
	/// @todo make members private and offer clean accessors!

	bool ResolveHostName(const std::string &sHostName);

	/**
	 * The hostname that is associated with this address. It is empty for an
	 * empty address.
	 */
	std::string m_sHostName;
	/**
	 * a byte-storage for the binary representation as returned by bsd-routines.
	 * We merely use the ability for dynamic resizing etc.
	 */
	std::string m_sInAddress;

	/**
	 * stores the adress-type of this adress. Currently only IPV4 is supported.
	 */
	int         m_iAddressType;

	/**
	 * internal flag to keep valid-state up-to-date. Note that this flag can only be read
	 * and is set to false after unsucessfull calls. An empty address is NOT valid
	 */
	bool        m_bIsValid;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

