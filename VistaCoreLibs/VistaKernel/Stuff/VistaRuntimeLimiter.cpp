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


#include "VistaRuntimeLimiter.h"

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/VistaFrameLoop.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaRuntimeLimiter::VistaRuntimeLimiter( VistaSystem* pSystem )
: m_pSystem( pSystem )
, m_nFrameLimit( std::numeric_limits<unsigned int>::max() )
, m_nTimeLimit( std::numeric_limits<VistaType::microtime>::max() )
{
	m_pSystem->GetEventManager()->AddEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREAPPLICATIONLOOP );
}


VistaRuntimeLimiter::~VistaRuntimeLimiter()
{
}


unsigned int VistaRuntimeLimiter::GetFrameLimit() const
{
	return m_nFrameLimit;
}

void VistaRuntimeLimiter::SetFrameLimit( const unsigned int& oValue )
{
	m_nFrameLimit = oValue;
}

VistaType::microtime VistaRuntimeLimiter::GetTimeLimit() const
{
	return m_nTimeLimit;
}

void VistaRuntimeLimiter::SetTimeLimit( const VistaType::microtime& oValue )
{
	m_nTimeLimit = oValue;
}

void VistaRuntimeLimiter::HandleEvent( VistaEvent *pEvent )
{
	if( m_oTimer.GetLifeTime() > m_nTimeLimit )		
	{
		vstr::outi() << "[VistaRuntimeLimiter]: Shutting down Vista after "
			<< vstr::formattime( m_oTimer.GetLifeTime(), 3 ) << "s runtime" << std::endl;
		m_pSystem->Quit();
	}
	else if( (unsigned int) m_pSystem->GetFrameLoop()->GetFrameCount() > m_nFrameLimit )
	{
		vstr::outi() << "[VistaRuntimeLimiter]: Shutting down Vista after "
			<<m_pSystem->GetFrameLoop()->GetFrameCount() << " frames" << std::endl;
		m_pSystem->Quit();
	}
}




