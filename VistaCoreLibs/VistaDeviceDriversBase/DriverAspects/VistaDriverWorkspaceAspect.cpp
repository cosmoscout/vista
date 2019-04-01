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


#include "VistaDriverWorkspaceAspect.h" 
#include "VistaDeviceDriverAspectRegistry.h"

#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverWorkspaceAspect::m_nAspectId  = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverWorkspaceAspect::VistaDriverWorkspaceAspect()
	: IVistaDeviceDriver::IVistaDeviceDriverAspect(false)
{
	if(VistaDriverWorkspaceAspect::GetAspectId() == -1) // unregistered
		VistaDriverWorkspaceAspect::SetAspectId( 
		VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("WORKSPACE"));

	SetId(VistaDriverWorkspaceAspect::GetAspectId());
}

VistaDriverWorkspaceAspect::~VistaDriverWorkspaceAspect()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaDriverWorkspaceAspect::GetWorkspace(const std::string &strKey, VistaBoundingBox &bbOut) const
{
	std::map<std::string, VistaBoundingBox>::const_iterator cit = m_mpWorkspaces.find(strKey);
	if(cit == m_mpWorkspaces.end())
		return false;

	bbOut = (*cit).second;
	return true;
}

void VistaDriverWorkspaceAspect::SetWorkspace( const std::string &strKey, const VistaBoundingBox &bb )
{
	m_mpWorkspaces[strKey] = bb;
}


std::list<std::string> VistaDriverWorkspaceAspect::GetWorkspaceKeys() const
{
	std::list<std::string> strKeys;
	for(std::map<std::string, VistaBoundingBox>::const_iterator cit = m_mpWorkspaces.begin();
		cit != m_mpWorkspaces.end(); ++cit)
	{
		strKeys.push_back( (*cit).first );
	}

	strKeys.sort();
	return strKeys;
}

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int  VistaDriverWorkspaceAspect::GetAspectId()
{
	return VistaDriverWorkspaceAspect::m_nAspectId;
}

void VistaDriverWorkspaceAspect::SetAspectId(int nId)
{
	assert(m_nAspectId == -1);
	m_nAspectId = nId;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


