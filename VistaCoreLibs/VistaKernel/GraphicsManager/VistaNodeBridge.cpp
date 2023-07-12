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

#include "VistaNodeBridge.h"
#include "VistaExtensionNode.h"
#include "VistaGeomNode.h"
#include "VistaGroupNode.h"
#include "VistaLODNode.h"
#include "VistaOpenGLNode.h"
#include "VistaSwitchNode.h"
#include "VistaTransformNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaNodeData::~IVistaNodeData() {
}

IVistaNodeBridge::IVistaNodeBridge() {
  m_pSceneGraph = NULL;
}

IVistaNodeBridge::~IVistaNodeBridge() {
  //// dump georefs (should be empty now if all went ok)
  // REFMAP::iterator it = m_mpGeomRefs.begin();
  // while(it != m_mpGeomRefs.end())
  //{
  //	std::cerr << it->first << ": " << it->second << std::endl;
  //	++it;
  //}
  // std::cerr << "DONE" << std::endl;
}

VistaSceneGraph* IVistaNodeBridge::GetVistaSceneGraph() const {
  return m_pSceneGraph;
}

bool IVistaNodeBridge::Init(VistaSceneGraph* pSG) {
  if (m_pSceneGraph)
    return false;

  m_pSceneGraph = pSG;
  return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int IVistaNodeBridge::RegisterGeometry(VistaGeometry* pGeom) {
  GeomRefMap::iterator it = m_mpGeomRefs.find(pGeom);

  if (it != m_mpGeomRefs.end()) {
    // ok, we have some
    (*it).second = (*it).second + 1; // increase counter
    return (*it).second;
  }

  m_mpGeomRefs.insert(GeomRefMap::value_type(pGeom, 1)); // register
  return 1;
}

bool IVistaNodeBridge::UnregisterGeometry(VistaGeometry* pGeom) {
  GeomRefMap::iterator it = m_mpGeomRefs.find(pGeom);

  if (it != m_mpGeomRefs.end()) {
    // ok, we have some
    if (it->second - 1 == 0) {
      // we were the last to reference this one...kick it...
      // calls ObservableDelete (should, at least)
      delete pGeom;
      // remove from reference-counting list
      m_mpGeomRefs.erase(it);
      return true;
    } else {
      // decrease refcount
      --(it->second);
    }
  }
  return false;
}

#if 0
bool IVistaNodeBridge::UnregisterGeometry(VistaGeometry *pGeom, VistaGeomNode *pNode)
{
	/**
	 * @todo this one looks strange...why do we get a node and a geometry here? what happens if the node is NOT containing the given geometry!!??
 	 */
	assert (pNode->GetGeometry() == pGeom);

	REFMAP::iterator it = m_mpGeomRefs.find(pGeom);
	
	if(it != m_mpGeomRefs.end())
	{
		// ok, we have some
		if(it->second-1 == 0) // ok, we have deleted it
		{
			// remove reference to content
			// as pGeom might persist after this call
			if(pNode)
				CleanContentNodeData(pNode->GetData());
			delete it->first;
			m_mpGeomRefs.erase(it);
			return true;
		}
		else
			it->second = it->second-1;
	}
	return false;
}
#endif

VistaNode* IVistaNodeBridge::NewNode(
    VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName) {
  return new VistaNode(pParent, this, pData, strName);
}

// ============================================================================

VistaGroupNode* IVistaNodeBridge::NewGroupNode(
    VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName) {
  return new VistaGroupNode(pParent, this, pData, strName);
}
// ============================================================================
// ============================================================================
VistaSwitchNode* IVistaNodeBridge::NewSwitchNode(
    VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName) {
  return new VistaSwitchNode(pParent, this, pData, strName);
}

// ============================================================================
// ============================================================================
VistaLODNode* IVistaNodeBridge::NewLODNode(
    VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName) {
  return new VistaLODNode(pParent, this, pData, strName);
}
// ============================================================================
// ============================================================================
VistaGeomNode* IVistaNodeBridge::NewGeomNode(VistaGroupNode* pParent, VistaGeometry* pGeom,
    IVistaNodeData* pData, const std::string& strName) {
  return new VistaGeomNode(pParent, pGeom, this, pData, strName);
}
// ============================================================================

// ============================================================================
VistaAmbientLight* IVistaNodeBridge::NewAmbientLight(
    VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName) {
  return new VistaAmbientLight(pParent, this, pData, strName);
}
// ============================================================================
// ============================================================================
VistaDirectionalLight* IVistaNodeBridge::NewDirectionalLight(
    VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName) {
  return new VistaDirectionalLight(pParent, this, pData, strName);
}
// ============================================================================
// ============================================================================
VistaPointLight* IVistaNodeBridge::NewPointLight(
    VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName) {
  return new VistaPointLight(pParent, this, pData, strName);
}
// ============================================================================
// ============================================================================
VistaSpotLight* IVistaNodeBridge::NewSpotLight(
    VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName) {
  return new VistaSpotLight(pParent, this, pData, strName);
}
// ============================================================================
// ============================================================================

VistaOpenGLNode* IVistaNodeBridge::NewOpenGLNode(VistaGroupNode* pParent, IVistaOpenGLDraw* pDI,
    IVistaNodeData* pData, const std::string& strName) {
  return new VistaOpenGLNode(pParent, pDI, this, pData, strName);
}

// ============================================================================
// ============================================================================
void IVistaNodeBridge::DeleteNode(IVistaNode* pNode) {
  delete pNode;
}

VistaExtensionNode* IVistaNodeBridge::NewExtensionNode(VistaGroupNode* pParent,
    IVistaExplicitCallbackInterface* pExtension, IVistaNodeData* pData, const std::string& sName) {
  return new VistaExtensionNode(pParent, pExtension, this, pData, sName);
}

VistaTransformNode* IVistaNodeBridge::NewTransformNode(
    VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& sName) {
  return new VistaTransformNode(pParent, this, pData, sName);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
