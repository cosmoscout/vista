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


#include "VistaDriverLoggingAspect.h" 
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include "VistaDeviceDriverAspectRegistry.h"

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>
#include <cassert>

#if defined(SUNOS)
#include <typeinfo.h>
#else
#include <typeinfo>
#endif


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverLoggingAspect::m_nAspectId  = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverLoggingAspect::VistaDriverLoggingAspect()
	: IVistaDeviceDriver::IVistaDeviceDriverAspect(true), // can be unregistered
	  m_nLogLevel(0),
	  m_bEnabled(true)
{
	if(VistaDriverLoggingAspect::GetAspectId() == -1) // unregistered
		VistaDriverLoggingAspect::SetAspectId( 
		VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("LOGGING"));

	SetId(VistaDriverLoggingAspect::GetAspectId());
}

VistaDriverLoggingAspect::~VistaDriverLoggingAspect()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


void VistaDriverLoggingAspect::Log( VistaType::microtime nTimestamp,
									const IVistaDeviceDriver *pDriver, 
									const std::string &strMsg,
									int nLogLevel)
{
	if(m_bEnabled && (nLogLevel >= m_nLogLevel))
	{
		DoLog( nTimestamp, pDriver, strMsg, nLogLevel );
	}
}


int  VistaDriverLoggingAspect::GetLogLevel() const
{
	return m_nLogLevel;
}

void VistaDriverLoggingAspect::SetLogLevel(int nLevel)
{
	m_nLogLevel = nLevel;
}


bool VistaDriverLoggingAspect::DoLog( VistaType::microtime nTimestamp,
									  const IVistaDeviceDriver *pDriver,
									  const std::string &strMsg, int nLogLevel)
{
	if(m_strMnemonic.empty())
	{
		vstr::outi() << nTimestamp << "\t" << typeid(pDriver).name() << "\t"
			<< pDriver << std::endl;
	}
	else
	{
		vstr::outi() << nTimestamp << "\t" << m_strMnemonic << "\t" << pDriver << std::endl;
	}

	vstr::outi() << strMsg << std::endl;
	return true;
}

bool VistaDriverLoggingAspect::GetEnabled() const
{
	return m_bEnabled;
}

void VistaDriverLoggingAspect::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

std::string VistaDriverLoggingAspect::GetMnemonic() const
{
	return m_strMnemonic;
}

void VistaDriverLoggingAspect::SetMnemonic(const std::string &strMnemonic)
{
	m_strMnemonic = strMnemonic;
}

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int  VistaDriverLoggingAspect::GetAspectId()
{
	return VistaDriverLoggingAspect::m_nAspectId;
}

void VistaDriverLoggingAspect::SetAspectId(int nId)
{
	assert(m_nAspectId == -1);
	m_nAspectId = nId;

}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


