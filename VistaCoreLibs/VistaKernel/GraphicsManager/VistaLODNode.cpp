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

#include "VistaKernel/GraphicsManager/VistaLODNode.h"
#include "VistaKernel/GraphicsManager/VistaNodeBridge.h"
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaLODNode::VistaLODNode() {
  m_nType = VISTA_LODNODE;
}

VistaLODNode::VistaLODNode(
    VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData, std::string strName)
    : VistaGroupNode(pParent, pBridge, pData, strName) {
  m_nType = VISTA_LODNODE;
}

VistaLODNode::~VistaLODNode() {
}

/** Set the the LOD spring values. If there is 3 level of details,
 *   it should be given 2 values
 */
bool VistaLODNode::SetRange(const std::vector<float>& rangeList) {
  return m_pBridge->SetRange(rangeList, m_pData);
}
/** Get the the LOD spring values.
 */
bool VistaLODNode::GetRange(std::vector<float>& rangeList) const {
  return m_pBridge->GetRange(rangeList, m_pData);
}

/**Set the center of the object (for the distance calculation)
 */
bool VistaLODNode::SetCenter(const VistaVector3D& center) {
  return m_pBridge->SetCenter(center, m_pData);
}
/**Get the center of the object
 */
bool VistaLODNode::GetCenter(VistaVector3D& center) const {
  return m_pBridge->GetCenter(center, m_pData);
}
// ============================================================================
// ============================================================================
