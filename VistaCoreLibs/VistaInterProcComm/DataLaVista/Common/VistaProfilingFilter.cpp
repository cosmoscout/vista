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


#include "VistaProfilingFilter.h"
#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaTimeStamp.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaRTC.h>

#include <cstdio>


/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaProfilingFilter::DLVistaProfilingFilter()
{
	m_i64MicroMean = m_i64MicroMin = m_i64MicroMax = 0;
	m_i32MacroMean = m_i32MacroMin = m_i32MacroMax = 0;
	m_pTimer = IDLVistaRTC::GetRTCSingleton();
	m_i32PacketCount = 0;
}


DLVistaProfilingFilter::~DLVistaProfilingFilter()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


IDLVistaDataPacket *DLVistaProfilingFilter::FilterPacketL(IDLVistaDataPacket *pPacket)
{
	++m_i32PacketCount;

	//DLV_INT64 i64Diff, i64now;
	//DLV_INT32 i32Diff, i32now;

	//i64now = (DLV_INT64)m_pTimer->GetTickCount();
	//i32now = (DLV_INT32)m_pTimer->GetTimeStamp();

	//i64Diff = i64now - pPacket->GetTimeStamp()->GetMicroStamp();
	//i32Diff = i32now - pPacket->GetTimeStamp()->GetMacroStamp();

	//printf("i64Diff=%I64u,i32Diff=%ld\n", i64Diff, i32Diff);

	return pPacket; // any packet will go through
}

IDLVistaDataPacket *DLVistaProfilingFilter::GivePacket(bool)
{
	return NULL;
}

IDLVistaDataPacket *DLVistaProfilingFilter::CreatePacket()
{
	return NULL;
}

void DLVistaProfilingFilter::DeletePacket(IDLVistaDataPacket *)
{
}

bool DLVistaProfilingFilter::InitPacketMgmt()
{
	return true;
}


int DLVistaProfilingFilter::GetInputPacketType() const
{
	if(!m_pOutput) 
		return IDLVistaInPlaceFilter::GetInputPacketType(); // we let the superclass decide
	return m_pOutput->GetInputPacketType();
}

int DLVistaProfilingFilter::GetOutputPacketType() const
{
   if(!m_pInput) 
		return IDLVistaInPlaceFilter::GetOutputPacketType(); // we let the superclass decides

	return m_pInput->GetOutputPacketType();
}


