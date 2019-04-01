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


#include "VistaDriverMap.h"
#include "VistaDeviceDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverMap::VistaDriverMap()
: m_pReadstateSource(NULL)
{
}

VistaDriverMap::~VistaDriverMap()
{
 	for(CRMAP::iterator it1 = m_mapCreationMethods.begin();
 		it1 != m_mapCreationMethods.end(); ++it1)
 	{
 		(*it1).second->OnUnload();
 		delete (*it1).second;
 	}
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaDriverMap::iterator VistaDriverMap::begin()
{
	return m_mpDevices.begin();
}

VistaDriverMap::iterator VistaDriverMap::end()
{
	return m_mpDevices.end();
}


VistaDriverMap::const_iterator VistaDriverMap::begin() const
{
	return m_mpDevices.begin();
}

VistaDriverMap::const_iterator VistaDriverMap::end() const
{
	return m_mpDevices.end();
}

size_t   VistaDriverMap::size() const
{
	return m_mpDevices.size();
}

VistaDriverMap::crm_iterator VistaDriverMap::crm_begin()
{
	return m_mapCreationMethods.begin();
}

VistaDriverMap::crm_iterator VistaDriverMap::crm_end()
{
	return m_mapCreationMethods.end();
}

VistaDriverMap::crm_const_iterator VistaDriverMap::crm_begin() const
{
	return m_mapCreationMethods.begin();
}

VistaDriverMap::crm_const_iterator VistaDriverMap::crm_end() const
{
	return m_mapCreationMethods.end();
}

size_t VistaDriverMap::crm_size() const
{
	return m_mapCreationMethods.size();
}


bool VistaDriverMap::AddDeviceDriver(const std::string &sName,
												  IVistaDeviceDriver *pDriver)
{
	const_iterator cit = m_mpDevices.find(sName);
	if(cit == m_mpDevices.end())
	{
		m_mpDevices[sName] = pDriver;
		Notify(MSG_ADDDRIVER);
		return true;
	}
	else
	{
		if((*cit).second != pDriver)
		{
			m_mpDevices[sName] = pDriver; // overwrite
			Notify(MSG_ADDDRIVER);
			return true;
		}
	}
	return false;
}


bool VistaDriverMap::GetIsDeviceDriver(const std::string &sName) const
{
	return (m_mpDevices.find(sName) != m_mpDevices.end());
}

IVistaDeviceDriver *VistaDriverMap::GetDeviceDriver(const std::string &sName) const
{
	std::map<std::string, IVistaDeviceDriver*>::const_iterator cit = m_mpDevices.find(sName);
	if(cit == m_mpDevices.end())
		return NULL;

	return (*cit).second;
}


bool VistaDriverMap::DelDeviceDriver(const std::string &sName)
{
	std::map<std::string, IVistaDeviceDriver*>::iterator cit = m_mpDevices.find(sName);
	if(cit == m_mpDevices.end())
		return false;

	delete (*cit).second;

	m_mpDevices.erase(cit);

	Notify(MSG_DELDRIVER);

	return true;

}

bool VistaDriverMap::RemDeviceDriver(const std::string &sName)
{
	std::map<std::string, IVistaDeviceDriver*>::iterator cit = m_mpDevices.find(sName);
	if(cit == m_mpDevices.end())
		return false;

	m_mpDevices.erase(cit);
	Notify(MSG_REMDRIVER);

	return true;
}

bool VistaDriverMap::RemDeviceDriver( IVistaDeviceDriver *pDrv )
{
	for(std::map<std::string, IVistaDeviceDriver*>::iterator cit = m_mpDevices.begin();
			cit != m_mpDevices.end(); ++cit )
	{
		if( (*cit).second == pDrv )
		{
			// found
			m_mpDevices.erase(cit);
			Notify(MSG_REMDRIVER);
			return true;
		}
	}
	return false;
}

IVistaDeviceDriver *VistaDriverMap::RemAndGetDeviceDriver(const std::string &sName)
{
	std::map<std::string, IVistaDeviceDriver*>::iterator cit = m_mpDevices.find(sName);
	if(cit == m_mpDevices.end())
		return NULL;

	IVistaDeviceDriver* pDriver = (*cit).second;
	m_mpDevices.erase(cit);
	Notify(MSG_REMDRIVER);
	return pDriver;

}

bool VistaDriverMap::RegisterDriverCreationMethod(const std::string &sTypeName,
								  IVistaDriverCreationMethod *pMethod, bool bForce)
{
	if((bForce == true) || (GetDriverCreationMethod(sTypeName) == NULL))
	{
		m_mapCreationMethods[sTypeName] = pMethod;
		return true; // note that we do not free the memory in case of bForce == true!
	}
	return false;
}

bool VistaDriverMap::UnregisterDriverCreationMethod(const std::string &sTypeName,
																 bool bDeleteMethod)
{
	CRMAP::iterator it = m_mapCreationMethods.find(sTypeName);
	if(it == m_mapCreationMethods.end())
		return false;

	if(bDeleteMethod)
		delete (*it).second;

	m_mapCreationMethods.erase(it);

	return true;
}

IVistaDriverCreationMethod *VistaDriverMap::GetDriverCreationMethod(const std::string &sTypeName) const
{
	CRMAP::const_iterator it = m_mapCreationMethods.find(sTypeName);
	if(it == m_mapCreationMethods.end())
		return NULL;
	return (*it).second;
}

bool VistaDriverMap::DeleteDriverCreationMethod( IVistaDriverCreationMethod *pM )
{
	delete pM; // we are the cm's friend, so we may call the destructor...
	return true;
}

void VistaDriverMap::Purge(bool bDelete)
{
	if(bDelete)
	{
		for( DEVMAP::iterator it = m_mpDevices.begin(); it != m_mpDevices.end(); ++it )
			delete (*it).second;

		for( CRMAP::iterator it0 = m_mapCreationMethods.begin(); it0 != m_mapCreationMethods.end(); ++it0 )
			delete (*it0).second;

	}

	m_mpDevices.clear();
	m_mapCreationMethods.clear();
}


IVddReadstateSource *VistaDriverMap::GetReadstateSource() const
{
	return m_pReadstateSource;
}

void VistaDriverMap::SetReadstateSource( IVddReadstateSource *creator )
{
	m_pReadstateSource = creator;
}
