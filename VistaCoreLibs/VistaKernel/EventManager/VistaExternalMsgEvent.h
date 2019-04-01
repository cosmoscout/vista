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


#ifndef _VISTAEXTERNALMSGEVENT_H
#define _VISTAEXTERNALMSGEVENT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include "VistaSystemEvent.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaMsg;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaExternalMsgEvent : public VistaSystemEvent
{
public:
	enum
	{
		VEID_FIRST = 0,
		VEID_INCOMING_MSG=0,
		VEID_LAST
	};


	VistaExternalMsgEvent();
	virtual ~VistaExternalMsgEvent();

	VistaMsg *GetThisMsg() const;
	void             SetThisMsg(VistaMsg *);

	/**
	 * Think of this as "SAVE"
	 */
	virtual int Serialize(IVistaSerializer &) const;

	/**
	 * Think of this as "LOAD"
	 */
	virtual int DeSerialize(IVistaDeSerializer &);

	virtual std::string GetSignature() const;

	bool	SetId(int iId);

	static int GetTypeId();
	static void SetTypeId(int nId);
	static std::string GetIdString(int nId);
protected:
private:
	VistaMsg *m_pKernelMsg;
	static int m_nEventId;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAEXTERNALMSGEVENT_H

