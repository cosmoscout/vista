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


#include "VistaKernel/GraphicsManager/VistaTextNode.h"
#include "VistaKernel/GraphicsManager/VistaGeometry.h"
#include "VistaKernel/GraphicsManager/Vista3DText.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaTextNode::VistaTextNode(	VistaGroupNode*			pParent, 
								IVista3DText*				pText, 
								IVistaNodeBridge*			pBridge,
								IVistaNodeData*				pData,
								std::string strName)
				:VistaLeafNode(pParent,pBridge,pData,strName)
{
	m_nType = VISTA_TEXTNODE;
	m_pTextImp = pText;
}


VistaTextNode::~VistaTextNode()
{
	delete m_pTextImp;
}

// ============================================================================
// ============================================================================
bool VistaTextNode::CanHaveChildren() const
{
	return false;
}

IVista3DText *VistaTextNode::GetTextImp() const
{
	return m_pTextImp;
}

void VistaTextNode::SetText(const std::string &sText)
{
	if(m_pTextImp)
		m_pTextImp->SetText(sText);
}

std::string VistaTextNode::GetText() const
{
	if(m_pTextImp)
		return m_pTextImp->GetText();
	return "<no-text-imp-no-fun>";
}

// ============================================================================
// ============================================================================


