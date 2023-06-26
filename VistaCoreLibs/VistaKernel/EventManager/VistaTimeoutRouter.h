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

#ifndef _VISTATIMEOUTROUTER_H
#define _VISTATIMEOUTROUTER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaTimeoutHandler.h"

#include <list>
#include <map>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaEventManager;
class VistaClusterMode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A utility class for the management of timeouts and the registration
 * of callback routines for those timeouts.
 * The router compiles timeout requests for a single timestamp in a list
 * of update-routines. This allows the re-use of HD_TIMER instances
 * and takes some pressure off the event bus system.
 * Timeouts can be configured to be repetitious or for a finite number
 * of repetitions (and infinite). The repeat count for a time callback
 * can be changed dynamically as it is evaluated after each callback anew.
 * Use as follows.
 * - create a timeout router instance
 * - register a number of CTimedCallback instances (you have to subclass here)
         use the VistaTimeoutRouter::AddUpdateCall() api.
 * - the timeout router does *only* manage its internal memory, so
         you have to care for valid pointers to CTimedCallback instances
         as well as the release of the memory once the callback is not to be
         used anymore.
 */
class VISTAKERNELAPI VistaTimeoutRouter : public VistaTimeoutHandler {
 public:
  VistaTimeoutRouter(VistaEventManager* pEvMgr, VistaClusterMode* pClusterMode);
  virtual ~VistaTimeoutRouter();

  /**
   * The default number of repetitions is infinite.
   */
  class VISTAKERNELAPI CTimedCallback {
   public:
    virtual ~CTimedCallback() {
    }

    bool Callback(double dTimeStamp, double dDesiredLength, double dRealLength);

    int  GetRepeatCount() const;
    void SetRepeatCount(int nRepeatCount);

    virtual void CallbackDoneHandshake() {
    }

   protected:
    /**
     * IMPLEMENT THIS ROUTINE IN A SUBCLASS.
     */
    virtual bool DoCallback(double dTimeStamp, double dDesiredLength, double dIntervalLength) = 0;
    CTimedCallback()
        : m_nRepeatCount(-1) // inifinite
    {
    }

    int m_nRepeatCount;
  };

  /**
   * Register an update callback with this router. An instance pointer
   * to a CTimedCallback can be registered more than once with differnent
   * update timespans, e.g. register the same instance for 1000msecs and
   * for 500msecs.
   */
  void AddUpdateCall(int iMsecs, CTimedCallback*);

  /**
   * Remove an update call. You have to remember the timespan that you registered
   * it for
   * @return false iff the callback instance was not registered with the
                                   timing given in iMsecs
   */
  bool RemUpdateCall(int iMsecs, CTimedCallback*);

  // ##################################################################
  // OVERLOADED FROM VistaTimeoutHandler
  // ##################################################################
  void HandleTimeout(HD_TIMER tim);
  void HandleEvent(VistaEvent* pEvent);

 protected:
 private:
  /**
   * we map milliseconds to timer instances
   */
  std::map<int, HD_TIMER> m_mpTimers;
  /**
   * once a timer is found, we map it to a list of
   * update candidates
   */
  typedef std::list<CTimedCallback*>  CBLIST;
  typedef std::map<HD_TIMER, CBLIST*> CANDLIST;
  CANDLIST                            m_mpUpdateCandidates;

  typedef std::pair<HD_TIMER, CTimedCallback*> REMPAIR;
  typedef std::list<REMPAIR>                   TIMLIST;
  TIMLIST                                      m_liScheduledRemoval;

  bool CallUpdaters(CBLIST* pList, HD_TIMER tim);
  void DoReleaseTim(const REMPAIR& tim);

  double m_dEventTick;
  bool   m_bInCallback;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
