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


#ifndef _VISTATIMERIMP_H
#define _VISTATIMERIMP_H

#include "VistaBaseConfig.h"
#include "VistaBaseTypes.h"

#include <cstddef>
#include <vector>

/**
 * Interface for Timing classes
 * Only use if you want to provide a special timing method
 * If a special TimerImp is created, it can be set as singleton so
 * that all new VistaTimer's use it by default
 */
class VISTABASEAPI IVistaTimerImp
{
protected:
	IVistaTimerImp();
	virtual ~IVistaTimerImp();
public:

	/**
	 * @return a strictly monotonic increasing time
	 *         with arbitrary reference value 
	 */
	virtual VistaType::microtime  GetMicroTime()   const = 0;

	/**
	 * @return a strictly monotonic increasing timestamp
	 */
	virtual VistaType::microstamp GetMicroStamp()  const = 0;

	/**
	 * @return a monotonic increasing time value
	 *         representing system time (i.e. secs.msecs since 1970).
	 *         Due to exceeded double precision, it is not
	 *         necessarily strictly monotonic, and less precise
	 */
	virtual VistaType::systemtime GetSystemTime() const = 0;

	/**
	 * @param  VistaType::microtime value, as received from GetMicrotime(), i.e. unaltered.
	 * @return a monotonic increasing time value
	 *         representing system time (i.e. secs.msecs since 1970)
	 */
	virtual VistaType::systemtime ConvertToSystemTime( const VistaType::microtime mtTime ) const = 0;

	/**
	 * @return the currently set TimerImp singleton
	 *         this will not create a new instance if no singleton has
	 *         been set before and bCreateDefaultIfNull is set to false,
	 *         so NULL may be returned
	 */
	static IVistaTimerImp *GetSingleton( bool bCreateDefaultIfNull = true );
	/**
	 * @param the timer implementation to use as default for every timer
	 *        that is created from now on. Note that it does not, however,
	 *        update the instance of the singleton, so if you want this
	 *        to be up-to-date, do it yourself
	 */
	static void SetSingleton( IVistaTimerImp * );

	/**
	 * Functions for reference counting, so that an instance is freed when no
	 * timer uses it anymore, and if its not used otherwise (e.g. as default
	 * implementation singleton)
	 * When created, the reference count starts at 0
	 * Note: we cant use IVistaReferenceCountable here, since we're base
	 */
	void IncReferenceCount();
	void DecReferenceCount();
private:
	int m_iReferenceCount;
};

#endif /* _VISTATIMERIMP_H */
