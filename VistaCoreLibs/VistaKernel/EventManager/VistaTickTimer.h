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


#ifndef _VISTATICKTIMER_H
#define _VISTATICKTIMER_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/EventManager/VistaEventObserver.h>

/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaEventManager;
class VistaEventHandler;
class VistaTickTimerEvent;
class VistaClusterMode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is a simple class to use whenever you intend to have a constant ticking
 * information for fixed time intervalls. It does realize an observer that does
 * account for every PreDrawGfxEvent and see whether a specific intervall has
 * already passed or not. If this is the case it will pass a TickTimerEvent
 * to the application's EventHandler, which will pass it to any interested party.
 * In case this happens, you do now know at least:<br>
 * <ul>
 * <li>At least a time interval t0 has passed
 * <li>No more :)
 * </ul>
 */
class VISTAKERNELAPI VistaTickTimer : public VistaEventObserver
{
public:
	/**
	 * This is a modifier for the timestamp that is received by VistaTimer.
	 * You can wait in second-intervalls or milliseconds, but do not expect a
	 * real granularity of a millisecond ;)
	 * @see SetGranularity()
	 */
	enum GRANULARITY
	{
		SWT_SEC=1, /**< sets granularity to seconds */
		SWT_MILLI=1000 /**< sets granularity to milliseconds. */
	};

public:

	/**
	 * Constructor, registeres a new event-type for this particular TickTimerEvent, in case
	 * you specify a specific handler for TickTimerEvents like this, give it as second argument.
	 * Note that this observer is registered to the given EventManager and registered type-id.
	 * @see VistaEventManager()
	 * @see VistaEventHandler()
	 * @param pEvMa the EventManager to receive a type-id and register to
	 * @param han a special handler for TickTimerEvents thrown from this TickTimer
	 */
	VistaTickTimer(VistaEventManager *pEvMa,
					VistaClusterMode *pClusterAux );

	/**
	 * Unregisters this observer, stops this stop watch
	 */
	virtual ~VistaTickTimer();

	/**
	 * Measures the difference between this and the last PreDrawGfxEvent, the difference is summed
	 * up, and if it is greater that the give TickValue t0, an event is given to the EventManager.
	 * @param pEvent the event that this observer is registered to
	 */
	virtual void Notify(const VistaEvent *pEvent);


	/**
	 * Sets the modifier to have an influence on the time granularity.
	 * In case you set this to SWT_MILLI, a call to SetTickTime(50) will wait for at least 50ms.
	 * In case you set this to SWT_SEC, a call to SetTickTime(50) will wait for at least 50s.
	 * @param eG the modifier to use.
	 * @see GRANULARITY
	 */
	void SetGranularity(GRANULARITY eG);

	GRANULARITY GetGranularity() const;


	/**
	 * Specifies the tick time interval. This value has to be understood according to the granularity
	 * set.
	 * @see SetGranularity()
	 * @parm dTickTime the length of the time interval according to the granularity set.
	 */
	void SetTickTime(double dTickTime);

	/**
	 * Starts this TickTimer. A started stop watch can be started again, without effect.
	 * A started TickTimer will constantly throw Events every passed time interval as specified
	 * using SetTickTime.
	 * @return true always
	 */
	bool StartTickTimer();

	/**
	 * Stops this TickTimer. A stopped TickTimer can be stopped again.
	 * A StoppedTickTimer will receive observation notifications, but will not react on them.
	 * @return true always.
	 */
	bool StopTickTimer();

	/**
	 * Indicates whether this TickTimer is running or not.
	 * @return true iff this TickTimer is counting, living and breathing.
	 */
	bool IsRunning() const;

	/**
	 * Resets this TickTimer. The interval counting will start over, but this watch will not be stopped
	 * if it was started.
	 * @return true always.
	 */
	bool ResetTickTimer();

	bool GetPulsingTimer() const;
	void SetPulsingTimer(bool bPulsing);

	double GetRealIntervalLength() const;
	double GetTickTime() const;

protected:

	/**
	 * A pointer to the EventManager to register with.
	 */
	VistaEventManager *m_pEventManager;

	/**
	 * A corresponding TickTimerEvent to throw for this TickTimer.
	 */
	VistaTickTimerEvent *m_pTickTimerTick;


	/**
	 * Needed for the global timing in a vista frame (frame clock)
	 */
	VistaClusterMode *m_pClusterAux;

private:
	/**
	 * The ticker granularity modifier as set by SetGranularity
	 */
	GRANULARITY m_eTickerGranularity;

	/**
	 * the amount of intervalls to wait for until a TickTimerEvent is thrown.
	 */
	double m_dTickTime, m_dOriginValue;

	/**
	 * Indicates whether this stop watch is up and running.
	 */
	bool m_bIsRunning;

	/**
	 * Intermediate value, saves the last tick time to compare to
	 */
	double m_dLastTick, m_dRealLength;

	bool m_bIsPulsingTimer;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTATICKTIMER_H
