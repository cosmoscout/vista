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


#ifndef _VISTATIMER_H
#define _VISTATIMER_H

#include "VistaBaseConfig.h"
#include "VistaBaseTypes.h"

#include <cstddef>
#include <vector>
#include <cstddef>
class IVistaTimerImp;

class VISTABASEAPI VistaTimer
{
public:
	/**
	 * Creates a timer using the passed Implementation.
	 * If no implementation is given, the timerimp singleton
	 * will be used
	 * @see IVistaTimerImp::SetSingleton
	 */
	VistaTimer( IVistaTimerImp *pImp = NULL );
	virtual ~VistaTimer();

	/**
	 * @return time value with arbitrary reference time,
	 *         high precision (10^-6s to 10^-7s),
	 *         should be strictly monotonic rising most of the time
	 *         (changes every 1microsec (LINUX) or 0.1microsec(WIN32))
	 */
	VistaType::microtime  GetMicroTime() const;

	/**
	 * @return Unspecified strictly monotonic rising Timestamp
	 */
	VistaType::microstamp GetMicroStamp() const;

	/**
	 * @return time since 01.01.1970,
	 *         medium precision (10^-5s / 10 microsecond)
	 *         (not necessaryly strictly) monotonic rising
	 */
	VistaType::systemtime GetSystemTime() const;

	/**
	 * @param  VistaType::microtime value, as received from GetMicrotime(), i.e. unaltered.
	 *         NOTE: The VistaType::microtime has to be retrieved from the same
	 *         timer implementation, so take care that it doesnt change inbetween
	 * @return a monotonic increasing time value
	 *         representing system time (i.e. secs.msecs since 1970)
	 */
	VistaType::systemtime ConvertToSystemTime( const VistaType::microtime mtTime ) const;

	/**
	 * @return Life time of the Timer, i.e. time since 
	 *         creation or last call to ResetLifeTimer()
	 */
	VistaType::microtime GetLifeTime() const;
	/**
	 * resets Timer's life time to zero
	 */
	void ResetLifeTime();

	/**
	 * @return The standard timer singleton, for easy time access.
	 *         If no timer instance is available yet, a new one will
	 *         be created using the current TimerImp singleton.
	 */
	static const VistaTimer& GetStandardTimer();
protected:
	IVistaTimerImp* m_pImp;
	VistaType::microtime       m_nBirthTime;
private:
	// make uncopyable
	VistaTimer( const VistaTimer& oCpoy );
	VistaTimer& operator=( const VistaTimer& oCopy );
};


class VISTABASEAPI VistaAverageTimer : public VistaTimer
{
public:
	VistaAverageTimer( IVistaTimerImp *pImp = NULL );
	virtual ~VistaAverageTimer();

	/**
	 * Starts the recording and sets the reference time used by RecordTime().
	 */
	virtual void StartRecording();
	/**
	 * Records the current time different and adds it to the
	 * averaging time span. If a StartRecording has been called before,
	 * the time span is the difference to this start, otherwise the
	 * time of the last StartRecording() call (or the timer creation/reset) is used
	 */
	virtual void RecordTime();		

	/**
	 * @return time since record started (or since last record, or timer creation/reset)
	 */
	VistaType::microtime GetCurrentRecordingTime() const;
	
	virtual VistaType::microtime GetAccumulatedTime() const;
	virtual int GetRecordCount() const;
	virtual VistaType::microtime GetAverageTime() const;				

	virtual void ResetAveraging();

protected:
	VistaType::microtime	m_nRecordStartTime;
	VistaType::microtime	m_nAccumulatedTime;
	VistaType::microtime	m_nAverage;
	int			m_iCount;
};


/**
 * Average filter, where the average time is not calculated from
 * total_time / total_count, but by a weighted average, so that
 * average =  ( weight * new_record + old_average ) / ( weight + 1 )
 * This way, old values have a more and more diminishing influence,
 * while newer ones are weighted stronger
 */
class VISTABASEAPI VistaWeightedAverageTimer : public VistaAverageTimer
{
public:
	VistaWeightedAverageTimer( const float fWeight = 1,
								IVistaTimerImp *pImp = NULL );
	virtual ~VistaWeightedAverageTimer();

	float GetWeight() const;
	void SetWeight( const float fWeight );

	/**
	 * Records the current time different and adds it to the
	 * averaging time span. If a StartRecording has been called before,
	 * the time span is the difference to this start, otherwise the
	 * time of the last StartRecording() call (or the timer creation/reset) is used
	 */
	virtual void RecordTime();		

	virtual void ResetAveraging();

protected:
	float		m_fWeight;
	float		m_fNorm;
};

/**
 * Average filter, where the average time is not calculated form
 * total_time / total_count, but by a weighted average, so that
 * average =  ( weight * new_record + old_average ) / ( weight + 1 )
 * This way, old values have a more and more diminishing influence,
 * while newer ones are weighted stronger
 */
class VISTABASEAPI VistaWindowAverageTimer : public VistaAverageTimer
{
public:
	VistaWindowAverageTimer( const int iWindowSize = 10,
								IVistaTimerImp *pImp = NULL );
	virtual ~VistaWindowAverageTimer();

	int GetWindowSize() const;

	/**
	 * Records the current time different and adds it to the
	 * averaging time span. If a StartRecording has been called before,
	 * the time span is the difference to this start, otherwise the
	 * time of the last StartRecording() call (or the timer creation/reset) is used
	 */
	virtual void RecordTime();		

	virtual void ResetAveraging();

protected:
	std::vector<VistaType::microtime>	m_vecRecords;
	int						m_iCurrentSlot;
};


#endif /* _VISTATIMER_H */
