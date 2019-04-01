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

#if defined(VISTA_THREADING_WIN32)

// project includes
#include "VistaWin32ProcessEventImp.h"

#include <winsock2.h>
#include <Windows.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimer.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

std::string GetErrorText( int nCode = -1 )
{
	std::string sReturn( 1024, '\0' );
	if( nCode == -1 )
		nCode = GetLastError();
	int nSize = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					nCode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR) &sReturn[0],
					1024,
					NULL );
	sReturn.resize( nSize );
	return sReturn;
}

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaWin32ProcessEventImp::VistaWin32ProcessEventImp( const std::string& sEventName )
: IVistaProcessEventImp( sEventName, true )
{
	m_pEventHandle = ::CreateEvent( NULL, false, false, TEXT( sEventName.c_str() ) );
	if( m_pEventHandle == NULL )
	{
		vstr::warnp() << "[VistaWin32ProcessEventImp]: Could not create event ["
			<< sEventName << "] - Error: " << GetErrorText() << std::endl;
	}
} 

VistaWin32ProcessEventImp::VistaWin32ProcessEventImp( const std::string& sEventName,
														const int nMaxWaitForSignaller )
: IVistaProcessEventImp( sEventName )
{
	// created by another process, just open it
	// we'll have 
	VistaType::microtime dWaitTime = (VistaType::microtime)nMaxWaitForSignaller * 1e-3;
	VistaTimer oTimer;
	while( oTimer.GetLifeTime() < dWaitTime )
	{
		m_pEventHandle = ::OpenEvent( SYNCHRONIZE, true, TEXT( sEventName.c_str() ) );
		if( m_pEventHandle != NULL )
			break;
	}
	if( m_pEventHandle == NULL )
	{
		vstr::warnp() << "[VistaWin32ProcessEventImp]: Could not open event ["
			<< sEventName << "] in allowed time frame - Error: " << GetErrorText() << std::endl;
	}
} 



VistaWin32ProcessEventImp::~VistaWin32ProcessEventImp()
{
	::CloseHandle( m_pEventHandle );
} 

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaWin32ProcessEventImp::GetIsValid() const
{
	return ( m_pEventHandle != NULL );
} 

bool VistaWin32ProcessEventImp::SignalEvent()
{
	return( ::SetEvent( m_pEventHandle ) == TRUE );
} 

bool VistaWin32ProcessEventImp::WaitForEvent( int nBlockTime )
{
	if( GetIsValid() == false )
		return false;
	int nRet =  WaitForSingleObject( m_pEventHandle, nBlockTime );
	switch( nRet )
	{
		case WAIT_OBJECT_0:
			return true; // succes
		case WAIT_TIMEOUT:
			return false; // timeout
		case WAIT_FAILED:
			vstr::errp() << "[InterProcEmittingBarrier]: Waiting for event failed - "
						<< GetErrorText() << std::endl;
		case WAIT_ABANDONED: // should only happen for mutexes!
		default:
			VISTA_THROW( "InterProcEmittingBarrier WaitForSingleObject fatal error", -1 );
	}
} 

bool VistaWin32ProcessEventImp::WaitForEvent(bool bBlock)
{
	if( GetIsValid() == false )
		return false;
	int nTime = bBlock ? INFINITE : 0;
	int nRet =  WaitForSingleObject( m_pEventHandle, nTime );
	switch( nRet )
	{
		case WAIT_OBJECT_0:
			return true; // succes
		case WAIT_TIMEOUT:
			return false; // timeout
		case WAIT_FAILED:
			vstr::errp() << "[InterProcEmittingBarrier]: Waiting for event failed - "
						<< GetErrorText() << std::endl;
		case WAIT_ABANDONED: // should only happen for mutexes!
		default:
			VISTA_THROW( "InterProcEmittingBarrier WaitForSingleObject fatal error", -1 );
	}
} 

#endif // VISTA_THREADING_WIN32
// ============================================================================
// ============================================================================

