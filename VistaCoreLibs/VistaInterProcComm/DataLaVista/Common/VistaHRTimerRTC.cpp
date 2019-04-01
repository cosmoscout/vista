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



/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
#include "VistaHRTimerRTC.h"

#if defined(_USE_HRRTC)


#include <cstdio>
#include <sys/types.h>

#ifdef UXPV
	#include <sys/timesu.h>
#else
	#include <unistd.h>
	#include <sys/times.h>
	#include <sys/time.h>
	#include <time.h>
#endif

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaHRTimerRTC::DLVistaHRTimerRTC()
{
}


DLVistaHRTimerRTC::DLVistaHRTimerRTC(DLVistaHRTimerRTC &)
{
}


DLVistaHRTimerRTC::~DLVistaHRTimerRTC()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
double DLVistaHRTimerRTC::GetTickToSecond( DLV_INT64 nTs ) const
{
	// nTs is given in nanoseconds
	return nTs / 1000000L;
}


DLV_INT32 DLVistaHRTimerRTC::GetTimeStamp() const
{
	return (DLV_INT32)GetSystemTime();
}


double DLVistaHRTimerRTC::GetSystemTime() const
{
	double  sysSeconds;

	struct timeval tv;
	gettimeofday(&tv, (struct timezone*)0);

	// seconds since 1.1.1970
	sysSeconds = ((double)tv.tv_sec + (double)tv.tv_usec / 1000000.0 );

	return sysSeconds;
}


DLV_INT64 DLVistaHRTimerRTC::GetTickCount() const
{
	return (DLV_INT64)gethrtime();
}


#endif // _USE_HRRTC

