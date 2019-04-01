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

#include "VistaDataConsumer.h"

#include <cstddef>
#include <list>
#include <cstddef>
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

IDLVistaDataConsumer::IDLVistaDataConsumer()
{
	m_pDataInput = 0;
}


IDLVistaDataConsumer::~IDLVistaDataConsumer()
{
	m_pDataInput = 0;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


bool IDLVistaDataConsumer::AttachOutputComponent(IDLVistaPipeComponent * )
{
	return false;
}

bool IDLVistaDataConsumer::AttachInputComponent(IDLVistaPipeComponent * pIn)
{
	m_pDataInput = pIn;
	return true;
}


bool IDLVistaDataConsumer::DetachInputComponent(IDLVistaPipeComponent *pComp)
{
	if(IsInputComponent(pComp))
	{
		m_pDataInput = 0;
		return true;
	}

	return false;
}

bool IDLVistaDataConsumer::DetachOutputComponent(IDLVistaPipeComponent *pComp)
{
	return false;
}


bool IDLVistaDataConsumer::IsInputComponent(IDLVistaPipeComponent *pComp) const
{
	return (pComp == m_pDataInput);
}

list<IDLVistaPipeComponent *> IDLVistaDataConsumer::GetOutputComponents() const
{
	list<IDLVistaPipeComponent *> list; 
	return list;// return empty list
}

list<IDLVistaPipeComponent *> IDLVistaDataConsumer::GetInputComponents() const
{
	list<IDLVistaPipeComponent *> list;
	if(m_pDataInput)
	{
		list.push_back(m_pDataInput);
	}
	return list;
}

IDLVistaPipeComponent *IDLVistaDataConsumer::GetOutboundByIndex(int iIndex) const
{
	return NULL;
}


IDLVistaPipeComponent *IDLVistaDataConsumer::GetInboundByIndex(int iIndex) const
{
	return (iIndex == 0 ? m_pDataInput : NULL);
}

int IDLVistaDataConsumer::GetNumberOfOutbounds() const
{
	return 0;
}

int IDLVistaDataConsumer::GetNumberOfInbounds() const
{
	return m_pDataInput ? 1 : 0;
}

