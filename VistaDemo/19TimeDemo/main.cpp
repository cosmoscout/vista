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

/**
 * Description:
 * This is the TimeDemo, which shows the different options to measure and handle
 * time in Vista. Start reading from the top
 */

#include "VistaBase/VistaUtilityMacros.h"
#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>

#include "VistaTools/VistaRandomNumberGenerator.h"

#include "VistaKernel/EventManager/VistaEvent.h"
#include "VistaKernel/EventManager/VistaEventManager.h"
#include "VistaKernel/EventManager/VistaSystemEvent.h"
#include "VistaKernel/VistaFrameLoop.h"
#include "VistaKernel/VistaSystem.h"
#include <VistaKernel/EventManager/VistaEventObserver.h>

#include "VistaKernel/EventManager/VistaEventHandler.h"
#include "VistaKernel/EventManager/VistaTickTimer.h"
#include "VistaKernel/EventManager/VistaTickTimerEvent.h"
#include "VistaKernel/EventManager/VistaTimeoutRouter.h"
#include <iostream>

// time measures and utils
void ShowTimeUtils() {
  // in Vista, time is represented as seconds, measured as double
  // there are two typedefs: VistaType::systemtime and VistaType::microtime
  // these can be used interchangeably, but systemtime should only be used for
  // absolute time since 1970

  // VistaType::microtime represents arbitrary time intervals
  VistaType::microtime nRelativeTime = 3.5; // 3 seconds and 500 milliseconds
  std::cout << "nRelativeTime = " << vstr::formattime(nRelativeTime) << " seconds" << std::endl;

  // VistaType::systemtime represents seconds since January 1, 1970 (unix time)
  VistaType::systemtime nDate1 = 12 * 60 * 60;       // january 1, 1970 at noon
  VistaType::systemtime nDate2 = 1363227255.9265349; // may 14th, 2013 (pi day), at 03:14:15.9265349

  // there are some utils in VistaTimeUtils
  // VistaTimeUtils::ConvertToDate converts a systemtime to its seperate day components
  int nYear, nMonth, nDay, nHour, nMinute, nSeconds, nMilliseconds;
  VistaTimeUtils::ConvertToDate(
      nDate1, nMilliseconds, nSeconds, nMinute, nHour, nDay, nMonth, nYear);

  // we output the time, using its different components. We also use the vstr::formattime stream
  // modifier, which outputs the time with a higher precision (default: up to microseconds)
  vstr::outi() << "Date1 = " << vstr::formattime(nDate1) << " represents date " << nYear << "-"
               << nMonth << "-" << nDay << " at " << nHour << ":" << nMinute << ":" << nSeconds
               << "." << nMilliseconds << std::endl;
  VistaTimeUtils::ConvertToDate(
      nDate2, nMilliseconds, nSeconds, nMinute, nHour, nDay, nMonth, nYear);
  vstr::outi() << "Date2 = " << vstr::formattime(nDate2) << " represents date " << nYear << "-"
               << nMonth << "-" << nDay << " at " << nHour << ":" << nMinute << ":" << nSeconds
               << "." << nMilliseconds << std::endl;

  // this doesn't look too nice, because no leading zero are added as padding. We can use formatted
  // strings using the strftime syntax instrad
  vstr::outi() << "nDate2 = " << vstr::formattime(nDate2) << " represents date "
               << VistaTimeUtils::ConvertToFormattedTimeString(nDate2, "%A, %d of %B %Y, %H:%M:%S")
               << std::endl;

  // An alternative is VistaTimeUtils::ConvertToLexicographicDateString,
  // which converts a systemtime to a lexicographic string of format YYMMDD_HHMMSS
  // which is well suited for sorting, e.g. for usi in log file names
  vstr::outi() << "ConvertToLexicographicDateString( " << vstr::formattime(nDate2)
               << " ) = " << VistaTimeUtils::ConvertToLexicographicDateString(nDate2) << std::endl;

  // We can also convert a systemtime to daytime, representing seconds since midnight
  vstr::outi() << "ConvertToDayTime( " << vstr::formattime(nDate2)
               << " ) = " << vstr::formattime(VistaTimeUtils::ConvertToDayTime(nDate2))
               << std::endl;

  // VistaTimeUtils::Sleep allows to suspend the current thread for the requested amount of
  // milliseconds
  std::cout << "Sleeping for 3 seconds ..." << std::flush;
  VistaTimeUtils::Sleep(3000);
  std::cout << " Woke up again!" << std::endl;
};

// The TimePrintObserver observes all system events and prints their time, and also
// prints the frame clock at the beginning of the frame
class TimePrintObserver : public VistaEventObserver {
 public:
  TimePrintObserver(VistaSystem* pVistaSystem)
      : m_pVistaSystem(pVistaSystem) {
    // we observe all system events
    m_pVistaSystem->GetEventManager()->RegisterObserver(this, VistaSystemEvent::GetTypeId());
  }
  ~TimePrintObserver() {
    m_pVistaSystem->GetEventManager()->UnregisterObserver(this, VistaSystemEvent::GetTypeId());
  }

  virtual void Notify(const VistaEvent* pEvent) {
    // to not spam the console with too much output, we only print the event timing info every
    // 100'th frame
    int nFrameCount = m_pVistaSystem->GetFrameLoop()->GetFrameCount();
    if (nFrameCount % 100 != 0)
      return;

    const VistaSystemEvent* pSysEvent = Vista::assert_cast<const VistaSystemEvent*>(pEvent);

    // One of the easiest ways to receive a time measurement is the FrameClock
    // it can be received directly from the VistaSystem as well as the VistaClusterMode,
    // and is a systemtime. It will stay the same over the whole frame, i.e. until the
    // next POSTGRAPHICS system event
    if (pSysEvent->GetId() == VistaSystemEvent::VSE_POSTGRAPHICS) {
      std::cout << "\n---------------------------------------------------\n"
                   "Frame "
                << std::setw(8) << nFrameCount << " with FrameClock " << std::setw(20)
                << vstr::formattime(m_pVistaSystem->GetFrameClock()) << std::endl;
    }

    // Every event has a time, retrieved py pEvent->GetTime()
    // If the event is cluster-synced (which is the case for VistaSystemEvents,
    // VistaInteractionEvents, and VistaExternalMessageEvents
    // This time should be used if a higher resolution than the once-per-frame FrameClock
    // is required. However, the granularity may still be rather coarse, especially if
    // large calculations or rendering is performed between two event emits.
    // The event time is a time measure relative to an arbitrary reference
    std::cout << "SystemEvent [" << std::setw(30)
              << VistaSystemEvent::GetIdString(pSysEvent->GetId()) << "] at " << std::setw(20)
              << vstr::formattime(pSysEvent->GetTime()) << std::endl;

    // The FrameClock and the event times of th e SystemEvents are usually the best
    // way to do timing, because they are guaranteed to be the same on all nodes
    // when running Vista in cluster mode. Keep in mind that in order to keep all
    // application cluster nodes synchronized, all application logic that depends on
    // times have to use cluster-synced times, otherwise the nodes may get out of sync.
    // For more options to get a synchronized time value on all cluster nodes, independent
    // from the system events, see the ClusterDemo
  }

 private:
  VistaSystem* m_pVistaSystem;
};

// VistaTimer
void ShowVistaTimer() {
  // The VistaTimer is a class to measures time with a high precision.
  // It has at least microsecond accuracy, so it is well-suited for timing code

  // Timers can be constructed locally
  VistaTimer oTimer;
  // Alternatively, the standard timer can be used - doesn't require creating a new instance, but
  // LifeTime should not be used on it
  const VistaTimer& oStandardTimer = VistaTimer::GetStandardTimer();

  // from the Timer, we can receive three different times of measures
  // systemtime: time since January 1, 1970
  // note that due to the high number of seconds since that time, the systemtime
  // needs around 10 of its ~16 significant bits are used, so accuracy degrades
  // in nanosecond and low-microsecond regions
  VistaType::systemtime nAbsoluteTime = oStandardTimer.GetSystemTime();
  std::cout << "The current time is: "
            << VistaTimeUtils::ConvertToFormattedTimeString(
                   nAbsoluteTime, "%A, %d of %B %Y, %H:%M:%S")
            << std::endl;

  // microtime is relative to an arbitrary timestamp (usually the start of the application)
  // so it has enough significant digits to utilize the full resolution of the timer
  VistaType::microtime nRelativeTime = oStandardTimer.GetMicroTime();
  std::cout << "nRelativeTime = " << vstr::formattime(nRelativeTime) << std::endl;

  // microstamp is an arbitrary, monotonously rising counter
  VistaType::microstamp nStamp = oStandardTimer.GetMicroStamp();
  std::cout << "nStamp = " << nStamp << std::endl;

  // The actual resolution of the timer depends on the system - let's estimate it
  // these are very rough estimates, but should give a hint on the magnitude
  VistaType::systemtime nSystemTimeStart = oTimer.GetSystemTime();
  VistaType::systemtime nSystemTimeEnd;
  do {
    nSystemTimeEnd = oTimer.GetSystemTime();
  } while (nSystemTimeStart == nSystemTimeEnd);
  VistaType::microtime nSystemTimeDelta = nSystemTimeEnd - nSystemTimeStart;
  std::cout << "VistaTimer::GetSystemTime()  accuracy is about "
            << vstr::formattime(1e9 * nSystemTimeDelta, 3) << " nanoseconds" << std::endl;

  VistaType::microtime nMicroTimeStart = oTimer.GetMicroTime();
  VistaType::microtime nMicroTimeEnd;
  do {
    nMicroTimeEnd = oTimer.GetMicroTime();
  } while (nMicroTimeStart == nMicroTimeEnd);
  VistaType::microtime nMicroTimeDelta = nMicroTimeEnd - nMicroTimeStart;
  std::cout << "VistaTimer::GetMicroTime()   accuracy is about "
            << vstr::formattime(1e9 * nMicroTimeDelta, 3) << " nanoseconds" << std::endl;

  // The timer can also be used for relative timings
  oTimer.ResetLifeTime();
  double nRes = 0;
  for (int i = 0; i < 100000; ++i) {
    nRes += cos(42.0 / (double)(i + 23)); // calculate something
  }
  VistaType::microtime nDuration = oTimer.GetLifeTime();
  std::cout << "Calculating the value " << nRes << " took " << vstr::formattime(1e3 * nDuration, 3)
            << " milliseconds" << std::endl;

  // for periodic operations, one can use the VistaAverageTimers, of which there are differnet
  // variants the AverageTimer takes the average over all samples
  VistaAverageTimer oAverage;
  // the WindowAverageTimer takes the average over the latest n samples
  VistaWindowAverageTimer oWindowAverage(12);
  // the VistaWeightedAverage updates the average to dampen out older values and
  // give newer ones a higher influence. The influence of new values is configurable
  VistaWeightedAverageTimer oFastAverage(1);
  VistaWeightedAverageTimer oSlowAverage(0.05f);

  // A new averaging measure is recorded by calling RecordTime
  nRes = 0;
  for (int i = 0; i < 42; ++i) {
    // computations here would not be included in the average
    oAverage.StartRecording();
    oWindowAverage.StartRecording();
    oFastAverage.StartRecording();
    oSlowAverage.StartRecording();

    int nIterations = VistaRandomNumberGenerator::GetStandardRNG()->GenerateInt32(1000, 10000);
    for (int j = 0; j < nIterations; ++j) {
      nRes += cos(42.0 / (double)(j + 23)); // calculate something
    }

    oAverage.RecordTime();
    oWindowAverage.RecordTime();
    oFastAverage.RecordTime();
    oSlowAverage.RecordTime();
    // computations here would not be included in the average
  }
  std::cout << "When calculating the value " << nRes << ", iterations required on average:"
            << "\nNormal Average         " << std::setw(20)
            << vstr::formattime(1e3 * oAverage.GetAverageTime(), 3) << " milliseconds"
            << "\n12-Window Average      " << std::setw(20)
            << vstr::formattime(1e3 * oWindowAverage.GetAverageTime(), 3) << " milliseconds"
            << "\nFast Weighted Average  " << std::setw(20)
            << vstr::formattime(1e3 * oFastAverage.GetAverageTime(), 3) << " milliseconds"
            << "\nSlow Weigted Average   " << std::setw(20)
            << vstr::formattime(1e3 * oSlowAverage.GetAverageTime(), 3) << " milliseconds"
            << std::endl;

  // With all these, again keep in mind that timing in cluster-mode can be tricky.
  // Using the VistaTimer for debugging or performance measurements is fine. However, its time
  // values should not be used to influence application logic, otherwise cluster asynchonization is
  // looming

  // these timers all record the same times (apart dfrom the life time), so
  // you will receive the same time values from the standard timer or a local instance.
  // requesting systemtime, microtime or microstamp from timers is thread-safe, however, reading or
  // resetting the life time is not
}

// You can use different specialized eventhandlers/observers, namely
// VistaTickTimer, VistaTimeoutHandler, and VistaTimeoutRouter,
// to receive events at approximate intervals

void SetupTimeEventHandlers(VistaSystem* pSystem) {
  VistaEventManager* pEventManager = pSystem->GetEventManager();
  // TickTimers check the time once a frame, and emit a TickTimerEvent whenever the desired
  // tick duration is exceeded. We first create the tick timer (note: we'll not store it, so its a
  // memleak - don't do that in real apps)
  VistaTickTimer* pTimer = new VistaTickTimer(pEventManager, pSystem->GetClusterMode());
  pTimer->SetGranularity(VistaTickTimer::SWT_SEC);
  pTimer->SetTickTime(1.2); // tick every 4.2 seconds
  pTimer->StartTickTimer();

  VistaTickTimer* pTimer2 = new VistaTickTimer(pEventManager, pSystem->GetClusterMode());
  pTimer2->SetGranularity(VistaTickTimer::SWT_MILLI);
  pTimer2->SetTickTime(314); // tick every 314 milliseconds
  pTimer2->StartTickTimer();
  // The TickTimer checks the interval once every frame, using the frameclock, during the
  // POSTAPPLICATIONLOOP event. Thus, it can only work with the granularity of one frame To receive
  // the tick time events, we create and register an event observer Pardon the function-local class
  // - its just to keep the flow in this file ;)
  class TickTimerHandler : public VistaEventObserver {
   public:
    virtual void Notify(const VistaEvent* pEvent) {
      const VistaTickTimerEvent* pTickEvent =
          Vista::assert_cast<const VistaTickTimerEvent*>(pEvent);
      std::string sGranularityPostfix = "ms ";
      if (pTickEvent->GetSourceTickTimer()->GetGranularity() == VistaTickTimer::SWT_SEC)
        sGranularityPostfix = "s  ";
      std::cout << "Received TickTimerEvent at time " << std::setw(20)
                << vstr::formattime(pEvent->GetTime()) << ", requested tick time: " << std::setw(10)
                << pTickEvent->GetSourceTickTimer()->GetTickTime() << sGranularityPostfix
                << ", actual time: " << std::setw(10)
                << pTickEvent->GetSourceTickTimer()->GetRealIntervalLength() << sGranularityPostfix
                << std::endl;
    }
  };
  // we create and register the event observer to handle all TickTimerEvents
  // pEventManager->RegisterObserver( new TickTimerHandler(), VistaTickTimerEvent::GetTypeId() );

  // Another alternative is the TimeoutHandler, which manages multiple ticktimers, and
  // forwards events to an overload. The TimeoutRouter is a specialization, that
  // allows registering multiple callbacks with different timeouts
  VistaTimeoutRouter* pTimeoutRouter =
      new VistaTimeoutRouter(pEventManager, pSystem->GetClusterMode());
  // we write a class for the callbacks
  class TimeoutRouterCallback : public VistaTimeoutRouter::CTimedCallback {
   public:
    TimeoutRouterCallback(const std::string& sName)
        : m_sName(sName) {
    }
    virtual bool DoCallback(double dTimeStamp, double dDesiredLength, double dIntervalLength) {
      std::cout << "TimeutRouterHandler [" << std::setw(22) << m_sName << "] called at time "
                << std::setw(20) << vstr::formattime(dTimeStamp)
                << ", requested tick time: " << std::setw(10) << dDesiredLength << "ms"
                << ", actual time: " << std::setw(10) << dIntervalLength << "ms" << std::endl;
      return true;
    }
    virtual void CallbackDoneHandshake() {
      std::cout << "TimeutRouterHandler [" << std::setw(22) << m_sName << "] finished! "
                << std::endl;
    }

   private:
    std::string m_sName;
  };
  // lets create some callbacks (again ignoring memleaks, sorry for the laziness)
  // the intervals are specified in milliseconds
  // notice that internally, the TimeoutRouter uses TickTimers, thus our TickTimerHandler
  // will receive their events, too
  pTimeoutRouter->AddUpdateCall(1000, new TimeoutRouterCallback("PerSecondCallback"));
  // multiple calls can be added for the same time, and only one timer will be created
  pTimeoutRouter->AddUpdateCall(1000, new TimeoutRouterCallback("AnotherSecondCallback"));

  // it is also possible to register one callback for multiple times
  TimeoutRouterCallback* pCallback = new TimeoutRouterCallback("MultiTimeCallbacks");
  pTimeoutRouter->AddUpdateCall(1000, pCallback);
  pTimeoutRouter->AddUpdateCall(1250, pCallback);
  pTimeoutRouter->AddUpdateCall(3333, pCallback);
  // usually, callbacks are registered until they are unregistered again.
  // However, it is possible to set the repetition counts, so after 100 updates,
  // it will automatically be removed, and the CallbackDoneHandshake is called.
  pCallback->SetRepeatCount(10);
}

int main(int argc, char* argv[]) {
  try {
    VistaSystem* pVistaSystem = new VistaSystem();

    std::list<std::string> liSearchPaths;
    liSearchPaths.push_back("../configfiles/");
    pVistaSystem->SetIniSearchPaths(liSearchPaths);
    pVistaSystem->Init(argc, argv);

    // Show the time types and VistaTimeUtils
    ShowTimeUtils();

    // demonstrate the usage of the VistaTimer
    ShowVistaTimer();

    // next: set up an event handler to pint times of SystemEvents
    TimePrintObserver* pPrintObserver = new TimePrintObserver(pVistaSystem);

    // Set up TimeEvent handles
    SetupTimeEventHandlers(pVistaSystem);

    pVistaSystem->Run();

    delete pPrintObserver;
  } catch (VistaExceptionBase& e) { e.PrintException(); } catch (std::exception& e) {
    std::cerr << "Exception:" << e.what() << std::endl;
  }

  return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "main.cpp"                                                    */
/*============================================================================*/
