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

#ifndef _VISTAOBSERVER_H
#define _VISTAOBSERVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaAspectsConfig.h"

#include "VistaNameable.h"
#include "VistaObserveable.h"

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
 * A basic interface for a simple observer-pattern implementation.
 * Observers and observeables are usually in a n:1 relation, but this
 * may not be the mandatory state. It is possible that an observer
 * can outlive its observeable (i.e. an editing divasplog may outlive
 * its edited entity). For that reason, the simple observer-pattern
 * presented by Gamma et.al. is enhanced with a Delete/Detach-Scheme
 * which reflects a n:m relation in addition to a varied lifespan.
 */
class VISTAASPECTSAPI IVistaObserver {
 public:
  virtual ~IVistaObserver();

  /**
   * An observeable is about to be deleted, this can be forbidden by returning
   * false here.
   * Imagine the situation where an observeable is edited and listed somewhere else.
   * The user tries to remove it from a list, forgetting that it is currently
   * edited. In that case, the edit-divasplog may pop up a request to confirm the deletion,
   * and return true here. If the request is denied (this was a mistake) this routine
   * can return false here. An observable must only be deleted iff all observes return
   * true here.
   * Default implementation always returns true (by default, all observables can be deleted)
   * @param pObserveable the observeable to be freed
   * @return true iff pObserveable can be deleted
   */
  virtual bool ObserveableDeleteRequest(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE);

  /**
   * This is sent to the observer before the observeable is deleted. Note that this
   * is not a question, but a notification. After this notification passed, the observer
   * can be sure that the observeable is deleted and no more available.
   * Note: you should not rely on the fact that any other part of the observeable is valid
   * than the members of the IVistaObserveable class! This method may be called from
   * an IVistaObserveable::~IVistaObserveable() which is run AFTER all specialized
   * destructors have already had their chance to cleanup! Especially dynamic_casts<>
   * may fail in this method. Use it only to remove and pending reference in the observer's
   * data structure which is identifiable by the pointer to the base instance alone.
   * IsObserved(pOberveable) must return false then.
   * @param pObserveable the observeable to be deleted
   */
  virtual void ObserveableDelete(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE);

  /**
   * This message is sent when pObserveable shall be released and not be observed anymore.
   * Note that this is the case when an observeable is not deleted, but only "moved" or
   * out of scope.
   * IsObserved(pObserveable) must return false after this message.
   * @param pObserveable the observeable to be released.
   */
  virtual void ReleaseObserveable(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE);

  /**
   * This message is sent to the observer when the state of pObserveable changed. Note that
   * this is the only information transmitted, the observer can query the new state of pObserveable
   * using the public interface of pObserveable. Note that an observer usually knows the real type
   * or at least the least-known-type of the current IVistaObserveable, so a downcast should be used
   * to query the current state of pObserveable. An Observer can have more than one observeable that
   * is observed.
   * @param pObserveable the observeable that changed its state.
   */
  virtual void ObserverUpdate(IVistaObserveable* pObserveable, int nMsg, int nTicket) = 0;

  /**
   * This method can be used to query whether pObserveable is observed by this observer or not.
   * Note that a concrete implementation of an observe may choses its way of storing observeables,
   * @param pObserveable the observeable to query
   * @return true iff this observes pObserveable, else false
   */
  virtual bool Observes(IVistaObserveable* pObserveable);

  /**
   * This method can be used to attach an observeable to this observer. A multiple call to Observe
   * must not change the state of this observer (an observeable is only observed ONCE).
   * IsObserved(pObserveable) must return true after a call to this
   * @param pObserveable the observeable to be observed
   */
  virtual void Observe(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE);

  void UpdateRequest(IVistaObserveable* pObserveable, int nMsg, int nTicket);

  bool         GetIsObserverEnabled() const;
  virtual void SetIsObserverEnabled(bool bEnabled);

 protected:
  IVistaObserver();

  // list of objects observed by this observer
  // Note: this is implemented as a vector (even though we have to perform
  // operations like deleting arbitrary elements), as this is nevertheless
  // way faster than on an std::list.
  std::vector<IVistaObserveable*> m_vecObserveables;

 private:
  /**
   * Copy-constructor. We prevent copying.
   */
  IVistaObserver(IVistaObserver&);

  bool m_bObserverEnabled;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOBSERVER_H
