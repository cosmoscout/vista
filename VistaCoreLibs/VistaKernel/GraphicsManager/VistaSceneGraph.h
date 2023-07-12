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

#ifndef _VISTASCENEGRAPH_H
#define _VISTASCENEGRAPH_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaNodeInterface.h>
#include <VistaKernel/VistaKernelConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGraphicsManager;
class VistaOpenGLNode;
class IVistaOpenGLDraw;
class VistaNode;
class IVistaNode;
class VistaGroupNode;
class VistaSwitchNode;
class VistaLODNode;
class VistaLeafNode;
class VistaTextNode;
class VistaGeomNode;
class VistaLightNode;
class VistaTransformNode;
class VistaAmbientLight;
class VistaDirectionalLight;
class VistaPointLight;
class VistaSpotLight;
class IVistaNodeBridge;
class IVistaNodeData;
class IVistaGraphicsBridge;
class IVistaGeometryData;
class VistaVertex;
class VistaPropertyList;

class VistaExtensionNode;
class IVistaExplicitCallbackInterface;
class IVista3DText;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaSceneGraph {
  friend class VistaGraphicsManager;
  friend class IVistaNodeBridge;

 public:
  VistaSceneGraph();
  virtual ~VistaSceneGraph();

  //** GENERATOR ROUTINES
  // routines return pointer to specified node or NULL iff something went wrong
  /** Generate a new OpenGL node
   * @param VistaGroupNode* pParent : Pointer to node's parent (NULL if node is intended to be
   * orphan)
   * @param IVistaOpenGLDraw* pDI : Pointer to draw interface containing the OpenGL calls that will
   * be executed upon rendering
   * @return VistaOpenGLNode* : Pointer to newly created node
   */
  VistaOpenGLNode* NewOpenGLNode(VistaGroupNode*, IVistaOpenGLDraw*);

  /** Generate a new Extension node
   * @param VistaGroupNode* pParent : Pointer to node's parent (NULL if node is intended to be
   * orphan)
   * @param IVistaExplicitCallbackInterface* pDI : Pointer to callback interface that will be
   * executed upon rendering
   * @return VistaExtensionNode* : Pointer to newly created node
   */
  VistaExtensionNode* NewExtensionNode(
      VistaGroupNode* pParent, IVistaExplicitCallbackInterface* pExt);
  /** Generate a new group node
   * @param VistaGroupNode* pParent : Pointer to node's parent (NULL if node is intended to be
   * orphan)
   * @return VistaGroupNode* : Pointer to newly created node
   */
  VistaGroupNode* NewGroupNode(VistaGroupNode* pParent);
  /** Generate a new switch node
   * @param VistaGroupNode* pParent : Pointer to node's parent (NULL if node is intended to be
   * orphan)
   * @return VistaSwitchNode* : Pointer to newly created node
   */
  VistaSwitchNode* NewSwitchNode(VistaGroupNode* pParent);
  /** Generate a new LOD node
   * @param VistaGroupNode* pParent : Pointer to node's parent (NULL if node is intended to be
   * orphan)
   * @return VistaLODNode* : Pointer to newly created node
   */
  VistaLODNode* NewLODNode(VistaGroupNode* pParent);
  /** Generate a new geometry node
   * @param VistaGroupNode* pParent : Pointer to node's parent (NULL if node is intended to be
   * orphan)
   * @param VistaGeometry* pGeom : Pointer to geometry that will be attached to new node
   * @return VistaGeomNode* : Pointer to newly created node
   */
  VistaGeomNode* NewGeomNode(VistaGroupNode* pParent, VistaGeometry* pGeom);

  VistaLightNode*    NewLightFromProplist(const VistaPropertyList& oProps, VistaGroupNode* pParent);
  VistaAmbientLight* NewAmbientLight(VistaGroupNode* pParent);
  VistaDirectionalLight* NewDirectionalLight(VistaGroupNode* pParent);
  VistaPointLight*       NewPointLight(VistaGroupNode* pParent);
  VistaSpotLight*        NewSpotLight(VistaGroupNode* pParent);

  VistaTextNode* NewTextNode(VistaGroupNode* pParent, const std::string& sFontDesc = "Helvetica",
      IVista3DText* pTextI = NULL);
  VistaTransformNode* NewTransformNode(VistaGroupNode* pParent);

  int          AddMaterial(const VistaMaterial& Material);
  int          GetNumberOfMaterials(void);
  virtual bool GetMaterialByIndex(int iIndex, VistaMaterial& oIn) const;
  virtual bool GetMaterialByName(const std::string& sName, VistaMaterial& oIn) const;

  /** Generate a new geometry object
   * @return VistaGeometry* : Pointer to newly created geometry object
   */
  //! Deprecated version for creating an indexed geometry, as vectors of VistaVector3Ds are used.
  VistaGeometry* NewIndexedGeometry(const std::vector<VistaIndexedVertex>& vertices,
      const std::vector<VistaVector3D>& coords, const std::vector<VistaVector3D>& textureCoords,
      const std::vector<VistaVector3D>& normals, const std::vector<VistaColor>& colors,
      const VistaVertexFormat& vFormat = VistaVertexFormat(),
      const VistaGeometry::FaceType    = VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);

  /**
   *
   * @param coords - vector with x,y,z value for the textures length%3 should be 0
   * @param textureCoords - vector with u,v values for the textures length%2 should be 0
   * @param normals - vector with x,y,z value for the textures length%3 should be 0
   *
   */
  VistaGeometry* NewIndexedGeometry(const std::vector<VistaIndexedVertex>& vertices,
      const std::vector<float>& coords, const std::vector<float>& textureCoords,
      const std::vector<float>& normals, const std::vector<VistaColor>& colors,
      const VistaVertexFormat& vFormat = VistaVertexFormat(),
      const VistaGeometry::FaceType    = VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);

  //*****
  //** DESTRUCTION ROUTINES
  /** Delete pLeafNode and its API specific structure
   * @param VistaLeafNode* pLeafNode : Pointer to node to be deleted
   * @return bool true/false
   */
  bool DeleteLeaf(VistaLeafNode* pLeafnode);
  /** Delete pGroupNode and connect all its children to its parent
   * @param VistaGroupNode* pGroupNode : Pointer to node to be deleted
   * @return bool true/false
   */
  bool DeleteGroupNode(VistaGroupNode* pGroupNode, bool bMoveChildren = true);
  /** Delete the subtree beginning with pGroupNode including all API specific structures
   * @param VistaGroupNode* pGroupNode : Pointer to group node to be deleted
   * @return bool true/false
   */
  bool DeleteSubTree(VistaGroupNode* pGroupnode);
  /** Delete the geometry object referenced by pGeom and API specific structures
   * @param VistaGeometry* pGeom : Pointer to geometry to be deleted
   * @return bool true/false
   */
  bool DeleteGeometry(VistaGeometry* pGeom);
  /** Delete the whole SG including all API specific structures (i.e. DeleteSubTree(m_pRoot))
   * @return bool true/false
   */
  bool Delete();

  //** MEMBER ACCESS
  /** Returns the real root node of the SG
   * @return VistaGroupNode* : Pointer to scene graph's real root node
   */
  virtual VistaGroupNode* GetRealRoot() const;
  /** Returns the root node of the SG
   * @return VistaGroupNode* : Pointer to scene graph's root node
   */
  virtual VistaGroupNode* GetRoot() const;

  /** find node specified with the given name (if names aren't unique, the first match is returned)
   * @param string strNodeName : Name of node to be found
   * @param IVistaNode*: Node to start the search from
   * @return IVistaNode* : Pointer to node if found / else NULL
   */
  virtual IVistaNode* GetNode(const std::string& strNodeName, IVistaNode* pSubRoot = NULL) const;

  enum eOptFlags {
    OPT_NONE = 0,

    OPT_GEOMETRY_LOW  = 1,
    OPT_GEOMETRY_MID  = 2,
    OPT_GEOMETRY_HIGH = 4,

    OPT_MEMORY_LOW  = 8,
    OPT_MEMORY_HIGH = 16,

    OPT_CULLING = 32,

    OPT_BEST_EFFORT = 256
  };

  //*****
  //** I/O STUFF
  /** Load a subtree from a hierarchically structured file
   * @param string strFileName : Name of file to be loaded
   * @return VistaGroupNode* : Pointer to top level node of read structure
   */
  virtual VistaGroupNode* LoadSubTree(const std::string& strFileName, eOptFlags eOpts = OPT_NONE,
      float fScale = 1.0f, bool bVerbose = false);
  /** Save the subtree beginning with pNode to a VRML-file named strFileName.wrl
   * @param strinf strFileName : Name of output file
   * @return bool true/false
   */
  virtual bool SaveSubTree(std::string strFileName, IVistaNode* pNode);
  /** Load a single geometry described in a file
   * @param string strFileName : Name of file to be loaded
   * @return VistaGeomNode* : Pointer to geometry node to which read geometry has been attached
   */
  virtual VistaGeomNode* LoadGeomNode(const std::string& strFileName, eOptFlags eOpts = OPT_NONE,
      float fScale = 1.0f, bool bVerbose = false);

  /** load a subtree from a hierarchically structured file; in contrast to LoadSubTree,
   * the resulting node is not casted; use this method if you do not know what type
   * of geometry structure is inside your file
   * @param string sFileName : name of file to be loaded
   * @return VistaGeomNode* : pointer to a ViSTA-node with the read geometry
   */
  virtual IVistaNode* LoadNode(const std::string& sFileName, eOptFlags eOpts = OPT_NONE,
      float fScale = 1.0f, bool bVerbose = false);

  bool ApplyOptimizationToNode(IVistaNode* pNode, eOptFlags eFlags, bool bVerbose = false);
  //*****

  //** MISC
  virtual void Debug(std::ostream& out) const;
  //*****

  IVistaNodeBridge*     GetNodeBridge() const;
  IVistaGraphicsBridge* GetGraphicsBridge() const;

  /**
   * Clones the subtree starting from pNode, and appends it as child to pNewParent.
   * Keep in mind that all specific implementation references - like geometries,
   * Callbacks... - are not copied, but only referenced by the new node
   * @return the copied version of pNode (i.e. local root of the new subtree)
   */
  IVistaNode* CloneSubtree(IVistaNode* pNode, VistaGroupNode* pNewParent = NULL);

  /**
   * Searchs all nodes of type wantedNodeType in the subtree starting from pNode and pushes them
   * into vecNodesOfWantedType.
   */
  static void GetAllSubNodesOfType(std::vector<IVistaNode*>& vecNodesOfWantedType,
      const VISTA_NODETYPE& eWantedNodeType, IVistaNode* pNode);
  /**
   * Searchs all nodes of type wantedNodeType in the subtree starting from the scenegraph root and
   * pushes them into vecNodesOfWantedType.
   */
  void GetAllSubNodesOfType(
      std::vector<IVistaNode*>& vecNodesOfWantedType, const VISTA_NODETYPE& eWantedNodeType) const;

  const std::vector<VistaLightNode*>& GetAllLightNodes() const;
  void            GetAllLightNodes(std::vector<VistaLightNode*>& vecLights) const;
  VistaLightNode* GetLightNode(const int nIndex) const;
  int             GetNumberOfLights() const;
  int             GetNumberOfActiveLights() const;

 protected:
  // NEW: NodeBridge and GraphicsBridge are concepts of the new VistaGraphicsManager
  virtual bool Init(IVistaNodeBridge*, IVistaGraphicsBridge*);

  // construct geometry from given geom data. protected, because user shouldn't see the 'data
  // concept'
  VistaGeometry* NewGeometry(IVistaGeometryData* pData);

  /**
   * @todo make root nodes group nodes!?
   */
  VistaTransformNode*   m_pRoot;
  VistaTransformNode*   m_pModelRoot;
  IVistaNodeBridge*     m_pNodeBridge;
  IVistaGraphicsBridge* m_pGraphicsBridge;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
VISTAKERNELAPI std::ostream& operator<<(std::ostream& out, const VistaSceneGraph& device);

inline IVistaNodeBridge* VistaSceneGraph::GetNodeBridge() const {
  return m_pNodeBridge;
}
inline IVistaGraphicsBridge* VistaSceneGraph::GetGraphicsBridge() const {
  return m_pGraphicsBridge;
}
inline VistaGeometry* VistaSceneGraph::NewGeometry(IVistaGeometryData* pData) {
  return new VistaGeometry(m_pGraphicsBridge, pData);
}
#endif //_VISTASCENEGRAPH_H
