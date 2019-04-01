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



#ifndef _VISTADEFAULTTIMERIMP_H
#define _VISTADEFAULTTIMERIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#include <windows.h>
#endif

#if defined(DARWIN)
#include <mach/mach_time.h>
#endif

#include "VistaTimerImp.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Default implementation of IVistaTimerImp
 * Should not be used directly - use VistaTimer instead, which automatically
 * retrieves the TimerImpl singleton
 */
class VISTABASEAPI VistaDefaultTimerImp : public IVistaTimerImp
{
public:
	VistaDefaultTimerImp();	

	virtual VistaType::microtime  GetMicroTime()   const;
	virtual VistaType::microstamp GetMicroStamp()  const;
	virtual VistaType::microtime  GetSystemTime()  const;
	virtual VistaType::systemtime ConvertToSystemTime( const VistaType::microtime mtTime ) const;

protected:
	~VistaDefaultTimerImp();
protected:
#ifdef WIN32
	mutable VistaType::microstamp		m_nInitialStamp;
	mutable VistaType::microstamp		m_nLastStamp;	
	mutable DWORD						m_nLastTickCount;
	VistaType::microtime				m_nFrequencyDenom;
	VistaType::microtime				m_nInitialSystemTime;
#elif defined DARWIN
	mach_timebase_info_data_t			m_sTimebaseInfo;
#else
	double								m_nInitialTime;
#endif
};


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif /* _VISTADEFAULTTIMERIMP_H */
