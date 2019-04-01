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


#include "VistaMcastIPAddress.h"

#include <VistaBase/VistaStreamUtils.h>

#if defined(WIN32)
#pragma warning(disable: 4996)
#endif


#if defined(WIN32)
//#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined (LINUX) || defined (DARWIN)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <arpa/inet.h>

#elif defined (SUNOS) || defined (IRIX)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/filio.h>
	#include <arpa/inet.h>
	#if !defined(INADDR_NONE)
	#define INADDR_NONE 0xFFFFFFFF
	#endif

#elif defined (HPUX)
	#include <unistd.h>          // ioctl(), FIONREAD
	#include <netdb.h>           // getprotoent() libc-Version!,gethostbyname()
	#include <sys/socket.h>      // socket(),gethostbyname(),connect()
	#include <arpa/inet.h>

#else
	#error You have to define the target platform in order to compile ViSTA

#endif

#include <iostream>
using namespace std;


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaMcastIPAddress::VistaMcastIPAddress()
{
}

VistaMcastIPAddress::VistaMcastIPAddress(const string &sHostName)
{
	//m_iAddressType = VIPA_UNASSIGNED;
	//m_bIsValid     = false;
	SetHostName(sHostName);
}

VistaMcastIPAddress::VistaMcastIPAddress(const VistaMcastIPAddress &adr)
 : VistaIPAddress( adr )
{
   /*
	m_iAddressType = adr.m_iAddressType;
   m_sHostName = adr.m_sHostName;
   m_bIsValid = adr.m_bIsValid;
   
   if(m_iAddressType != VIPA_UNASSIGNED)
	m_sInAddress.assign(adr.m_sInAddress.begin(), adr.m_sInAddress.end());
	*/
}

VistaMcastIPAddress::VistaMcastIPAddress(const VistaIPAddress &adr)
 : VistaIPAddress( adr )
{

}

VistaMcastIPAddress::~VistaMcastIPAddress()
{

}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


void VistaMcastIPAddress::SetHostName(const string &sHostName)
{
	if(sHostName != m_sHostName)
	{
		if(isdigit(sHostName[0]))
		{
			// this works, this means that we have numbers-and-dots
			// notation ip address in sHostName. We should check whether
			// this address is a valid multicast address.
			int iAddr = inet_addr(sHostName.c_str());
			int iAddrHl = ntohl(iAddr);
			if ( ((unsigned int)iAddr != INADDR_NONE)  &&  IN_MULTICAST( iAddrHl ))
			{
				//reserved multicast address for local scope
				int iReservedL1 = ntohl(inet_addr("224.0.0.0"));
				int iReservedU1 = ntohl(inet_addr("224.0.0.255"));
				//reserved multicast address for global scope
				int iReservedL2 = ntohl(inet_addr("224.0.1.0"));
				int iReservedU2 = ntohl(inet_addr("224.0.1.255"));
				//reserved multicast address for administrative scope
				int iReservedL3 = ntohl(inet_addr("239.0.0.0"));
				int iReservedU3 = ntohl(inet_addr("239.25.255.255"));

				if(  (iAddrHl>=iReservedL1 && iAddrHl<=iReservedU1)
					|| (iAddrHl>=iReservedL2 && iAddrHl<=iReservedU2)
					|| (iAddrHl>=iReservedL3 && iAddrHl<=iReservedU3) )
				{
					vstr::err() << "VistaMcastIPAddress::SetHostName() -- ["
						<< sHostName
						<< "] is a reserved multicast address, can not be used as a multicast group address"
						<< std::endl;
					
					m_bIsValid     = false;
					return;

				}else{

					m_sHostName    = sHostName; // string ip address for multicast
					m_sInAddress.resize(sizeof(int));
					m_sInAddress.assign((char *)&iAddr, sizeof(iAddr));

					m_iAddressType = AF_INET;
					m_bIsValid     = true;				
					return;
				
				}

			}else {
				vstr::errp() << "VistaMcastIPAddress::SetHostName() error: ["
					<< sHostName
					<< "] is not a valid multicast address" << std::endl;

				m_bIsValid     = false;
				return;
			
			}


		}
		else
		{
			// this fails! we have a non-number-and-dot notation now
			// it can not be used in multicast ip address
			vstr::errp() << "VistaMcastIPAddress::SetHostName() error: ["
				<< sHostName
				<< "] is not a numbers-and-dots notation address" << std::endl;

			m_bIsValid     = false;
			return;

		}	
	
	}
	return;
}

void VistaMcastIPAddress::SetAddress(const string &sAddress)
{
	
			int iAddr = *( (const int *)(sAddress.data()) );
			if (  IN_MULTICAST(ntohl(iAddr)) )
			{
				//reserved multicast address for local scope
				int iReservedL1 = inet_addr("224.0.0.0");
				int iReservedU1 = inet_addr("224.0.0.255");
				//reserved multicast address for global scope
				int iReservedL2 = inet_addr("224.0.1.0");
				int iReservedU2 = inet_addr("224.0.1.255");
				//reserved multicast address for administrative scope
				int iReservedL3 = inet_addr("239.0.0.0");
				int iReservedU3 = inet_addr("224.25.255.255");

				if( ( iAddr>=iReservedL1 && iAddr<=iReservedU1 )
					|| ( iAddr>=iReservedL2 && iAddr<=iReservedU2  )
					|| ( iAddr>=iReservedL3 && iAddr<=iReservedU3 ) )
				{
					vstr::errp() << "VistaMcastIPAddress::SetAddress() error: ["
						<< sAddress
						<< "] is a reserved multicast address, can not be used as a multicast group address"
						<< std::endl;
					
					m_bIsValid     = false;
					return;
				}
				else
				{					
					m_sHostName    = inet_ntoa( *((in_addr *)&iAddr) ); // string ip address for multicast
					m_sInAddress.resize(sizeof(in_addr));
					m_sInAddress.assign( reinterpret_cast<char *>( &iAddr ), sizeof(in_addr));

					m_iAddressType = AF_INET;
					m_bIsValid     = true;				
					return;
				
				}

			}
			else
			{
				vstr::errp() << "VistaMcastIPAddress::SetAddress() error: ["
					<< sAddress
					<< "] is not a valid multicast address" << std::endl;

				m_bIsValid     = false;
				return;
			
			}

}


void  VistaMcastIPAddress::GetAddressTypeString(string &sString) const
{
	switch(m_iAddressType)
	{
	case VIPA_UNASSIGNED:
		{
			sString = "Unassigned";
			break;
		}
	case AF_INET:
		{
			sString = "IPV4 (AF_INET), Multicast";
			break;
		}
	default:
		{
			sString = "Unknown, sorry";
			break;
		}
	}
}


VistaMcastIPAddress &VistaMcastIPAddress::operator= (const VistaMcastIPAddress &inAddr)
{
	SetHostName(inAddr.GetHostNameC());
	m_iAddressType = inAddr.m_iAddressType;
	return *this;
}

VistaMcastIPAddress &VistaMcastIPAddress::operator= (const string &sHostName)
{
	SetHostName(sHostName);
	m_iAddressType = AF_INET; // we simply assume this...
	return *this;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


