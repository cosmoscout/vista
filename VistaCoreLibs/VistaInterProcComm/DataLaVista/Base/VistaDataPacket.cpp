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



/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

#include "VistaDataPacket.h"
#include "VistaTimeStamp.h"


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/


IDLVistaDataPacket::IDLVistaDataPacket(IDLVistaPipeComponent *pProducer)
{
	m_bIsLocked = false;
	m_pPacketSource = pProducer;
	m_pTimeStamp = new DLVistaTimeStamp;
	m_iPacketType = -1; // this does stand for: "not initialized", in contrast to "invalid"
	m_bIsValid = false;

}


IDLVistaDataPacket::~IDLVistaDataPacket()
{
	delete  m_pTimeStamp;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


bool IDLVistaDataPacket::Unlock()
{
	m_bIsLocked = false;
	return true;
}

void IDLVistaDataPacket::Lock()
{
	m_bIsLocked = true;
}

bool IDLVistaDataPacket::IsLocked() const 
{
	return m_bIsLocked;
}

bool IDLVistaDataPacket::IsValid() const 
{
	return m_bIsValid;
}

void IDLVistaDataPacket::SetIsValid(bool bValid)
{
	m_bIsValid = bValid;
}


IDLVistaDataPacket::IDLVistaDataPacket(const IDLVistaDataPacket &)
{
}

IDLVistaDataPacket * IDLVistaDataPacket::Clone() const 
{
	return 0;
}

int IDLVistaDataPacket::GetPacketType() const 
{
	return m_iPacketType;
}


void IDLVistaDataPacket::SetPacketType(int iPacketType)
{
	m_iPacketType = iPacketType;
}

void IDLVistaDataPacket::Stamp(DLV_INT64 i64MicroStamp, DLV_INT32 i32MacroStamp)
{
	(*m_pTimeStamp).Stamp(i64MicroStamp,i32MacroStamp);
}

IDLVistaPipeComponent * IDLVistaDataPacket::GetDataSource() const 
{
	return m_pPacketSource;
}

DLVistaTimeStamp *IDLVistaDataPacket::GetTimeStamp() const 
{
	return m_pTimeStamp;
}
