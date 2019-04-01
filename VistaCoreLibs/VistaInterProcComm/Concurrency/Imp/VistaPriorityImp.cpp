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
#include "VistaPriorityImp.h"


#if defined(VISTA_THREADING_WIN32)
#include "VistaWin32PriorityImp.h"
#elif defined(VISTA_THREADING_POSIX)
#include "VistaPthreadsPriorityImp.h"
#elif defined(VISTA_THREADING_SPROC)
#include "VistaSPROCPriorityImp.h"
#else
#error "DEFINE PROPER THREAD MODEL TO USE PRIORITIES"
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaPriorityImp::IVistaPriorityImp()
{
	m_iSystemPriorityMax = 1;
	m_iSystemPriorityMin = 0;
	m_iSystemPriorityInterval = 1;
	m_iSystemPriorityIntervalDirection = 1;
}

IVistaPriorityImp::~IVistaPriorityImp()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


IVistaPriorityImp *IVistaPriorityImp::CreatePriorityImp()
{
#if defined(VISTA_THREADING_WIN32)
	return new VistaWin32PriorityImp;
#elif defined(VISTA_THREADING_POSIX)
	return new VistaPthreadsPriorityImp;
#elif defined(VISTA_THREADING_SPROC)
	return new VistaSPROCPriorityImp;
#else
	return 0;
#endif
}

int IVistaPriorityImp::GetSystemPriorityMax()
{
	return m_iSystemPriorityMax;
}

int IVistaPriorityImp::GetSystemPriorityMin()
{
	return m_iSystemPriorityMin;
}

int IVistaPriorityImp::GetSystemPriorityInterval()
{
	return m_iSystemPriorityInterval;
}

int IVistaPriorityImp::GetSystemPriorityIntervalDirection()
{
	return m_iSystemPriorityIntervalDirection;
}


void IVistaPriorityImp::SetSystemPriorityMax( int iMax )
{
	m_iSystemPriorityMax = iMax;
}

void IVistaPriorityImp::SetSystemPriorityMin( int iMin )
{
	m_iSystemPriorityMin = iMin;
}

void IVistaPriorityImp::InitInterval()
{
	m_iSystemPriorityInterval = m_iSystemPriorityMax - m_iSystemPriorityMin;
	if ( m_iSystemPriorityInterval > 0)
		m_iSystemPriorityIntervalDirection = 1;
	else
		m_iSystemPriorityIntervalDirection = -1;
	m_iSystemPriorityInterval *= m_iSystemPriorityIntervalDirection;
}

int IVistaPriorityImp::ScalePriorityToSystemPriority(int iPrio) const
{
	double dResultPrio = (double) iPrio * (double) m_iSystemPriorityInterval / (double) VISTA_MAX_PRIORITY;
	dResultPrio = m_iSystemPriorityMin + (m_iSystemPriorityIntervalDirection * dResultPrio);

	return (int) dResultPrio;

}

int IVistaPriorityImp::ScaleSystemPriorityToPriority(int iSysPrio) const
{
	double iSysPrioTransformed = (double)( iSysPrio - m_iSystemPriorityMin ) / (double)m_iSystemPriorityIntervalDirection;
	double dResultPrio =  iSysPrioTransformed * (double) VISTA_MAX_PRIORITY / (double) m_iSystemPriorityInterval;

	return (int) dResultPrio;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


