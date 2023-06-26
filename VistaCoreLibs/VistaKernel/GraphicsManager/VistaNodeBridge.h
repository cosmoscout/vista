/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright( c )1997-2011 RWTH Aachen University               */
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

#ifndef _VISTANODEBRIDGE_H
#define _VISTANODEBRIDGE_H

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/GraphicsManager/VistaLightNode.h>  // needed for lighttype
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h> // for GeomOpt enum

#include <map>

/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaVector3D;
class VistaQuaternion;
class IVistaOpenGLDraw;
class VistaOpenGLNode;
class VistaTraversalCallbackNode;

class IVista3DText;
class VistaTextNode;

class IVistaGeometryData;
class IVistaNodeData;
class VistaTransformNode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Helper class to provide a proper context for toolkit specific method
 * calls. It only declares a virtual destructor, as it's meaning is
 * completely toolkit specific.
 */
class VISTAKERNELAPI IVistaNodeData {
 public:
  virtual ~IVistaNodeData();
};

class VISTAKERNELAPI IVistaNodeBridge {
 public:
  virtual ~IVistaNodeBridge();

  /*******************************************/
  /* Non-abstract functions                  */
  /*******************************************/
  virtual bool Init(VistaSceneGraph* pSG);

  /** @todo handle this as an observer? -> think about this! */
  virtual int RegisterGeometry(VistaGeometry* pGeom);

  /** unregister geometry  - gets deleted if refcount goes down to zero
   * returns true if pGeom has been deleted, false, if it still is alive
   */
  virtual bool UnregisterGeometry(VistaGeometry* pGeom);

  /*******************************************/
  /* Node Construction                       */
  /*******************************************/
  virtual bool GetName(std::string& sName, const IVistaNodeData* pData) const = 0;
  virtual bool SetName(const std::string& sName, IVistaNodeData* pData)       = 0;
  virtual bool GetIsEnabled(const IVistaNodeData* pData) const                = 0;
  virtual void SetIsEnabled(bool bEnabled, IVistaNodeData* pData)             = 0;

  virtual bool GetBoundingBox(
      VistaVector3D& v3Min, VistaVector3D& v3Max, const IVistaNodeData* pData) const = 0;

  /*******************************************/
  /* Node Construction                       */
  /*******************************************/

  virtual IVistaNodeData* NewRootNodeData()                          = 0;
  virtual IVistaNodeData* NewGroupNodeData()                         = 0;
  virtual IVistaNodeData* NewSwitchNodeData()                        = 0;
  virtual IVistaNodeData* NewLODNodeData()                           = 0;
  virtual IVistaNodeData* NewGeomNodeData(IVistaGeometryData* pData) = 0;

  virtual IVistaNodeData* NewLightNodeData(VISTA_LIGHTTYPE nLightType) = 0;

  virtual IVistaNodeData* NewOpenGLNodeData()    = 0;
  virtual IVistaNodeData* NewExtensionNodeData() = 0;
  virtual IVistaNodeData* NewTransformNodeData() = 0;

  virtual IVistaNodeData* NewTextNodeData(IVista3DText* pText) = 0;

  virtual void DeleteNode(IVistaNode* pNode);

  /*******************************************/
  /* Node Transformation                     */
  /*******************************************/

  virtual bool GetTranslation(VistaVector3D& v3Trans, const IVistaNodeData* pData) const = 0;
  virtual bool SetTranslation(const VistaVector3D& v3Trans, IVistaNodeData* pData)       = 0;
  virtual bool GetRotation(
      VistaTransformMatrix& matRotation, const IVistaNodeData* pData) const                    = 0;
  virtual bool SetRotation(const VistaTransformMatrix& matRotation, IVistaNodeData* pData)     = 0;
  virtual bool GetRotation(VistaQuaternion& qRotation, const IVistaNodeData* pData) const      = 0;
  virtual bool SetRotation(const VistaQuaternion& qRotation, IVistaNodeData* pData)            = 0;
  virtual bool GetTransform(VistaTransformMatrix& matTrans, const IVistaNodeData* pData) const = 0;
  virtual bool SetTransform(const VistaTransformMatrix& matTrans, IVistaNodeData* pData)       = 0;

  virtual bool Rotate(float nEulerX, float nEulerY, float nEulerZ, IVistaNodeData* pData) = 0;
  virtual bool Rotate(const VistaQuaternion& qRotation, IVistaNodeData* pData)            = 0;
  virtual bool Translate(const VistaVector3D& v3Trans, IVistaNodeData* pData)             = 0;

  virtual bool GetWorldTransform(
      VistaTransformMatrix& matTransform, const IVistaNodeData* pData) const                  = 0;
  virtual bool GetWorldPosition(VistaVector3D& v3Position, const IVistaNodeData* pData) const = 0;
  virtual bool GetWorldOrientation(
      VistaQuaternion& qOrientation, const IVistaNodeData* pData) const = 0;

  /*******************************************/
  /* Node Hierarchy                          */
  /*******************************************/

  virtual bool CanAddChild(IVistaNodeData* pChildData, const IVistaNodeData* pData) const = 0;
  virtual bool AddChild(IVistaNodeData* pChildData, IVistaNodeData* pData)                = 0;
  virtual bool InsertChild(IVistaNodeData* pChildData, int nIndex, IVistaNodeData* pData) = 0;
  virtual bool DisconnectChild(int nChildindex, IVistaNodeData* pData)                    = 0;

  /*******************************************/
  /* SwitchNodes                             */
  /*******************************************/

  virtual int  GetActiveChild(const IVistaNodeData* pData) const      = 0;
  virtual bool SetActiveChild(int nChildIndex, IVistaNodeData* pData) = 0;

  /*******************************************/
  /* LOD Nodes                               */
  /*******************************************/

  virtual bool GetRange(std::vector<float>& vecRanges, const IVistaNodeData* pData) const = 0;
  virtual bool SetRange(const std::vector<float>& vecRanges, IVistaNodeData* pData)       = 0;
  virtual bool GetCenter(VistaVector3D& v3Center, const IVistaNodeData* pData) const      = 0;
  virtual bool SetCenter(const VistaVector3D& v3Center, IVistaNodeData* pData)            = 0;

  /*******************************************/
  /* Light Nodes                             */
  /*******************************************/
  virtual bool GetLightAmbientColor(VISTA_LIGHTTYPE nType, float& nRed, float& nGreen, float& nBlue,
      const IVistaNodeData* pData) const = 0;
  virtual bool SetLightAmbientColor(
      VISTA_LIGHTTYPE nType, float nRed, float nGreen, float nBlue, IVistaNodeData* pData) = 0;
  virtual bool GetLightDiffuseColor(VISTA_LIGHTTYPE nType, float& nRed, float& nGreen, float& nBlue,
      const IVistaNodeData* pData) const                                                   = 0;
  virtual bool SetLightDiffuseColor(
      VISTA_LIGHTTYPE nType, float nRed, float nGreen, float nBlue, IVistaNodeData* pData) = 0;
  virtual bool GetLightSpecularColor(VISTA_LIGHTTYPE nType, float& nRed, float& nGreen,
      float& nBlue, const IVistaNodeData* pData) const                                     = 0;
  virtual bool SetLightSpecularColor(
      VISTA_LIGHTTYPE nType, float nRed, float nGreen, float nBlue, IVistaNodeData* pData) = 0;
  virtual bool SetLightColor(
      VISTA_LIGHTTYPE nType, float nRed, float nGreen, float nBlue, IVistaNodeData* pData) = 0;

  virtual bool GetLightAttenuation(
      VISTA_LIGHTTYPE nType, VistaVector3D& v3Attenuation, const IVistaNodeData* pData) const = 0;
  virtual bool SetLightAttenuation(
      VISTA_LIGHTTYPE nType, const VistaVector3D& v3Attenuation, IVistaNodeData* pData) = 0;

  virtual bool GetLightPosition(
      VISTA_LIGHTTYPE nType, VistaVector3D&, const IVistaNodeData* pData) const = 0;
  virtual bool SetLightPosition(
      VISTA_LIGHTTYPE nType, const VistaVector3D& v3Position, IVistaNodeData* pData) = 0;
  virtual bool GetLightDirection(
      VISTA_LIGHTTYPE nType, VistaVector3D&, const IVistaNodeData* pData) const = 0;
  virtual bool SetLightDirection(
      VISTA_LIGHTTYPE nType, const VistaVector3D& v3Driection, IVistaNodeData* pData) = 0;

  virtual float GetLightIntensity(const IVistaNodeData* pData) const       = 0;
  virtual bool  SetLightIntensity(float fIntensity, IVistaNodeData* pData) = 0;

  virtual float GetSpotCharacter(const IVistaNodeData* pData) const       = 0;
  virtual bool  SetSpotCharacter(float fCharacter, IVistaNodeData* pData) = 0;

  virtual int  GetSpotDistribution(const IVistaNodeData* pData) const    = 0;
  virtual bool SetSpotDistribution(int nExponent, IVistaNodeData* pData) = 0;

  virtual bool GetLightIsEnabled(const IVistaNodeData* pData) const    = 0;
  virtual void SetLightIsEnabled(bool bEnabled, IVistaNodeData* pData) = 0;

  virtual const std::vector<VistaLightNode*>& GetAllLightNodes() const                    = 0;
  virtual void                                RegisterLightNode(VistaLightNode* pLight)   = 0;
  virtual void                                UnregisterLightNode(VistaLightNode* pLight) = 0;

  /*******************************************/
  /* 3D Text Nodes                           */
  /*******************************************/
  virtual IVista3DText* CreateFontRepresentation(const std::string& sFontName) = 0;
  virtual bool          DestroyFontRepresentation(IVista3DText* pText)         = 0;

  /*******************************************/
  /* Extension/OpenGL Nodes                  */
  /*******************************************/

  virtual bool InitExtensionNode(VistaExtensionNode* pNode, IVistaNodeData* pData) = 0;
  virtual bool InitOpenGLNode(IVistaNodeData* pData, VistaOpenGLNode*)             = 0;

  /*******************************************/
  /* Geometry Nodes                          */
  /*******************************************/

  virtual bool SetGeometry(IVistaGeometryData* pGeomData, IVistaNodeData* pNodeData) = 0;
  virtual void ClearGeomNodeData(VistaGeomNode* pNodeData) const                     = 0;

  /*******************************************/
  /* Node Loading/Saving/Optimization        */
  /*******************************************/

  virtual IVistaNode* LoadNode(const std::string& strFileName, float     fScale = 1.0f,
      VistaSceneGraph::eOptFlags = VistaSceneGraph::OPT_NONE, const bool bVerbose = false) = 0;

  virtual bool SaveNode(const std::string& sFileName, IVistaNode* pNode) = 0;

  virtual bool ApplyOptimizationToNode(
      IVistaNode* pNode, VistaSceneGraph::eOptFlags eFlags, const bool bVerbose = false) = 0;

  virtual IVistaNode* CloneSubtree(IVistaNodeData* pNodeData) = 0;

  /*******************************************/
  /* NON-VIRTUAL METHODS                     */
  /*******************************************/

  VistaSceneGraph* GetVistaSceneGraph() const;

  VistaNode* NewNode(VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName);
  VistaGroupNode* NewGroupNode(
      VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName);
  VistaSwitchNode* NewSwitchNode(
      VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName);
  VistaLODNode* NewLODNode(
      VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName);
  VistaGeomNode* NewGeomNode(VistaGroupNode* pParent, VistaGeometry* pGeom, IVistaNodeData* pData,
      const std::string& strName);
  VistaAmbientLight* NewAmbientLight(
      VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName);
  VistaDirectionalLight* NewDirectionalLight(
      VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName);
  VistaPointLight* NewPointLight(
      VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName);
  VistaSpotLight* NewSpotLight(
      VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& strName);
  VistaTextNode* NewTextNode(
      VistaGroupNode* pParent, IVista3DText*, IVistaNodeData* pData, const std::string& sName);
  VistaTransformNode* NewTransformNode(
      VistaGroupNode* pParent, IVistaNodeData* pData, const std::string& sName);
  VistaExtensionNode* NewExtensionNode(VistaGroupNode* pParent,
      IVistaExplicitCallbackInterface* pExtension, IVistaNodeData* pDatapData,
      const std::string& sName);
  VistaOpenGLNode*    NewOpenGLNode(VistaGroupNode* pParent, IVistaOpenGLDraw* pDi,
         IVistaNodeData* pData, const std::string& strName);

 protected:
  IVistaNodeBridge();

 private:
  typedef std::map<VistaGeometry*, int> GeomRefMap;
  GeomRefMap                            m_mpGeomRefs;
  VistaSceneGraph*                      m_pSceneGraph;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif
