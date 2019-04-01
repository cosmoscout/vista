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

#include <cstring>
#include <list>
#include <cstddef>

using std::list;

#include <cstddef>

#include "VistaPipe.h"


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

IDLVistaPipe::IDLVistaPipe()
{
	m_pInput  = 0;
	m_pOutput = 0;
}

IDLVistaPipe::~IDLVistaPipe()
{}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool IDLVistaPipe::AttachOutputComponent(IDLVistaPipeComponent *pIn )
{
	m_pOutput  = pIn;
	return true;
}


bool IDLVistaPipe::AttachInputComponent(IDLVistaPipeComponent *pOut)
{
	m_pInput = pOut;
	return true;
}

bool IDLVistaPipe::DetachInputComponent(IDLVistaPipeComponent * pComp)
{
	if(pComp == m_pInput)
	{
		m_pInput = 0;
		return true;
	}
	return false;
}

bool IDLVistaPipe::DetachOutputComponent(IDLVistaPipeComponent * pComp)
{
	if(pComp == m_pOutput)
	{
		m_pOutput = 0;
		return true;
	}
	return false;
}

bool IDLVistaPipe::IsInputComponent(IDLVistaPipeComponent *pComp) const
{
	return (m_pInput == pComp);
}

bool IDLVistaPipe::IsOutputComponent(IDLVistaPipeComponent *pComp) const
{
	return (m_pOutput == pComp);
}


list<IDLVistaPipeComponent *> IDLVistaPipe::GetOutputComponents() const
{
	list<IDLVistaPipeComponent *> list;
	if(m_pOutput)
		list.push_back(m_pOutput);
	return list;
}

list<IDLVistaPipeComponent *> IDLVistaPipe::GetInputComponents() const
{
	list<IDLVistaPipeComponent *> list;
	if(m_pInput)
	{
		list.push_back(m_pInput);
	}
	return list;
}


int IDLVistaPipe::GetInputPacketType() const
{
	if(!m_pOutput) 
		return -1; // we let the superclass decide
	return m_pOutput->GetInputPacketType();
}

int IDLVistaPipe::GetOutputPacketType() const
{
   if(!m_pInput) 
		return -1; // we let the superclass decides

	return m_pInput->GetOutputPacketType();
}

IDLVistaPipeComponent *IDLVistaPipe::GetOutboundByIndex(int iIndex) const 
{
	return (iIndex == 0 ? m_pOutput : NULL);
}

int IDLVistaPipe::GetNumberOfOutbounds() const 
{
	return (m_pInput ? 1 : 0);
}


int IDLVistaPipe::GetNumberOfInbounds() const
{
	return (m_pOutput ? 1 : 0);
}


IDLVistaPipeComponent *IDLVistaPipe::GetInboundByIndex(int iIndex) const 
{
	return (iIndex == 0 ? m_pInput : NULL);
}

