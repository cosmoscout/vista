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


#include "VistaMsg.h"
#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>

#include <limits>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMsg::VistaMsg()
: m_iType( 0 )
, m_iTicket( -1 ) // invalidate
, m_bSuccess( true )
{

}

VistaMsg::VistaMsg(const VistaMsg &rMsg)
: m_veMsg( rMsg.m_veMsg )
, m_veAnswer( rMsg.m_veAnswer )
, m_iType( rMsg.m_iType )
, m_iTicket(rMsg.m_iTicket )
, m_bSuccess( rMsg.m_bSuccess )
{
}

VistaMsg::~VistaMsg()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

// #############################################
// ##########    VistaMsg    ############
// #############################################

VistaMsg::MSG VistaMsg::GetThisMsg() const
{
	return m_veMsg;
}

VistaMsg::MSG &VistaMsg::GetThisMsgRef()
{
	return m_veMsg;
}

const VistaMsg::MSG &VistaMsg::GetThisMsgConstRef() const
{
	return m_veMsg;
}

void VistaMsg::SetThisMsg(const MSG &sMsg)
{
	m_veMsg.assign(sMsg.begin(), sMsg.end());
}


VistaMsg::MSG VistaMsg::GetMsgAnswer() const
{
	return m_veAnswer;
}

void VistaMsg::SetMsgAnswer(const MSG &sAnswer)
{
	m_veAnswer.assign(sAnswer.begin(), sAnswer.end());
}

int VistaMsg::GetMsgType() const
{
	return m_iType;
}

void VistaMsg::SetMsgType(int iType)
{
	m_iType = iType;
}

int VistaMsg::GetMsgTicket() const
{
	return m_iTicket;
}

void VistaMsg::SetMsgTicket(int iTicket)
{
	m_iTicket = iTicket;
}

bool VistaMsg::GetMsgSuccess() const
{
	return m_bSuccess;
}

void VistaMsg::SetMsgSuccess(bool bSuccess)
{
	m_bSuccess = bSuccess;
}


int VistaMsg::Serialize(IVistaSerializer &out) const
{
	int iLength = out.WriteInt32(m_iType);
	iLength += out.WriteInt32(m_iTicket);
	iLength += out.WriteBool(m_bSuccess);

	// does size actually fit in 32 bit?
	if( m_veMsg.size() > numeric_limits<VistaType::uint32>::max() )
		VISTA_THROW("VistaMsg::Serialize - vector too large for 32 bit!", 0);
	iLength += out.WriteInt32(VistaType::uint32(m_veMsg.size()));

	if(m_veMsg.size())
		iLength += out.WriteRawBuffer(&m_veMsg[0], (int)m_veMsg.size());

	if( m_veAnswer.size() > numeric_limits<VistaType::uint32>::max() )
		VISTA_THROW("VistaMsg::Serialize - vector too large for 32 bit!", 0);        
	iLength += out.WriteInt32(VistaType::uint32(m_veAnswer.size()));

	if(m_veAnswer.size())
		iLength += out.WriteRawBuffer(&m_veAnswer[0], (int)m_veAnswer.size());

	return iLength;
}

int VistaMsg::DeSerialize(IVistaDeSerializer &in)
{
	int iLength = in.ReadInt32(m_iType);
	iLength += in.ReadInt32(m_iTicket);
	iLength += in.ReadBool(m_bSuccess);
	int iLen=0;
	iLength += in.ReadInt32(iLen);
	if(iLen)
	{
		m_veMsg.resize(iLen);
		iLength += in.ReadRawBuffer(&m_veMsg[0], iLen);
	};
	iLength += in.ReadInt32(iLen);
	if(iLen)
	{
		m_veAnswer.resize(iLen);
		iLength += in.ReadRawBuffer(&m_veAnswer[0], iLen);
	}
	else
		m_veAnswer.clear();

	return iLength;
}

string VistaMsg::GetSignature() const
{
	return "VistaMsg";
}


int VistaMsg::AssignMsgByString(const string &sString, MSG &rMsg)
{
	rMsg.clear();
	rMsg.reserve(sString.length());
	int i=0;
	for(string::const_iterator cit = sString.begin(); cit != sString.end(); ++cit, ++i)
		rMsg.push_back(*cit);
	return i;	
}

int VistaMsg::AssignStringByMsg(const MSG &rMsg, string &sStorage)
{
	string sRet;
        #if defined(LINUX) || defined(DARWIN) || (defined(WIN32) && _MSC_VER>=1300)
	sRet = string(rMsg.begin(), rMsg.begin()+rMsg.size());
	#else
	sRet = string((char*)rMsg.begin(), (char*)rMsg.begin()+rMsg.size());
	#endif

	sStorage = sRet;
	return (int)sStorage.size();
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


