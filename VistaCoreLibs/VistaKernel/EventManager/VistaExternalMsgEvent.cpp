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


#include "VistaExternalMsgEvent.h"

#include "VistaKernel/VistaKernelMsgPort.h"
#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

int VistaExternalMsgEvent::m_nEventId = VistaEvent::VET_INVALID;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaExternalMsgEvent::VistaExternalMsgEvent()
{
	SetType(VistaExternalMsgEvent::GetTypeId());
	SetId(VistaEvent::VEID_NONE);
	m_pKernelMsg = NULL;
}

VistaExternalMsgEvent::~VistaExternalMsgEvent()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaMsg *VistaExternalMsgEvent::GetThisMsg() const
{
	return m_pKernelMsg;
}


void             VistaExternalMsgEvent::SetThisMsg(VistaMsg *pMsg)
{
	m_pKernelMsg = pMsg;
}

int VistaExternalMsgEvent::Serialize(IVistaSerializer &rSer) const
{
	int iLength = VistaSystemEvent::Serialize(rSer);
	iLength += rSer.WriteBool(m_pKernelMsg != NULL);
	if(m_pKernelMsg)
		iLength += (*m_pKernelMsg).Serialize(rSer);
	return iLength;
}

int VistaExternalMsgEvent::DeSerialize(IVistaDeSerializer &rDeSer)
{
	int iLength = VistaSystemEvent::DeSerialize(rDeSer);
	bool bHasMsg = false;
	iLength += rDeSer.ReadBool(bHasMsg);
	if(bHasMsg)
	{
		iLength += (*m_pKernelMsg).DeSerialize(rDeSer);
	}
	return iLength;
}

std::string VistaExternalMsgEvent::GetSignature() const
{
	return "VistaExternalMsgEvent";
}

bool VistaExternalMsgEvent::SetId(int iId)
{
	if (iId > VSE_INVALID && iId < VEID_LAST)
	{
		VistaEvent::SetId(iId);
		return true;
	}
	VistaEvent::SetId(VSE_INVALID);
	return false;
}

int VistaExternalMsgEvent::GetTypeId()
{
	return VistaExternalMsgEvent::m_nEventId;
}

void VistaExternalMsgEvent::SetTypeId(int nId)
{
	if(VistaExternalMsgEvent::m_nEventId == VistaEvent::VET_INVALID)
		VistaExternalMsgEvent::m_nEventId = nId;
}


std::string VistaExternalMsgEvent::GetIdString(int nId) 
{
	switch (nId)
	{
	case VEID_INCOMING_MSG:
		return  "VEID_INCOMING_MSG";
	case VEID_LAST:
		return "VEID_LAST";
	default:
		return VistaSystemEvent::GetIdString(nId);
	}
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


