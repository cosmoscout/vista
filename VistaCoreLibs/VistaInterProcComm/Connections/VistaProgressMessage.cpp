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

#include <string>
using namespace std;

#include "VistaProgressMessage.h"

#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaProgressMessage::VistaProgressMessage()
{
		m_iTotalProgress = 0;
		m_iSubtaskProgress = 0;
		m_iIconStateMax = 7;
		m_bHideFlag = false;
}

VistaProgressMessage::~VistaProgressMessage()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
string VistaProgressMessage::GetApplicationName() const
{
	return m_sApplicationName;
}

void        VistaProgressMessage::SetApplicationName(const string &sApplicationName)
{
	m_sApplicationName = sApplicationName;
}


int         VistaProgressMessage::GetIconState() const
{
	return m_iTotalProgress;
}

void        VistaProgressMessage::SetIconState(int iState)
{
	m_iTotalProgress = iState;
}

int	    VistaProgressMessage::GetIconStateMax() const
{
	return m_iIconStateMax;
}

void        VistaProgressMessage::SetIconStateMax(int iStateMax)
{
	m_iIconStateMax = iStateMax;
}

int         VistaProgressMessage::GetTotalProgress() const
{
	return m_iTotalProgress;
}

void        VistaProgressMessage::SetTotalProgress(int iTotalProgress)
{
	m_iTotalProgress = iTotalProgress;
}

string VistaProgressMessage::GetTotalProgressLabel() const
{
	return m_sProgressLabel;
}

void        VistaProgressMessage::SetTotalProgressLabel(const string &sTotalProgressLabel)
{
	m_sProgressLabel = sTotalProgressLabel;
}

int         VistaProgressMessage::GetSubtaskProgress() const
{
	return m_iSubtaskProgress;
}

void        VistaProgressMessage::SetSubtaskProgress(int iSubtaskProgress)
{
	m_iSubtaskProgress = iSubtaskProgress;
}

string VistaProgressMessage::GetSubtaskProgressLabel() const
{
	return m_sSubtaskLabel;
}

void        VistaProgressMessage::SetSubtaskProgressLabel(const string &sSubtaskLabel)
{
	m_sSubtaskLabel = sSubtaskLabel;
}


bool        VistaProgressMessage::GetHideFlag() const
{
	return m_bHideFlag;
}

void        VistaProgressMessage::SetHideFlag(bool bFlag)
{
	m_bHideFlag = bFlag;
}


string VistaProgressMessage::GetPictureName() const
{
	return m_sPictureName;
}

void        VistaProgressMessage::SetPictureName(const string &sPicName)
{
	m_sPictureName = sPicName;
}

int VistaProgressMessage::Serialize(IVistaSerializer &rSer) const
{
	int iSize = 0;
	iSize += rSer.WriteInt32(VistaType::uint32(m_sApplicationName.size()));
	iSize += rSer.WriteString(m_sApplicationName);

	iSize += rSer.WriteInt32(VistaType::uint32(m_sProgressLabel.size()));
	iSize += rSer.WriteString(m_sProgressLabel);

	iSize += rSer.WriteInt32(VistaType::uint32(m_sSubtaskLabel.size()));
	iSize += rSer.WriteString(m_sSubtaskLabel);

	iSize += rSer.WriteInt32(VistaType::uint32(m_sPictureName.size()));
	iSize += rSer.WriteString(m_sPictureName);

	iSize += rSer.WriteInt32(m_iTotalProgress);
	iSize += rSer.WriteInt32(m_iSubtaskProgress);
	iSize += rSer.WriteInt32(m_iIconStateMax);
	
	iSize += rSer.WriteBool(m_bHideFlag);

	return iSize;
}

int VistaProgressMessage::DeSerialize(IVistaDeSerializer &rDeSer)
{
	int iSizeTmp=0;
	int iSize = 0;
	iSize += rDeSer.ReadInt32(iSizeTmp);
	if(iSizeTmp)
		iSize += rDeSer.ReadString(m_sApplicationName, iSizeTmp);
	else
		m_sApplicationName = "";

	iSize += rDeSer.ReadInt32(iSizeTmp);
	if(iSizeTmp)
		iSize += rDeSer.ReadString(m_sProgressLabel, iSizeTmp);
	else
		m_sProgressLabel = "";

	iSize += rDeSer.ReadInt32(iSizeTmp);
	if(iSizeTmp)
		iSize += rDeSer.ReadString(m_sSubtaskLabel, iSizeTmp);
	else
		m_sSubtaskLabel = "";

	iSize += rDeSer.ReadInt32(iSizeTmp);
	if(iSizeTmp)
		iSize += rDeSer.ReadString(m_sPictureName, iSizeTmp);
	else
		m_sPictureName = "";

	iSize += rDeSer.ReadInt32(m_iTotalProgress);
	iSize += rDeSer.ReadInt32(m_iSubtaskProgress);
	iSize += rDeSer.ReadInt32(m_iIconStateMax);

	iSize += rDeSer.ReadBool(m_bHideFlag);
	return iSize;
}

string VistaProgressMessage::GetSignature() const
{
	return "VistaProgressMessage";
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


