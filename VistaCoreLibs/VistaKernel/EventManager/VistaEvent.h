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


#ifndef _VISTAEVENT_H
#define _VISTAEVENT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaAspects/VistaSerializable.h>
#include <VistaBase/VistaBaseTypes.h>

#include <VistaKernel/VistaKernelConfig.h>

#include <string>
#include <iostream>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaEventManager;

// prototypes
class   VistaEvent;
VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaEvent & );


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaEvent - the mother of all events ;-)
 */
class VISTAKERNELAPI VistaEvent : public IVistaSerializable
{
public:
	/**
	 * Event types, supported by Vista
	 */
	enum VISTAKERNELAPI EVENT_TYPE
	{
		VET_INVALID		= -1,
		VET_ALL			= -2
	};

	enum VISTAKERNELAPI EVENT_ID
	{
		VEID_NONE = -1,
		VEID_LAST
	};

public:

	virtual ~VistaEvent();

	/**
	 * Returns the system time of the event's creation
	 * @return  VistaType::systemtime    event creation time
	 */
	VistaType::systemtime GetTime() const;

	/**
	 * Returns the type of the event
	 * @return  int    event type id
	 */
	int GetType() const;

	/**
	 * Returns, whether the event has already been handled
	 * @return  bool   true = already handled / false = not handled yet
	 */
	bool IsHandled() const;

	/**
	 * Sets the handling state of the object.
	 * Use this to mark the event as being handled.
	 * @param   bool bHandled
	 */
	void SetHandled( bool bHandled );

	/**
	 * Returns the name of the event.
	 * @return  std::string
	 */
	virtual std::string GetName() const;

	/**
	 * Prints out some debug information to the given output stream.
	 * @param   std::ostream& out
	 */
	virtual void Debug( std::ostream& oOut ) const;

	/**
	 * Returns the event id
	 * @return  int    event id
	 */
	int GetId() const;

	/**
	 * Sets the id of the event.
	 * @param   int iId
	 * @return  bool    true=success / false=failure (i.e. id=INVALID)
	 */
	virtual bool SetId( int iEventId );


	/**
	 * Think of this as "SAVE"
	 */
	virtual int Serialize( IVistaSerializer& oSerializer) const;

	/**
	 * Think of this as "LOAD"
	 */
	virtual int DeSerialize( IVistaDeSerializer& oDeSerializer );

	virtual std::string GetSignature() const;

	static int GetTypeId();
	static void SetTypeId(int nId);

	static std::string GetIdString(int nId);

	int GetCount() const;

protected:
	// avoid instantiation of this class -> use a derived class, instead!
	VistaEvent( const int iEventType = VET_INVALID,
				const int iEventID = VET_INVALID );		
	void SetType(int iType);

	bool		m_bHandled;	// already handled?
	VistaType::systemtime	m_nTime;	// creation time of event (gets set by CEventManager)

private:
	/** @todo: static EventID may be confused with "normal" eventId */
	static int	m_nEventId;
	int			m_iType;	// the type of the event
	int			m_iId;		// the event id
	int			m_nCount;

	friend class VistaEventManager;
};

#endif //_VISTAEVENT_H

