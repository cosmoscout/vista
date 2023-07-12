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

#include "VistaKernel/GraphicsManager/VistaSwitchNode.h"
#include "VistaKernel/GraphicsManager/VistaNodeBridge.h"
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaSwitchNode::VistaSwitchNode() {
  m_nType = VISTA_SWITCHNODE;
}

VistaSwitchNode::VistaSwitchNode(
    VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData, std::string strName)
    : VistaGroupNode(pParent, pBridge, pData, strName) {
  m_nType = VISTA_SWITCHNODE;
}

VistaSwitchNode::~VistaSwitchNode() {
}

// ============================================================================
// ============================================================================
bool VistaSwitchNode::SetActiveChild(int i) {
  // if(i>=0 && i<(int)m_vecChildren.size() || i == -1)
  return m_pBridge->SetActiveChild(i, m_pData);
  // return false;
}
// ============================================================================
// ============================================================================
int VistaSwitchNode::GetActiveChild() const {
  return m_pBridge->GetActiveChild(m_pData);
}
// ============================================================================
// ============================================================================
/*
void VistaSwitchNode::Debug(std::ostream& out, int nLevel)
{
        VistaGroupNode::Debug(out,nLevel);
        for(int i=0; i<nLevel; i++)
                out << "\t";
        out << "|  Currently active child: " <<  GetActiveChild() << "" << std::endl;
}
*/
// ============================================================================
// ============================================================================
