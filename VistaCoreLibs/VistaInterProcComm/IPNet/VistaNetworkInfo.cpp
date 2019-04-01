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


//#ifndef WIN32
//#include <sys/ioctl.h>
//# ifdef HAVE_SYS_SOCKIO_H
//# include <sys/sockio.h>
//# endif
//
//#else
#if defined(WIN32)

#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined (LINUX)
#include <net/if.h>
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <arpa/inet.h>
	#include <sys/times.h>
	#include <sys/time.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <time.h>

#elif defined (SUNOS) || defined (IRIX)
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <sys/sockio.h>
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/filio.h>
	#include <arpa/inet.h>
	#include <sys/times.h>
	#include <sys/time.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <time.h>
#include <net/if.h>

#elif defined (HPUX)
#include <net/if.h>
	#include <unistd.h>          // ioctl(), FIONREAD
	#include <netdb.h>           // getprotoent() libc-Version!,gethostbyname()
	#include <sys/socket.h>      // socket(),gethostbyname(),connect()
	#include <arpa/inet.h>
	#include <sys/times.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <time.h>

#elif defined (DARWIN)
	#include <VistaBase/VistaExceptionBase.h>

#else
	#error You have to define the target platform in order to compile ViSTA

#endif

#include "VistaNetworkInfo.h"
#include "VistaUDPSocket.h"
#include "VistaIPAddress.h"

#include <cstring>
#include <cstdio>
#include <string>

using namespace std;


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaNetworkInfo::VistaNetworkInfo(const std::string& name, 
									 const VistaIPAddress& ipAddr,
									 const VistaIPAddress& ipBroadcast, 
									 const VistaIPAddress& ipNetmask,
									 int mtu)
: m_sName(name)
, m_iMtu(mtu)
, m_pipAddr(new VistaIPAddress(ipAddr))
, m_pipBroadcast(new VistaIPAddress(ipBroadcast))
, m_pipNetmask(new VistaIPAddress(ipNetmask))
{
}

VistaNetworkInfo::VistaNetworkInfo(const VistaNetworkInfo& ndi)
: m_sName(ndi.m_sName)
, m_iMtu(ndi.m_iMtu)
, m_pipAddr(new VistaIPAddress(*ndi.m_pipAddr))
, m_pipBroadcast(new VistaIPAddress(*ndi.m_pipBroadcast))
, m_pipNetmask(new VistaIPAddress(*ndi.m_pipNetmask))
{
	//printf("hn: %s\n", ndi.m_pipAddr->GetHostNameC());
}

VistaNetworkInfo::~VistaNetworkInfo()
{
	delete m_pipAddr;
	delete m_pipBroadcast;
	delete m_pipNetmask;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

const string& VistaNetworkInfo::GetName() const	
{ 
	return m_sName; 
}

// Returns the Address of the network device
const VistaIPAddress VistaNetworkInfo::GetAddress() const	
{ 
	return *m_pipAddr; 
}

// Returns the m_ipBroadcast address of the network device
const VistaIPAddress VistaNetworkInfo::GetBroadcastAddress() const	
{ 
	return *m_pipBroadcast; 
}

// Returns the m_ipNetmask of the network device
const VistaIPAddress VistaNetworkInfo::GetNetmask() const	
{ 
	return *m_pipNetmask; 
}

// Returns the MTU
int VistaNetworkInfo::GetMTU() const	
{ 
	return m_iMtu; 
}


bool VistaNetworkInfo::EnumVistaNetworkInfo(vector<VistaNetworkInfo>& devs)
{
	devs.clear();
	VistaUDPSocket sock;
	if(sock.OpenSocket())
	{
#ifndef WIN32
	#ifdef DARWIN
		VISTA_THROW( "VistaNetworkInfo::EnumVistaNetworkInfo() Not implementd", 1 );
	#else

		char buffer[8192];
		struct ifconf ifc;
		int fd = sock.GetSocketID();
		
		ifc.ifc_len = sizeof(buffer);
		ifc.ifc_buf = buffer;

		if(ioctl(fd, SIOCGIFCONF, &ifc) == -1)
			return false;

		VistaIPAddress m_ipAddr;
		VistaIPAddress brdaddr;
		VistaIPAddress maskaddr("255.255.255.255");
		int mtu = 0;

		int count = ifc.ifc_len / sizeof(ifreq);
		devs.reserve(count); 
		for(int i = 0; i < count; ++i)
		{
			if(ifc.ifc_req[i].ifr_addr.sa_family != AF_INET)
				continue;

			m_ipAddr.SetRawINAddress(&((sockaddr_in&)ifc.ifc_req[i].ifr_addr).sin_addr,sizeof(struct in_addr));

			struct ifreq devifreq;
			#if defined(LINUX)
			strcpy(devifreq.ifr_ifrn.ifrn_name, ifc.ifc_req[i].ifr_ifrn.ifrn_name);
			#else
			strcpy(devifreq.ifr_name, ifc.ifc_req[i].ifr_name);
			#endif

			if(ioctl(fd, SIOCGIFBRDADDR, &devifreq) != -1)
				brdaddr.SetRawINAddress(&((sockaddr_in&)devifreq.ifr_broadaddr).sin_addr, sizeof(struct in_addr));

			if(ioctl(fd, SIOCGIFNETMASK, &devifreq) != -1)
				maskaddr.SetRawINAddress(&((sockaddr_in&)devifreq.ifr_addr).sin_addr, sizeof(struct in_addr));

	#if defined(SIOCGLIFMTU)
			struct lifreq devlifreq;
			if(ioctl(fd, SIOCGLIFMTU, &devlifreq) == -1)
				mtu = 0;
			else
				mtu = devlifreq.lifr_mtu;
	#else
#if !defined(HPUX)
			if(ioctl(fd, SIOCGIFMTU, &devifreq) == -1)
				mtu = 0;
			else
				mtu = devifreq.ifr_mtu;
#endif
	#endif
	#if defined(LINUX)
			devs.push_back(VistaNetworkInfo(ifc.ifc_req[i].ifr_ifrn.ifrn_name, m_ipAddr, brdaddr, maskaddr, mtu));
	#else
			devs.push_back(VistaNetworkInfo(ifc.ifc_req[i].ifr_name, m_ipAddr, brdaddr, maskaddr, mtu));
	#endif
		}
#endif
	#else // WIN32


		SOCKET s = (SOCKET)sock.GetSocketID();

		char outbuff[8192];
		DWORD outlen;
		DWORD ret = WSAIoctl(s, SIO_GET_INTERFACE_LIST, 0, 0, outbuff, sizeof(outbuff), &outlen, 0, 0);
		if(ret == SOCKET_ERROR)
			return false;

		INTERFACE_INFO* iflist = (INTERFACE_INFO*)outbuff;
		int ifcount = outlen / sizeof(INTERFACE_INFO);

		VistaIPAddress ipAddr;
		VistaIPAddress brdaddr;
		VistaIPAddress maskaddr("0.0.0.0");

		for(int i = 0; i < ifcount; ++i)
		{
			ipAddr.SetRawINAddress(&((sockaddr_in&)iflist[i].iiAddress).sin_addr, sizeof(struct in_addr));
			brdaddr.SetRawINAddress(&((sockaddr_in&)iflist[i].iiBroadcastAddress).sin_addr, sizeof(struct in_addr));
			maskaddr.SetRawINAddress(&((sockaddr_in&)iflist[i].iiNetmask).sin_addr, sizeof(struct in_addr));

			devs.push_back(VistaNetworkInfo("", ipAddr, brdaddr, maskaddr, -1));
		}
	#endif
		sock.CloseSocket();

	} // if (OpenSocket())
		return true;

}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


