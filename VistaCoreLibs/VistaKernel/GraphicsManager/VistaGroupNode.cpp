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

#include "VistaGroupNode.h"
#include "VistaNodeBridge.h"

#include <algorithm>
#include <stack>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaGroupNode::VistaGroupNode() {
  m_nType = VISTA_GROUPNODE;
  // m_nNumChildren = 0;
}

VistaGroupNode::VistaGroupNode(
    VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData, std::string strName)
    : VistaNode(pParent, pBridge, pData, strName) {
  m_nType = VISTA_GROUPNODE;
  // m_nNumChildren = 0;
}

VistaGroupNode::~VistaGroupNode() {
  // cout << "m_vecChildren.size() = " << m_vecChildren.size() << std::endl;
  // as the children might tend to disconnect upon destruction
  // which will alter the parent pointer while we are traversing the child vector.
  /** @todo remove shadow graph!! */
  // cout << "-- ENTER --" << std::endl;
  for (unsigned int i = 0; i < m_vecChildren.size(); ++i) {
    // cout << "deleting: " << i << " @ " << m_vecChildren[i] << std::endl;
    //<< " ("
    //     << m_vecChildren[i]->GetName() << "" << std::endl;
    // m_pBridge->DisconnectChild(i, m_pData);
    static_cast<VistaNode*>(m_vecChildren[i])->m_pParent = NULL;
    delete m_vecChildren[i];
  }
  // cout << "-- LEAVE -- " << std::endl;
}

// ============================================================================
// ============================================================================
IVistaNode* VistaGroupNode::GetChild(unsigned int nChildindex) const {
  // index-check
  if (IsValidIndex(nChildindex))
    return m_vecChildren[nChildindex];
  else
    return NULL;
}
// ============================================================================
// ============================================================================
IVistaNode* VistaGroupNode::GetChild(const std::string& strName) const {
  for (unsigned int i = 0; i < m_vecChildren.size(); ++i)
    if (m_vecChildren[i]->GetName() == strName)
      return m_vecChildren[i];
  // still here, so I guess I haven't found anything
  return NULL;
}
// ============================================================================
// ============================================================================
bool VistaGroupNode::HasChildren() const {
  return !m_vecChildren.empty();
}
// ============================================================================
// ============================================================================
unsigned int VistaGroupNode::GetNumChildren() const {
  return (unsigned int)m_vecChildren.size();
}

bool VistaGroupNode::GetIsChild(const IVistaNode* pNode) const {
  return (std::find(m_vecChildren.begin(), m_vecChildren.end(), pNode) != m_vecChildren.end());
}
// ============================================================================
// ============================================================================
bool VistaGroupNode::CanAddChild(IVistaNode* pChild) const {
  bool                          HasCircle   = false;
  const IVistaNode*             pCurrNode   = NULL;
  VistaNode*                    pVistaChild = static_cast<VistaNode*>(pChild);
  std::stack<const IVistaNode*> NodeStack;
  // depth-first search through the tree connected to <child>
  NodeStack.push(pChild);
  do {
    pCurrNode = NodeStack.top();
    NodeStack.pop();
    // if one of the decessors of <child> is identicall to <this> we will
    // get a circle if we make <child> a child of <this>
    if (pCurrNode == this) {
      HasCircle = true;
      break; // we can leave the loop here
    }
    if (pCurrNode->CanHaveChildren()) {
      // push all children of current node onto the stack
      const VistaGroupNode* pFather = static_cast<const VistaGroupNode*>(pCurrNode);
      int                   num     = pFather->GetNumChildren();
      for (int i = 0; i < num; ++i)
        NodeStack.push(pFather->GetChild(i));
    }
  } while ((!HasCircle) && (!(NodeStack.empty())));
  return (!HasCircle) && m_pBridge->CanAddChild(pVistaChild->GetData(), m_pData);
}
// ============================================================================
// ============================================================================
bool VistaGroupNode::AddChild(IVistaNode* pChild) {
  // cout << "VistaGroupNode::AddChild(" << pChild << ")" << std::endl;

  VistaNode* pVistaChild = dynamic_cast<VistaNode*>(pChild);
  if (pVistaChild && CanAddChild(pChild)) {
    if (pVistaChild->m_pParent)
      pVistaChild->m_pParent->DisconnectChild(pVistaChild); // remove old parent

    if (m_pBridge->AddChild(pVistaChild->GetData(), m_pData)) {
      m_vecChildren.push_back(pChild);
      //++m_nNumChildren;
      // Introduce child to his/her parent
      pVistaChild->m_pParent = this;
      pVistaChild->IncTransScore(); // reparent
      return true;
    }
  }
  return false;
}
// ============================================================================
// ============================================================================
bool VistaGroupNode::InsertChild(IVistaNode* pChild, unsigned int nIndex) {
  VistaNode* pVistaChild = dynamic_cast<VistaNode*>(pChild);
  if (!pVistaChild)
    return false;
  // check for enough memory
  if ((m_vecChildren.size() < m_vecChildren.max_size()) && (CanAddChild(pChild)) &&
      (nIndex <= m_vecChildren.size())) {
    if (m_pBridge->InsertChild(pVistaChild->GetData(), nIndex, m_pData)) {
      if (nIndex < m_vecChildren.size()) {
        // find insertion position
        VistaNodeVectorIter iNodeIter = m_vecChildren.begin() + nIndex;
        m_vecChildren.insert(iNodeIter, pChild);
      } else
        m_vecChildren.push_back(pChild);

      //++m_nNumChildren;
      // Introduce child to his/her parent
      (static_cast<VistaNode*>(pChild))->m_pParent = this;
      (static_cast<VistaNode*>(pChild))->IncTransScore();
      return true;
    }
  }
  return false;
}
// ============================================================================

bool VistaGroupNode::IsValidIndex(unsigned int iIndex) const {
  return (iIndex < m_vecChildren.size());
}

// ============================================================================
IVistaNode* VistaGroupNode::DisconnectChild(unsigned int nChildindex) {
  IVistaNode* pChild = NULL;

  if (IsValidIndex(nChildindex)) {
    if (m_pBridge->DisconnectChild(nChildindex, m_pData)) {
      static_cast<VistaNode*>(m_vecChildren[nChildindex])->m_pParent = NULL;
      static_cast<VistaNode*>(m_vecChildren[nChildindex])->m_nLocalTransScore +=
          GetTransformScore() + 1;

      VistaNodeVector::iterator it = m_vecChildren.begin() + nChildindex;
      pChild                       = *it;
      m_vecChildren.erase(it);
      //--m_nNumChildren;

      /*
      VistaNodeVector NewChildren;

      NewChildren.reserve(m_nNumChildren-1);
      //backup all but child no. i to <NewChildren>
      int j;
      for (j=0; j<m_nNumChildren; j++)
              if(j<nChildindex)
                      NewChildren[j]=m_vecChildren[j];
              else
                      if(j==nChildindex)
                              //remember removed child in <pChild> to make sure not to have any
      dangling pointers pChild = m_vecChildren[j]; else if(j>nChildindex)
                                      NewChildren[j-1]=m_vecChildren[j];

      //resize <m_vecChildren>
      m_nNumChildren--;
      m_vecChildren[m_nNumChildren] = NULL;
      m_vecChildren.resize(m_nNumChildren);

      //copy <NewChildren> to <m_vecChildren>
      //(first i-1 elements are indentical anyway)
      for(j=nChildindex; j<m_nNumChildren; j++)
              m_vecChildren[j] = NewChildren[j];
      //Set parent of disconnected child
      */
    }
  }
  return pChild;
}
// ============================================================================
// ============================================================================
IVistaNode* VistaGroupNode::DisconnectChild(const std::string& strName) {
  int nChildindex = -1;

  // search for child with name <name>
  for (unsigned int i = 0; i < m_vecChildren.size(); i++)
    if (m_vecChildren[i]->GetName() == strName) {
      nChildindex = i;
    }

  return DisconnectChild(nChildindex);
}
// ============================================================================
// ============================================================================
int VistaGroupNode::DisconnectChild(IVistaNode* pChild) {
  int nChildNum = -1;

  if (pChild) {
    // go through children to determine childnumber
    int i = 0;
    for (VistaNodeVector::const_iterator cit = m_vecChildren.begin(); cit != m_vecChildren.end();
         ++cit, ++i) {
      if (*cit == pChild) {
        nChildNum = i;
        break;
      }
    }
    DisconnectChild(nChildNum);
  }
  return nChildNum;
}
// ============================================================================
// ============================================================================
bool VistaGroupNode::CanHaveChildren() const {
  return true;
}
// ============================================================================
// ============================================================================
/*
void VistaGroupNode::Debug(std::ostream& out, int nLevel)
{
        VistaNode::Debug(out,nLevel);
        for(int i=0; i<nLevel; i++)
                out << "\t";
        out << "|  Current ChildCount: " <<  m_nNumChildren << "" << std::endl;
}
*/
// ============================================================================
// ============================================================================
