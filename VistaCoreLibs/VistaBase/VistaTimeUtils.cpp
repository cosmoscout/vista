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


#include <cerrno>

#include "VistaTimeUtils.h"

#include "VistaTimerImp.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

#if defined(WIN32)
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif

#include <iostream>
#include <cassert>

#include "VistaTimer.h"


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaType::microtime VistaTimeUtils::ConvertToDayTime( VistaType::microtime dTime )
{	
	VistaType::microtime dMilliseconds = dTime - (long)dTime;

	time_t tCurrentTime = (time_t)dTime;
	struct tm* tLocalTime = localtime(&tCurrentTime);

	VistaType::microtime dDayTime = ( (VistaType::microtime)tLocalTime->tm_sec) 
						+ ( ((VistaType::microtime)tLocalTime->tm_min)*60)
						+ ( ((VistaType::microtime)tLocalTime->tm_hour) )*3600
						+ dMilliseconds;

	return dDayTime;
}

void VistaTimeUtils::ConvertToDate( const VistaType::microtime dTime, 
								int& iMillisecond,
								int& iSecond,
								int& iMinute,
								int& iHour,
								int& iDay,
								int& iMonth,
								int& iYear )
{
	iMillisecond = (int)( ( dTime - (long)dTime ) * 1000 );

	time_t tCurrentTime = (time_t)dTime;
	struct tm* tLocalTime = localtime(&tCurrentTime);

	iSecond = tLocalTime->tm_sec;
	iMinute = tLocalTime->tm_min;
	iHour = tLocalTime->tm_hour;
	iDay = tLocalTime->tm_mday;
	iMonth = 1 + tLocalTime->tm_mon;
	iYear = 1900 + tLocalTime->tm_year;
}

std::string VistaTimeUtils::ConvertToFormattedTimeString( const VistaType::systemtime dTime, const std::string& sFormat )
{
	std::string sResult;
	
	time_t tCurrentTime = (time_t)dTime;
	struct tm* tLocalTime = localtime(&tCurrentTime);

	std::size_t nSize = 64;
	for( ;; )
	{
		sResult.resize( nSize );
		std::size_t nRet = strftime( &sResult[0], nSize , sFormat.c_str(), tLocalTime );
		if( nRet > 0 )
		{
			sResult.resize( nRet );
			break;
		}
		nSize *= 2;
		assert( nSize < 1e6 ); // should never really happen
	}
	return sResult;
}

std::string VistaTimeUtils::ConvertToLexicographicDateString( const VistaType::systemtime dTime )
{
	return ConvertToFormattedTimeString( dTime, "%y%m%d_%H%M%S" );
}

void VistaTimeUtils::Sleep( int iMilliseconds )
{
#ifdef WIN32
	::Sleep( iMilliseconds );
#else
	if( iMilliseconds != 0 )
	{
		if( iMilliseconds >= 1000)
		{
			int iMicroseconds = iMilliseconds * 1000;
			struct timeval tv;
			tv.tv_sec  = iMicroseconds / 1000000;
			tv.tv_usec = iMicroseconds % 1000000;
			// simply poll, but in microwait!
			select( 0, NULL, NULL, NULL, &tv );
		}
		else
			TEMP_FAILURE_RETRY( usleep( iMilliseconds*1000 ) ); // 
	}
#endif
}

const VistaTimer& VistaTimeUtils::GetStandardTimer()
{
	return VistaTimer::GetStandardTimer();
}


/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

