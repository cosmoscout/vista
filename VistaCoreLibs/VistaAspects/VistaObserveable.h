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


#ifndef _VISTAOBSERVEABLE_H
#define _VISTAOBSERVEABLE_H

#include "VistaAspectsConfig.h"


#include <list>
#include <set>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaObserver;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


/**
 * This is an interface class for a basic implementation of a simple observer-pattern
 * as presented by Gamma et.al.
 * This class defines the interface for observables, meaning entities that can change
 * their state and inform multiple observers about this state change.
 * Observers are kept in a list which is managed by this class. Note that observers may
 * outlive their observables in this implementation, so there has to be a deletion scheme that
 * assures that there are no dangling pointers in different observers.
 * For that reason, this class implements a deletion scheme that informs all attached
 * observers about the deletion. Note that the clearing of stored pointers is part of the
 * observer implementation.
 * One thing to notice about event notifications and change sets:
 * - you can turn off notifications when doing a series of updates (called 'set-run')
	 using SetNotificationFlag())
 * - during the set-run, when observers are attached, a set of messages
	 is collected. You can retrieve the set using the GetChangedSetConstRef() API
 * - after the run, you can turn on notification then (SetNotificationFlag())
 * - you can send the update messages by iterating over the change set, but do not
	 forget to call ClearChangeSet() afterwards, otherwise some messages might
	 be sent twice.
 *
 * Another thing about the handling of DeleteMessages:
 * - a call to the destructor of an observable will call IVistaObserver::ObserveableDelete()
	 note that at the time of call, all specialized members are already DELETED by the specialized
	 destructors (destructors are called from the most specific node to the root), so
	 it is not wise to work on any member of an IVistaObserveable inside the method
	 IVistaObserver::ObserveableDelete() unless you know what you are doing!
	 This especially means not to downcast the observable pointer to anything else than
	 an IVistaObserveable itself!
 * - it surely is a wise thing to provide a de-registration for  observables besides the call
	 to "delete" with the instance pointer, you can use the MSG_PREDELETE then to indicate
	 what you intend to do for waiting views/observers.
 */
class VISTAASPECTSAPI IVistaObserveable
{
public:
	enum
	{
		MSG_PROBABLY_ALL_CHANGED=-2, /**< can be useful, e.g., after serialization, has to be triggered by user code */
		MSG_NONE=-1,   /**< should be regarded as error */
		MSG_ATTACH,    /**< NOT ANYMORE sent on a new attach  of an observer (using AttachObserver())*/
		MSG_DETACH,    /**< NOT ANYMORE sent after an observer detached (using DetachObserver())*/
		MSG_PREDELETE, /**< can be sent before deletion (by user code!)*/
		MSG_LAST       /**< for child-classes */
	};

	enum
	{
		TICKET_NONE=-1, /** enums for the ticketing system */
		TICKET_LAST     /** for child classes */
	};

	/**
	 * Destructor. Will notify all attached observers about the death of this instance
	 * by sending a ObserveableDelete() request. Note that during the run of the destructor,
	 * all base class destructors are already passed, so a down cast during the ObserveableDelete()
	 * method run is a no-go.
	 * @see SendDeleteRequest()
	 */
	virtual ~IVistaObserveable();

	/**
	 * this will add an observer to the list of observers that do observe this observeable.
	 * Note that an observer can be added more than once, although that does not make no sense ;)
	 * Calls Notify(MSG_ATTACH) after the attachment.
	 * @see m_lIVistaObservers()
	 * @param pObs the observer to add to the observer-list
	 * @return the current number of observers that do observe this instance
	 */
	virtual int AttachObserver(IVistaObserver *pObs, int eTicket = TICKET_NONE);

	/**
	 * Removes all occurrences of pObs in the associated list of observers.
	 * Calls Notify(MSG_DETACH) after the detachment.
	 * @param pObs the observer to detach from the observer list (all pointers)
	 * @return the number of observers in this instance's list after the detachment
	 */
	virtual int DetachObserver(IVistaObserver *pObs);


	/**
	 * This method can be used to query whether pObs is really an observer of this instance.
	 * @param pObs the observer to query
	 * @return true iff the observer is found in the internal bookkeeping (contains pObs)
	 */
	virtual bool GetIsObservedBy(IVistaObserver *pObs) const;

	/**
	 * This method will call IVistaObserver::ObserverUpdate(this) on any attached Observer.
	 * When Notification is turned off, and there are observers attached,
	 * the parameter eMsg is collected in the change set for this observable.
	 * They will reside in this change set, iff
	 * - a call to Notify() occurs, there are observers attached and
		 the notification is enabled
	 * - until the user explicitly calls ClearChangeSet()
	 *
	 * The rational behind the change set is to collect the update messages
	 * for pending observers. This is useful when you are doing a series
	 * of set-method (set-run) calls that can
	 * - set the object to an undefined state (partial set)
	 * - cause many time-consuming observer updates (due to callback nature
		 of this pattern in the C language)
	 *
	 * and you want to collect the messages, e.g. to call for each Notify()
	 * again, but the Observeable is in proper state now.
	 *
	 * @param eMsg the message to sent to the observer for this update.
				   This is useful for the observer in order to deduce what happened.
	 */
	virtual void Notify(int eMsg = MSG_NONE);

	/**
	 * This method can be used to temporarily switch notification on or off
	 * @param bNotify true iff notification should be performed on call to Notify(), false else
	 * @see GetNotificationFlag()
	 */
	void SetNotificationFlag(bool bNotifyEnabled);

	/**
	 * Retrieve the current state of the change notification flag.
	 * @see SetNotificationFlag()
	 * @return true when notification is enabled, false else
	 */
	bool GetNotificationFlag() const;


	/**
	 * Returns the set of messages that was collected when Notify() was called,
	 * but Notification was turned off by a call to SetNotificationFlag() with
	 * the parameter "false".
	 * You must (!) copy this set before trying to dispatch it with Notify() again!
	 * There is no need to copy it when you only inspect what is in the set.
	 * @return the internal set of messages that was collected. Read only.
	 */
	const std::set<int> &GetChangedSetConstRef() const;

	/**
	 * Clears the internal change set of messages.
	 * Call this method after retrieving the internal set of messages by
	 * a call to GetChangedSetConstRef(), before you call Notify() again
	 * and you made your copy of the change set.
	 */
	void ClearChangeSet();


	/**
	 * Copy constructor, it omits the list of attached observers.
	 * As the observes often do bookkeeping of their attached observables,
	 * this is the safest way of copying a subject.
	 * The observable that is created by using this copy operation is thus
	 * not observed by any observer, has an empty change set,
	 * but notification state is the same as in the original.
	 */
	IVistaObserveable(const IVistaObserveable &);

	/**
	 * Assignment operator, it omits the list of attached observers.
	 * As the observes often do bookkeeping of their attached observables,
	 * this is the safest way of copying a subject.
	 * The observable that is created by using this assignment is thus
	 * not observed by any observer, has an empty change set,
	 * but notification state is the same as in the original.
	 */
	IVistaObserveable &operator=(const IVistaObserveable &);
protected:

	/**
	 * Vanilla Constructor. Sets the notification flag to true, attached observer list
	 * is set to be the empty list, same accounts for the changed set.
	 */
	IVistaObserveable();

	/**
	 * An observable can be deleted in order to assure that no observer will keep references
	 * to this instance, one can kindly REQUEST deletion for this instance with this method.
	 * In case any observer does deny this request, deletion has to be rescheduled.
	 * @return true iff the instance's deletion was not objected by any attached observer
	 */
	virtual bool SendDeleteRequest();

	/**
	 * We can force deletion of this instance by sending a MESSAGE and not a request to all
	 * attached observers. Any observer has to make sure that any reference on this instance
	 * has to be removed when receiving this message.
	 * @see ~IVistaObserveable()
	 */
	virtual void SendDeleteMessage();


	/**
	 * Observers cannot be attached or detached during a Notify() call, as this might
	 * invalidate iterators used to process the list of observers.
	 * Therefore, when a Notify() call is currently running, a deferred attach / detach
	 * is done, that is executed after all currently existing observers have been notified.
	 */
	virtual void AttachObserverDeferred(IVistaObserver* pObs, int eTicket = TICKET_NONE);
	virtual void DetachObserverDeferred(IVistaObserver* pObs);

	/**
	 * Process all deferred actions. This is called from the Notify() function, after all
	 * notifications are sent out.
	 * Do not call this anywhere else!
	 */
	virtual void ProcessDeferredActions();

private:
	class OBSERVER_INFO
	{
	public:
		OBSERVER_INFO(IVistaObserver *pObs,
			int eTicket);

		IVistaObserver	*m_pObserver;
		int				 m_eTicket;

		bool operator==(const OBSERVER_INFO &) const;
		bool operator==(const IVistaObserver *) const;
	};

	/**
	 * The list of attached observers and their respective tickets.
	 * Notification will be done in the order of this list.
	 */
	std::vector<OBSERVER_INFO>  m_vecVistaObservers;

	/**
	 * The set of messages that are collected during a run of Notify() with
	 * notification turned off
	 */
	std::set<int>             m_sChangedSet;

	/**
	 * Reflects the state of the notification behavior for this instance.
	 */
	bool                 m_bNotificationFlag;


	/**
	 * Contains all deferred attach/detach actions in the order they are
	 * supposed to be called. The second entry is true iff it is an
	 * attach action.
	 */
	std::vector<std::pair<OBSERVER_INFO, bool> > m_vecDeferredActions;

	/** 
	 * Is true iff the notification process is currently active.
	 * During this time, all attach / detach actions are deferred to
	 * the end of the notification.
	 */
	bool m_bNotificationRunning;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOBSERVEABLE_H
