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


#include "VistaDeviceDriverAspectRegistry.h" 


namespace 
{
	VistaDeviceDriverAspectRegistry *SpSingleton = NULL;
}
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaDeviceDriverAspectRegistry *VistaDeviceDriverAspectRegistry::GetSingleton()
{
	if(SpSingleton == NULL)
		SpSingleton = new VistaDeviceDriverAspectRegistry;
	return SpSingleton;
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDeviceDriverAspectRegistry::VistaDeviceDriverAspectRegistry()
: m_nNextAspectId(0)
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

unsigned int VistaDeviceDriverAspectRegistry::RegisterAspect( const std::string &strAspectToken )
{
	m_mpTokenMap[m_nNextAspectId] = strAspectToken;
	return m_nNextAspectId++;
}

std::string  VistaDeviceDriverAspectRegistry::GetTokenForId( unsigned int nId ) const
{
	std::map<unsigned int, std::string>::const_iterator it = m_mpTokenMap.find(nId);
	if(it == m_mpTokenMap.end())
		return "<<NOT REGISTERED>>";
	return (*it).second;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
