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
#include "VistaWin32ThreadEventImp.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaWin32ThreadEventImp::VistaWin32ThreadEventImp()
{
	m_EventHandle = ::CreateEvent(NULL, false, false, NULL);
} 


VistaWin32ThreadEventImp::~VistaWin32ThreadEventImp()
{
	::CloseHandle(m_EventHandle);
} 


/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaWin32ThreadEventImp::SignalEvent()
{
	::SetEvent(m_EventHandle);
} 

bool VistaWin32ThreadEventImp::WaitForEvent(int iTimeoutMSecs)
{
	if(WaitForSingleObject(m_EventHandle, iTimeoutMSecs)==WAIT_OBJECT_0)
		return true;
	return false;
} 

bool VistaWin32ThreadEventImp::WaitForEvent(bool bBlock)
{
	if(bBlock)
	{
		if(WaitForSingleObject(m_EventHandle, INFINITE)==WAIT_OBJECT_0)
			return true;
	}
	else
	{
		if(WaitForSingleObject(m_EventHandle, 0)==WAIT_OBJECT_0)
			return true;
	} 
	return false;
} 

HANDLE VistaWin32ThreadEventImp::GetEventSignalHandle() const
{
	return m_EventHandle;
} 

HANDLE VistaWin32ThreadEventImp::GetEventWaitHandle() const
{
	return m_EventHandle;
} 

bool VistaWin32ThreadEventImp::ResetThisEvent( ResetBehavior )
{
	return (::ResetEvent(m_EventHandle) == TRUE ? true : false);
}


#endif // VISTA_THREADING_WIN32
// ============================================================================
// ============================================================================

