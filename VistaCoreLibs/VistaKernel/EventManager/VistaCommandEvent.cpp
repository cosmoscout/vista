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


#include "VistaCommandEvent.h"
#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>

#include <string>
using namespace std;


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
int VistaCommandEvent::m_nEventId = VistaEvent::VET_INVALID;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaCommandEvent::VistaCommandEvent()
{
	SetType(VistaCommandEvent::GetTypeId());
	SetId(VEIDC_CMD);
	m_iMethodToken = 0;
	m_iMessageTicket = -1;
	m_pMsg = NULL;
}

VistaCommandEvent::~VistaCommandEvent()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaCommandEvent::UsesPropertyListAnswer() const
{
	return (m_mpPropertyList.size()>0);
}

int VistaCommandEvent::Serialize(IVistaSerializer &rSer) const
{
	int iRet = VistaEvent::Serialize(rSer);
	iRet += rSer.WriteInt32(m_iMethodToken);
	iRet += rSer.WriteInt32(m_iMessageTicket);
	iRet += VistaPropertyList::SerializePropertyList(rSer, m_mpPropertyList, "VistaCommandEvent.m_mpPropertyList");
	return iRet;
}

/**
* Think of this as "LOAD"
*/
int VistaCommandEvent::DeSerialize(IVistaDeSerializer &rDeSer)
{
	int iRet = VistaEvent::DeSerialize(rDeSer);
	iRet += rDeSer.ReadInt32(m_iMethodToken);
	iRet += rDeSer.ReadInt32(m_iMessageTicket);
	m_mpPropertyList.clear(); // kill old values
		std::string sName;
		iRet += VistaPropertyList::DeSerializePropertyList(rDeSer, m_mpPropertyList, sName);

		//m_mpPropertyList.PrintPropertyList();
	return iRet;
}


string VistaCommandEvent::GetSignature() const
{
	return "VistaCommandEvent";
}

int VistaCommandEvent::GetMethodToken() const
{
	return m_iMethodToken;
}

void VistaCommandEvent::SetMethodToken(int iMethodToken)
{
	m_iMethodToken = iMethodToken;
}

int  VistaCommandEvent::GetMessageTicket() const
{
	return m_iMessageTicket;
}

void VistaCommandEvent::SetMessageTicket(int iMethodTicket)
{
	m_iMessageTicket = iMethodTicket;
}

VistaPropertyList VistaCommandEvent::GetPropertyList() const
{
	return m_mpPropertyList;
}
void VistaCommandEvent::ClearPropertyList()
{
	//cout << "VistaCommandEvent::ClearPropertyList()" << std::endl;
	m_mpPropertyList.clear();
}

int VistaCommandEvent::SetProperty(const VistaProperty &rProp)
{
/*	cout << "VistaCommandEvent()::SetProperty("
			 << rProp.GetNameForNameable() << ")" << std::endl;
			 */
	m_mpPropertyList[rProp.GetNameForNameable()] = rProp;
	return 1;
}

int VistaCommandEvent::GetProperty(VistaProperty &rProp)
{
	rProp = GetPropertyByName(rProp.GetNameForNameable());
	return 1;
}

VistaProperty VistaCommandEvent::GetPropertyByName(const string &sPropName)
{
	VistaPropertyList::const_iterator cit = m_mpPropertyList.find(sPropName);
	if(cit != m_mpPropertyList.end())
	{
		return (*cit).second;
	}
	return VistaProperty();
}

bool VistaCommandEvent::SetPropertyByName(const string &sPropName,
							   const string &sPropValue)
{
	m_mpPropertyList.SetValue(sPropName, sPropValue);
	return true;
}

int VistaCommandEvent::SetPropertiesByList(const VistaPropertyList &rList)
{
	m_mpPropertyList.clear();
	m_mpPropertyList = rList;

	return (int)m_mpPropertyList.size();
}

int VistaCommandEvent::GetPropertiesByList(VistaPropertyList &rList)
{
	rList = m_mpPropertyList;
	return (int)rList.size();
}


int VistaCommandEvent::GetPropertySymbolList(list<string> &rStorageList)
{
	int i=0;
	for(VistaPropertyList::const_iterator cit = m_mpPropertyList.begin();
		cit != m_mpPropertyList.end(); ++cit)
		{
			rStorageList.push_back((*cit).first);
			++i;
		}
		return i;
}

string VistaCommandEvent::GetPropertyDescription(const string &sPropName)
{
	return "CommandEvent: no description\n";
}

void VistaCommandEvent::SetCommandMsg(VistaMsg *pMsg)
{
	m_pMsg = pMsg;
}

VistaMsg *VistaCommandEvent::GetCommandMsg() const
{
	return m_pMsg;
}


std::string VistaCommandEvent::GetIdString(int nId) 
{
	switch (nId)
	{
	case VEIDC_CMD:
		return  "VEIDC_CMD";
	case VEIDC_LAST:
		return "VEIDC_LAST";
	default:
		return VistaEvent::GetIdString(nId);
	}
}

int VistaCommandEvent::GetTypeId()
{
	return VistaCommandEvent::m_nEventId;
}

void VistaCommandEvent::SetTypeId(int nId)
{
	if(VistaCommandEvent::m_nEventId == VistaEvent::VET_INVALID)
		VistaCommandEvent::m_nEventId = nId;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


