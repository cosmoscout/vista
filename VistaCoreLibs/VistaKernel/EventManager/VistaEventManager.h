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

#ifndef _VISTAEVENTMANAGER_H
#define _VISTAEVENTMANAGER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaEventHandler;
class VistaEventObserver;
class VistaEvent;
class VistaTimer;
class VistaClusterMode;

// prototypes
class VistaEventManager;
VISTAKERNELAPI std::ostream& operator<<(std::ostream&, const VistaEventManager&);

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaEventManager - the heart of ViSTA event handling
 */
class VISTAKERNELAPI VistaEventManager {
 public:
  /**
   * Sets the handling state of the object.
   * Use this to mark the event as being handled.
   *
   * @param   bool bHandled
   * @RETURN  --
   */
  VistaEventManager();
  virtual ~VistaEventManager();

  // ################################
  // IMPORTANT DEFINITIONS
  // ################################

  typedef int EVENTTYPE;
  typedef int EVENTID;

  enum { NVET_ALL = -2, NVET_INVALID = -1, NVET_NONE = 0 };

  enum { PRIO_FALLBACK = -1, PRIO_LOW = 0, PRIO_MID = 4096, PRIO_HIGH = 8192 };

  /**
   * Checks, whether the given event type is valid,
   * i.e. is a system event or has been registered
   * by some external source.
   * (NOTE: returns "false" for VistaEvent::VET_ALL!!!)
   *
   * @param   int iEventType
   * @RETURN  bool    true=valid event / false=invalid event
   */
  bool IsValidEventType(int iEventType) const;

  /**
   * Adds an event handler to the list of possible handlers for events of type
   * nEventType and id nEventId. Both, type and id, must be registered before
   * a call to this routine. Priority is defined as follows: handlers with
   * higher priority get events earlier than handlers with lower priority.
   * In principle, any value for int is legal, the pre-defined values PRIO_HIGH,
   * PRIO_MID, PRIO_LOW and PRIO_FALLBACK only help for most cases to define
   * reasonable regions. The same handler may not be registered twice (by direct
   * pointer comparison).
   * @see RegisterEventType()
   * @see RegisterEventId()
   * @param pHandler a non NULL pointer to a handler
   * @param nEventType the event-type id to register for
   * @param nEventId the event-id to register for the specific type
   * @param nPriority a sorting criteria,
                                          the higher, the earlier an event will be passed
                                          to the handler
   * @return true iff the handler was registered successfully,
                                  false can mean that einer pHandler was NULL
                                  or it was tried to register the VistaEventHandler
                                  to a non-registered id or type or pHandler
                                  was already registered.
   */
  bool AddEventHandler(VistaEventHandler* pHandler, EVENTTYPE nEventType, EVENTID nEventId,
      int nPriority = PRIO_HIGH);

  /**
   * Removes pHandler from the list of event handlers for event type nEventType
   * and nEventId.
   * @param pHandler the handler to remove (may not be NULL)
   * @param nEventType the type to check
   * @param nEventId the id to check
   * @return true iff the handler was found and removed from the handler list.
   */
  bool RemEventHandler(VistaEventHandler* pHandler, EVENTTYPE nEventType, EVENTID nEventId);

  int GetPriority(VistaEventHandler* pHandler, EVENTTYPE nEventType, EVENTID nEventId) const;

  bool SetPriority(
      VistaEventHandler* pHandler, EVENTTYPE nEventType, EVENTID nEventId, int nPriority);

  /**
   * Registers a new non-exclusive observer for the given event type.
   * Event observers can be registered during event processing, but
   * are cached in registration/unregistration queues. A problem arises
   * when event obervers are registered and unregistered in the very
   * same application processing frame.
   * As a compromise, the following can be said
   * - <register>+ sequences (registration whishes are only valid on unique instances)
           The same accounts to <unregister>+ sequences
   * - register, unregister sequences are ok and result in the state "unregistered"
   * - unregister, register sequences result in state "unregistered"
   *
   * Additionally be aware that registered observers get first events only
   * in the next ProcessEvent() cycle
   * @see UnregisterObserver()
   * @see ProcessEvent()
   */
  bool RegisterObserver(VistaEventObserver* pObserver, int iEventType);

  /**
   * Unregisters an observer for the given event type. If the given
   * event type is VistaEvent::VET_ALL, the observer is removed from
   * all lists, i.e. it won't receive any further event notifications.
   * Note that Unregistration requests are pooled until the current
   * ProcessEvent() loop is over. As a consequence, one should not try
   * to unregister observers in their destructors when an upcoming
   * event will trigger them.
   * @see RegisterObserver()
   * @see ProcessEvent()
   *
   * @RETURN  bool    true=no error / false=invalid observer or event type
   */
  bool UnregisterObserver(VistaEventObserver* pObserver, int iEventType);

  /**
   * Print out some status information, e.g. registered event types and handlers,
   * into the given stream.
   *
   * @param   std::ostream & out
   */
  virtual void Debug(std::ostream& out) const;

  /**
   * Initializes the event manager,
   * i.e. cleans up all lists and references to
   * event handlers and observers.
   *
   * @param   --
   * @RETURN  bool    true/false
   */
  bool Init();

  /**
   * Puts a new event into the system,
   * i.e. notifies all interested observers
   * and appropriate event handlers.
   * (NOTE: From this point on, the event pointer
   * is guaranteed to be valid, i.e. not NULL.)
   *
   * @param   VistaEvent *pEvent must not be NULL
   * @RETURN  bool    true=no error / false=invalid event
   */
  bool ProcessEvent(VistaEvent* pEvent);

  /**
   * @param the debug string to printout when the debugging facility
                    is used for the event manager.
   * @return the eventtype handle for a new event type that is to be
                     used for upcoming requests regarding the events.
   */
  EVENTTYPE RegisterEventType(const std::string& sDebugString);

  /**
   * @param nEventType the type for which to register new ids
   * @return the event id registered for the EventType given
   */
  EVENTID RegisterEventId(int nEventType, const std::string& sDebugString);

  /**
   * determines if the time stamp of an event is set to the local time or not.
   * This is of interest in client mode: the time stamp is serialized along
   * with the event. This method allows to explicitly reset the event time
   * to the local clock of the client.
   *
   * @param bool bRETTLT
   */
  void SetResetEventTimeToLocalTime(bool bRETTLT);

  bool GetIsObserver(VistaEventObserver*, int nType) const;

 protected:
 private:
  // #################################################################
  // NEW EVENT API
  // #################################################################

  struct _qCompPrio;

  class HANDLER {
   public:
    HANDLER(int n, VistaEventHandler* pE)
        : m_pHandler(pE)
        , m_nPrio(n) {
    }

    bool operator==(const HANDLER&);

    VistaEventHandler* m_pHandler;
    int                m_nPrio;
  };

  typedef std::list<VistaEventObserver*>            OBSQUEUE;
  typedef std::list<HANDLER>                        HANQUEUE;
  typedef std::pair<std::string, HANQUEUE*>         HQUEUE;
  typedef std::pair<std::vector<HQUEUE>, OBSQUEUE*> HOPAIR;
  typedef std::pair<std::string, HOPAIR>            HANP;

  /**
   * The handler mapping is as follows:
   * - each type points to a single observer list and a vector of handler, one entry for each type
   * - the handler entries carry a pointer to a handler and a priority, which is used as
           a sorting key
   * - we need symbolic names for debugging purposes, for that reason, the pair construction
           is used to store the name outside of the element, but with a defined mapping
   */
  std::vector<HANP> m_veHandlerMapping;

  HANQUEUE* GetHandlerList(EVENTTYPE nEventType, EVENTID nEventId) const;

  int AddEventType(const std::string& sDebugString);
  int AddEventId(int nEventType, const std::string& sDebugString);

  bool AddToIdList(HANP&, EVENTID eId, int nPrio, VistaEventHandler*);
  bool RemoveFromIdList(HANP&, EVENTID eId, VistaEventHandler*);

  bool CleanupHandlerMapping();

  // #################################################################
  // ProcessEvent internal variables
  // #################################################################
  int  m_iProcessRecursionCount;
  int  m_nEventCount;
  bool m_bResetEventTimeToLocalTime;

  // #################################################################
  // OBSERVER API
  // #################################################################

  // queues for pending registrations/unregistrations
  typedef std::set<std::pair<VistaEventObserver*, int>> OBSSET;
  bool                                                  m_bObserverQueueDirty;

  int  HandleObserverQueueChange();
  bool DoRegisterObserver(VistaEventObserver*, int);
  bool DoUnregisterObserver(VistaEventObserver*, int);

  OBSSET m_stPendingForRegistration;
  OBSSET m_stPendingForUnregistration;
};

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/

#endif //_VISTAEVENTMANAGER_H
