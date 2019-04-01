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


#include "VistaKernel/GraphicsManager/VistaExtensionNode.h"
#include "VistaKernel/GraphicsManager/VistaNodeBridge.h"
#include <VistaAspects/VistaExplicitCallbackInterface.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaExtensionNode::VistaExtensionNode()
{
	m_nType = VISTA_EXTENSIONNODE;
	m_pExtension = NULL;
}

VistaExtensionNode::VistaExtensionNode(	VistaGroupNode*				pParent, 
											IVistaExplicitCallbackInterface*	pExtension, 
											IVistaNodeBridge*				pBridge,
											IVistaNodeData*					pData,
											const std::string &name)
					:	VistaLeafNode(pParent, pBridge, pData, name),
					m_pExtension(pExtension)
{
	m_nType = VISTA_EXTENSIONNODE;
	m_bValid = false;
}


VistaExtensionNode::~VistaExtensionNode()
{
	m_pExtension = NULL;
}

// ============================================================================
// ============================================================================

bool VistaExtensionNode::Update()
{
	if(m_bValid && m_pExtension)
		return m_pExtension->Do();
	return false;
}

// ============================================================================
// ============================================================================
bool VistaExtensionNode::CanHaveChildren() const
{
	return false;
}

bool VistaExtensionNode::Init()
{
	//init only once 
	if(!m_bValid)
		m_bValid = m_pBridge->InitExtensionNode(this, m_pData);
	return m_bValid;
}

IVistaExplicitCallbackInterface *VistaExtensionNode::GetExtension() const
{
	return m_pExtension;
}

bool VistaExtensionNode::SetExtension(IVistaExplicitCallbackInterface *pExt)
{
	m_pExtension = pExt;
	return true;
}

// ============================================================================
// ============================================================================

