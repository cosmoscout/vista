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



/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

#include "VistaRegistration.h"

#include <cstdio>
#include <string>
using namespace std;


/*============================================================================*/
/* STATICS                                                                    */
/*============================================================================*/

IDLVistaRegistration *IDLVistaRegistration::m_pSingleton = NULL;

bool IDLVistaRegistration::RegisterRegistration(IDLVistaRegistration *pRegistry)
{
	if(!m_pSingleton)
	{
		m_pSingleton = pRegistry;
		return true;
	}
	
	return false;
}

IDLVistaRegistration *IDLVistaRegistration::GetRegistrationSingleton()
{
	return m_pSingleton;
}

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/


IDLVistaRegistration::IDLVistaRegistration(IDLVistaRegistration &)
{
	RegisterRegistration(this);
	RegisterPacketType("ANY Packet", 0);
}


IDLVistaRegistration::IDLVistaRegistration()
{
	RegisterRegistration(this);
	RegisterPacketType("ANY Packet", 0);
}


IDLVistaRegistration::~IDLVistaRegistration()
{
	m_pSingleton=(IDLVistaRegistration*)-1;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

int IDLVistaRegistration::RegisterPacketType(const string &SName)
{
	map<string, int>::iterator it = m_mpNames2Types.find(SName);
	if(it!=m_mpNames2Types.end())
	{
		// already registered, reject
		return -1;
	}
	
	// ok, this name is not yet registered
	int iTypeId = CreateTypeIdForName(SName);
	if(iTypeId!=-1)
	{
		RegisterPacketType(SName, iTypeId);
	}
	
	return iTypeId; // give back type-id or fail (-1)
}


bool IDLVistaRegistration::RegisterPacketType(const string &SName, int iTypeId)
{
	// should use DBC here!
	if((GetTypeIdForName(SName) == -1) && (GetNameForTypeId(iTypeId)==""))
	{
		m_mpNames2Types.insert(map<string, int>::value_type(SName, iTypeId));
		m_mpTypes2Names.insert(map<int, string>::value_type(iTypeId, SName));
		printf("Registered type [%s] with Type-Id [%d]\n", SName.c_str(), iTypeId);
		return true;
	}
	
	return false;
}

	
bool IDLVistaRegistration::UnregisterPacketType(int iTypeId)
{
	map<int, string>::iterator it = m_mpTypes2Names.find(iTypeId);
	if(it!=m_mpTypes2Names.end())
	{
		// ok, found
		string SName = (*it).second;
		m_mpTypes2Names.erase(it);
		
		map<string, int>::iterator it2 = m_mpNames2Types.find(SName);
		
		// this must be a hit!
		m_mpNames2Types.erase(it2);
		return true;
	}
	
	return false;
}

bool IDLVistaRegistration::UnregisterPacketType(const string &SPacketName)
{
	map<string, int>::iterator it = m_mpNames2Types.find(SPacketName);
	if(it!=m_mpNames2Types.end())
	{
		int iTypeId = (*it).second;
		m_mpNames2Types.erase(it);
		
		map<int, string>::iterator it2 = m_mpTypes2Names.find(iTypeId);
		
		// this must be a hit
		m_mpTypes2Names.erase(it2);
		return true;
	}
	return false;
}
	

string IDLVistaRegistration::GetNameForTypeId(int iTypeId) const
{
	map<int, string>::const_iterator it = m_mpTypes2Names.find(iTypeId);
	if(it !=  m_mpTypes2Names.end())
		return (*it).second;
	return "";
}
	
int IDLVistaRegistration::GetTypeIdForName(const string &SName) const
{
	map<string, int>::const_iterator it = m_mpNames2Types.find(SName);
	if(it!= m_mpNames2Types.end())
		return (*it).second;
	
	return -1;
}
	

