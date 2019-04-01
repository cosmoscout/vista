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


#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

// project includes

#include "VistaProcessEventImp.h"


#if !defined(NULL)
#define NULL 0
#endif

#if defined(VISTA_THREADING_WIN32)
#include "VistaWin32ProcessEventImp.h"
#else
#include "VistaPosixProcessEventImp.h"
#endif


/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
IVistaProcessEventImp::IVistaProcessEventImp( const std::string& sEventName )
: m_sEventName( sEventName )
, m_bIsSignaller( true )
{
}

IVistaProcessEventImp::IVistaProcessEventImp( const std::string& sEventName,
											  const int nMaxWaitForSignaller )
: m_sEventName( sEventName )
, m_bIsSignaller( false )
{
}

IVistaProcessEventImp::~IVistaProcessEventImp()
{
}


std::string IVistaProcessEventImp::GetEventName() const
{
	return m_sEventName;
}

bool IVistaProcessEventImp::GetIsSignaller() const
{
	return m_bIsSignaller;
}

IVistaProcessEventImp *IVistaProcessEventImp::CreateProcessEventSignallerImp( 
														const std::string& sEventName )
{
#if defined(VISTA_THREADING_WIN32)
	return new VistaWin32ProcessEventImp( sEventName );
	
#elif defined(VISTA_THREADING_POSIX)
	return new VistaPosixProcessEventImp( sEventName );
#elif defined(VISTA_THREADING_SPROC)
	return new VistaPosixProcessEventImp( sEventName );
#else
	printf("WARNING: NO PROCESS-EVENT-IMP!\n");
	return NULL;
#endif
}

IVistaProcessEventImp *IVistaProcessEventImp::CreateProcessEventReceiverImp( 
						const std::string& sEventName, const int nMaxWaitForSignaller )
{
#if defined(VISTA_THREADING_WIN32)
	return new VistaWin32ProcessEventImp( sEventName, nMaxWaitForSignaller );
	
#elif defined(VISTA_THREADING_POSIX)
	return new VistaPosixProcessEventImp( sEventName, nMaxWaitForSignaller );
#elif defined(VISTA_THREADING_SPROC)
	return new VistaPosixProcessEventImp( sEventName, nMaxWaitForSignaller );
#else
	printf("WARNING: NO PROCESS-EVENT-IMP!\n");
	return NULL;
#endif
}


// ============================================================================
