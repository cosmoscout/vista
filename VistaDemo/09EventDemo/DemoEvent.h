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


#ifndef _DEMOEVENT_H
#define _DEMOEVENT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/EventManager/VistaEvent.h>
#include <string>


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * CDemoEvent - an example for an application-defined event
 */
class DemoEvent : public VistaEvent
{
public:
    
	DemoEvent();

	virtual ~DemoEvent();

	virtual std::string GetName() const 
	{ 
		return VistaEvent::GetName()+"::CDemoEvent"; 
	}

	enum
	{
		EID_DEMOEVENT,
		EID_LAST
	};

	static int GetTypeId();
	static void SetTypeId( int iId );

	static std::string GetIdString( int iId );

private:
	static int  m_iEventId;
};


#endif //_DEMOEVENT_H
