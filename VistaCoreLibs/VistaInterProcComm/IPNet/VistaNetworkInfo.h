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


#ifndef _VISTANETWORKINFO_H
#define _VISTANETWORKINFO_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <string>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaIPAddress;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Interface for general network informations. The only way to use this
 * interface is to call VistaNetworkInfo::EnumVistaNetworkInfo(), look for
 * the interesting network and call the member methods on this instance.
 * Note that EnumVistaNetworkInfo() can take some time to complete ;)
 * @see VistaNetworkInfo::EnumVistaNetworkInfo()
 */
class VISTAINTERPROCCOMMAPI VistaNetworkInfo
{
protected:
	VistaNetworkInfo(
					const std::string & name,
					const VistaIPAddress& addr,
					const VistaIPAddress& broadcast,
					const VistaIPAddress& netmask,
					int mtu
					);

public:
	VistaNetworkInfo(const VistaNetworkInfo& ndi);
	~VistaNetworkInfo();

	/**
	 * Returns the Name of the network device
	 */
	const std::string& GetName() const;

	/**
	 * Returns the Address of the network device
	 */
	const VistaIPAddress GetAddress() const;

	/**
	 * Returns the Broadcast address of the network device
	 */
	const VistaIPAddress GetBroadcastAddress() const;

	/**
	 * Returns the Netmask of the network device
	 */
	const VistaIPAddress GetNetmask() const;

	/**
	 * Returns the MTU
	 */
	int GetMTU() const;

	/**
	 * Enumerate all available network devices
	 */
	static bool EnumVistaNetworkInfo(std::vector<VistaNetworkInfo>& devs);

private:
	std::string m_sName;
	VistaIPAddress *m_pipAddr;
	VistaIPAddress *m_pipBroadcast;
	VistaIPAddress *m_pipNetmask;
	int m_iMtu;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //VISTANETWORKINFO_H

