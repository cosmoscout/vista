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


#include "VistaDriverPropertyConfigurator.h" 
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

#include <set>
#include <list>
#include <string>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaDriverPropertyConfigurator::IConfigurator::IConfigurator()
: m_pDriver(NULL)
{}

VistaDriverPropertyConfigurator::IConfigurator::~IConfigurator() 
{}

bool VistaDriverPropertyConfigurator::IConfigurator::Configure( IVistaDeviceDriver* pDriver,
															const VistaPropertyList& oDriverSection,
															const VistaPropertyList& oConfig )
{
	m_pDriver = pDriver;
	SetPropertiesByList( oDriverSection );
	m_pDriver = NULL;
	return true;
}	

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverPropertyConfigurator::VistaDriverPropertyConfigurator()
{
}

VistaDriverPropertyConfigurator::~VistaDriverPropertyConfigurator()
{
	std::set<IConfigurator*> st;

	for(CONFIGMAP::iterator it = m_mpConfigurators.begin();
		it != m_mpConfigurators.end(); ++it)
		st.insert( (*it).second.m_pConf );


	for(std::set<IConfigurator*>::iterator sit = st.begin();
		sit != st.end(); ++sit)
			delete (*sit);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


void VistaDriverPropertyConfigurator::RegisterConfigurator(const std::string &sTriggerKey, 
															IConfigurator *pConf,
															const std::list<std::string> &liDepends,
															int nPrio)
{
	m_mpConfigurators[sTriggerKey] = _sHlp(pConf, nPrio, liDepends);
}


VistaDriverPropertyConfigurator::IConfigurator *VistaDriverPropertyConfigurator::RetrieveConfigurator(const std::string &sTriggerKey) const
{
	CONFIGMAP::const_iterator cit = m_mpConfigurators.find(sTriggerKey);
	if( cit == m_mpConfigurators.end())
		return NULL;

	return (*cit).second.m_pConf;
}

int VistaDriverPropertyConfigurator::GetPrioForTrigger(const std::string &sTriggerKey) const
{
	CONFIGMAP::const_iterator cit = m_mpConfigurators.find(sTriggerKey);
	if( cit == m_mpConfigurators.end())
		return -1;

	return (*cit).second.m_nPrio;
}

std::list<std::string> VistaDriverPropertyConfigurator::GetDependsForTrigger(const std::string &sTrigger) const
{
	CONFIGMAP::const_iterator cit = m_mpConfigurators.find(sTrigger);
	if( cit == m_mpConfigurators.end())
		return std::list<std::string>();

	return (*cit).second.m_liDependsOn;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

