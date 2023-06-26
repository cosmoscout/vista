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

#ifndef _VISTAOPENSGNODEBRIDGE_H
#define _VISTAOPENSGNODEBRIDGE_H

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/

#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/GraphicsManager/VistaNodeBridge.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include <VistaMath/VistaBoundingBox.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGDrawActionBase.h>
#include <OpenSG/OSGMatrix.h>
#include <OpenSG/OSGTransform.h>

#ifdef WIN32
#pragma warning(pop)
#endif

/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaOpenSGNodeData : public IVistaNodeData {
  friend class VistaOpenSGSG;
  friend class VistaOpenSGNodeBridge;

 public:
  osg::NodePtr GetNode() const;
  bool         SetNode(const osg::NodePtr& node);

  virtual osg::NodeCorePtr GetCore() const;
  virtual bool             SetCore(const osg::NodeCorePtr& core);

 protected:
  VistaOpenSGNodeData();
  virtual ~VistaOpenSGNodeData();

  osg::NodeRefPtr m_ptrNode;
};

class VISTAKERNELAPI VistaOpenSGGeomNodeData : public VistaOpenSGNodeData {
  friend class VistaOpenSGNodeBridge;

 public:
  virtual bool             SetCore(const osg::NodeCorePtr& core);
  virtual osg::NodeCorePtr GetCore() const;

 protected:
  VistaOpenSGGeomNodeData();
  virtual ~VistaOpenSGGeomNodeData();
};

class VISTAKERNELAPI VistaOpenSGExtensionNodeData : public VistaOpenSGNodeData {
  friend class VistaOpenSGNodeBridge; // for destructor access
 public:
 protected:
  VistaOpenSGExtensionNodeData();
  virtual ~VistaOpenSGExtensionNodeData();
};

/**
 * This class is needed for the intensity functionality.
 * for each light node data, we will store the original
 * values as given by the user when creating or manipulating
 * the light node's properties.
 * These values will be scaled by the factor given in
 * SetIntensity() for a given light node.
 * In case you want to query the original value, use this
 * structure, in case you want to query the "current"
 * value use the methods of the node bridge, as these
 * will ask the OSG content for their values.
 */
class VISTAKERNELAPI VistaOpenSGLightNodeData : public VistaOpenSGNodeData {
  // lights are split into light node and beacon in OpenSG
  // the VistaLightNode represents the beacon part of it.
  // The VistaOpenSGLightNodeData actually handles two OpenSG nodes!

  friend class VistaOpenSGNodeBridge;

 public:
  osg::NodePtr GetLightNode() const {
    return m_ptrLightNode;
  }

  osg::NodeCorePtr GetLightCore() const {
    if (m_ptrLightNode != osg::NullFC)
      return m_ptrLightNode->getCore();
    else
      return osg::NullFC;
  }

 private:
  osg::Color4f    m_cAmbient, m_cDiffuse, m_cSpecular;
  osg::Real32     m_fIntensity;
  osg::Real32     m_fShadowIntensity;
  osg::NodeRefPtr m_ptrLightNode;
  int             m_nLightType;

  VistaOpenSGLightNodeData()
      : m_fIntensity(1.0f)
      , m_fShadowIntensity(1.0f)
      , m_ptrLightNode(osg::NullFC)
      , m_nLightType(-1) {
  }

  virtual ~VistaOpenSGLightNodeData() {
  }
};

class VISTAKERNELAPI VistaOpenSGOpenGLNodeData : public VistaOpenSGNodeData {
  friend class VistaOpenSGSG;
  friend class VistaOpenSGNodeBridge;

 public:
  virtual ~VistaOpenSGOpenGLNodeData() {
  }

 private:
  VistaOpenSGOpenGLNodeData() {
  }
};

// class VISTAKERNELAPI VistaOpenSGTextNodeDraw : public IVistaOpenGLDraw
//{
//	friend class VistaOpenSGSG;
//	friend class VistaOpenSGNodeBridge;
// public:
//	VistaOpenSGTextNodeDraw();
//	virtual ~VistaOpenSGTextNodeDraw();
//
//	virtual bool Do ();
//	virtual bool GetBoundingBox( VistaBoundingBox &bb ) const;
//
// private:
//	IVista3DText *m_pText;
//};

class VISTAKERNELAPI VistaOpenSGTextNodeData : public VistaOpenSGNodeData {
 public:
  VistaOpenSGTextNodeData(IVista3DText* pText)
      : m_pText(pText) {
  }

  virtual ~VistaOpenSGTextNodeData() {
  }

  IVista3DText* GetVistaText() const {
    return m_pText;
  }

 protected:
  IVista3DText* m_pText;

 private:
};

class VISTAKERNELAPI VistaOpenSGNodeBridge : public IVistaNodeBridge {
 public:
  // Constructor/Destructor
  VistaOpenSGNodeBridge();
  virtual ~VistaOpenSGNodeBridge();

  //###################################################
  // local utility methods
  //###################################################
  osg::TransformPtr GetTransformCore(const IVistaNodeData*) const;
  bool              GetAmbientLightState() const;

  //###################################################
  // NodeBridge API Implementation
  //###################################################

  /*******************************************/
  /* Node Construction                       */
  /*******************************************/
  virtual bool GetName(std::string& sName, const IVistaNodeData* pData) const;
  virtual bool SetName(const std::string& sName, IVistaNodeData* pData);
  virtual bool GetIsEnabled(const IVistaNodeData* pData) const;
  virtual void SetIsEnabled(bool bEnabled, IVistaNodeData* pData);

  virtual bool GetBoundingBox(
      VistaVector3D& v3Min, VistaVector3D& v3Max, const IVistaNodeData* pData) const;

  /*******************************************/
  /* Node Construction                       */
  /*******************************************/

  virtual IVistaNodeData* NewRootNodeData();
  virtual IVistaNodeData* NewGroupNodeData();
  virtual IVistaNodeData* NewSwitchNodeData();
  virtual IVistaNodeData* NewLODNodeData();
  virtual IVistaNodeData* NewGeomNodeData(IVistaGeometryData* pData);

  virtual IVistaNodeData* NewLightNodeData(VISTA_LIGHTTYPE nLightType);

  virtual IVistaNodeData* NewOpenGLNodeData();
  virtual IVistaNodeData* NewExtensionNodeData();
  virtual IVistaNodeData* NewTransformNodeData();

  virtual IVistaNodeData* NewTextNodeData(IVista3DText* pText);

  virtual void DeleteNode(IVistaNode* pNode);

  /*******************************************/
  /* Node Transformation                     */
  /*******************************************/

  virtual bool GetTranslation(VistaVector3D& v3Trans, const IVistaNodeData* pData) const;
  virtual bool SetTranslation(const VistaVector3D& v3Trans, IVistaNodeData* pData);
  virtual bool GetRotation(VistaTransformMatrix& matRotation, const IVistaNodeData* pData) const;
  virtual bool SetRotation(const VistaTransformMatrix& matRotation, IVistaNodeData* pData);
  virtual bool GetRotation(VistaQuaternion& qRotation, const IVistaNodeData* pData) const;
  virtual bool SetRotation(const VistaQuaternion& qRotation, IVistaNodeData* pData);
  virtual bool GetTransform(VistaTransformMatrix& matTrans, const IVistaNodeData* pData) const;
  virtual bool SetTransform(const VistaTransformMatrix& matTrans, IVistaNodeData* pData);

  virtual bool Rotate(float nEulerX, float nEulerY, float nEulerZ, IVistaNodeData* pData);
  virtual bool Rotate(const VistaQuaternion& qRotation, IVistaNodeData* pData);
  virtual bool Translate(const VistaVector3D& v3Trans, IVistaNodeData* pData);

  virtual bool GetWorldTransform(
      VistaTransformMatrix& matTransform, const IVistaNodeData* pData) const;
  virtual bool GetWorldPosition(VistaVector3D& v3Position, const IVistaNodeData* pData) const;
  virtual bool GetWorldOrientation(
      VistaQuaternion& qOrientation, const IVistaNodeData* pData) const;

  /*******************************************/
  /* Node Hioerarchy                         */
  /*******************************************/

  virtual bool CanAddChild(IVistaNodeData* pChildData, const IVistaNodeData* pData) const;
  virtual bool AddChild(IVistaNodeData* pChildData, IVistaNodeData* pData);
  virtual bool InsertChild(IVistaNodeData* pChildData, int nIndex, IVistaNodeData* pData);
  virtual bool DisconnectChild(int nChildindex, IVistaNodeData* pData);

  /*******************************************/
  /* SwitchNodes                             */
  /*******************************************/

  virtual int  GetActiveChild(const IVistaNodeData* pData) const;
  virtual bool SetActiveChild(int nChildIndex, IVistaNodeData* pData);

  /*******************************************/
  /* LOD Nodes                               */
  /*******************************************/

  virtual bool GetRange(std::vector<float>& vecRanges, const IVistaNodeData* pData) const;
  virtual bool SetRange(const std::vector<float>& vecRanges, IVistaNodeData* pData);
  virtual bool GetCenter(VistaVector3D& v3Center, const IVistaNodeData* pData) const;
  virtual bool SetCenter(const VistaVector3D& v3Center, IVistaNodeData* pData);

  /*******************************************/
  /* Light Nodes                             */
  /*******************************************/
  virtual bool GetLightAmbientColor(VISTA_LIGHTTYPE nType, float& nRed, float& nGreen, float& nBlue,
      const IVistaNodeData* pData) const;
  virtual bool SetLightAmbientColor(
      VISTA_LIGHTTYPE nType, float nRed, float nGreen, float nBlue, IVistaNodeData* pData);
  virtual bool GetLightDiffuseColor(VISTA_LIGHTTYPE nType, float& nRed, float& nGreen, float& nBlue,
      const IVistaNodeData* pData) const;
  virtual bool SetLightDiffuseColor(
      VISTA_LIGHTTYPE nType, float nRed, float nGreen, float nBlue, IVistaNodeData* pData);
  virtual bool GetLightSpecularColor(VISTA_LIGHTTYPE nType, float& nRed, float& nGreen,
      float& nBlue, const IVistaNodeData* pData) const;
  virtual bool SetLightSpecularColor(
      VISTA_LIGHTTYPE nType, float nRed, float nGreen, float nBlue, IVistaNodeData* pData);
  virtual bool SetLightColor(
      VISTA_LIGHTTYPE nType, float nRed, float nGreen, float nBlue, IVistaNodeData* pData);

  virtual bool GetLightAttenuation(
      VISTA_LIGHTTYPE nType, VistaVector3D& v3Attenuation, const IVistaNodeData* pData) const;
  virtual bool SetLightAttenuation(
      VISTA_LIGHTTYPE nType, const VistaVector3D& v3Attenuation, IVistaNodeData* pData);

  virtual bool GetLightPosition(
      VISTA_LIGHTTYPE nType, VistaVector3D&, const IVistaNodeData* pData) const;
  virtual bool SetLightPosition(
      VISTA_LIGHTTYPE nType, const VistaVector3D& v3Position, IVistaNodeData* pData);
  virtual bool GetLightDirection(
      VISTA_LIGHTTYPE nType, VistaVector3D&, const IVistaNodeData* pData) const;
  virtual bool SetLightDirection(
      VISTA_LIGHTTYPE nType, const VistaVector3D& v3Driection, IVistaNodeData* pData);

  virtual float GetLightIntensity(const IVistaNodeData* pData) const;
  virtual bool  SetLightIntensity(float fIntensity, IVistaNodeData* pData);

  virtual float GetSpotCharacter(const IVistaNodeData* pData) const;
  virtual bool  SetSpotCharacter(float fCharacter, IVistaNodeData* pData);

  virtual int  GetSpotDistribution(const IVistaNodeData* pData) const;
  virtual bool SetSpotDistribution(int nExponent, IVistaNodeData* pData);

  virtual bool GetLightIsEnabled(const IVistaNodeData* pData) const;
  virtual void SetLightIsEnabled(bool bEnabled, IVistaNodeData* pData);

  virtual const std::vector<VistaLightNode*>& GetAllLightNodes() const;
  virtual void                                RegisterLightNode(VistaLightNode* pLight);
  virtual void                                UnregisterLightNode(VistaLightNode* pLight);

  /*******************************************/
  /* 3D Text Nodes                           */
  /*******************************************/
  virtual IVista3DText* CreateFontRepresentation(const std::string& sFontName);
  virtual bool          DestroyFontRepresentation(IVista3DText* pText);

  /*******************************************/
  /* Extension/OpenGL Nodes                  */
  /*******************************************/

  virtual bool InitExtensionNode(VistaExtensionNode* pNode, IVistaNodeData* pData);
  virtual bool InitOpenGLNode(IVistaNodeData* pData, VistaOpenGLNode*);

  /*******************************************/
  /* Geometry Nodes                          */
  /*******************************************/

  virtual bool SetGeometry(IVistaGeometryData* pGeomData, IVistaNodeData* pNodeData);
  virtual void ClearGeomNodeData(VistaGeomNode* pNodeData) const;

  /*******************************************/
  /* Node Loading/Saving/Optimization        */
  /*******************************************/

  virtual IVistaNode* LoadNode(const std::string& strFileName, float     fScale = 1.0f,
      VistaSceneGraph::eOptFlags = VistaSceneGraph::OPT_NONE, const bool bVerbose = false);

  virtual bool SaveNode(const std::string& sFileName, IVistaNode* pNode);

  virtual bool ApplyOptimizationToNode(
      IVistaNode* pNode, VistaSceneGraph::eOptFlags eFlags, const bool bVerbose = false);

  virtual IVistaNode* CloneSubtree(IVistaNodeData* pNodeData);

 private:
  osg::Color4f                 m_afAmbientLight;
  bool                         m_bAmbientLightState;
  std::vector<VistaLightNode*> m_vecLightNodes;
};

/*============================================================================*/
/* INLINES		                                                              */
/*============================================================================*/

#endif
