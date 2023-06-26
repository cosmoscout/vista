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

#ifndef _VISTANODE_H
#define _VISTANODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaKernel/GraphicsManager/VistaNodeInterface.h"
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaMath/VistaBoundingBox.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGroupNode;
class IVistaNodeBridge;
class IVistaNodeData;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaNode : public IVistaNode {
  friend class VistaSceneGraph;
  friend class VistaGroupNode;
  friend class IVistaNodeBridge;

 public:
  virtual ~VistaNode();

  std::string     GetName() const;
  bool            SetName(const std::string& strName);
  VISTA_NODETYPE  GetType() const;
  virtual bool    CanHaveChildren() const;
  VistaGroupNode* GetParent() const;

  bool GetIsEnabled() const;
  void SetIsEnabled(bool bEnabled);

  void Debug(std::ostream& out, int nLevel = 0) const;

  virtual bool             GetBoundingBox(VistaVector3D& v3Min, VistaVector3D& v3Max) const;
  virtual bool             GetBoundingBox(VistaBoundingBox& oBox) const;
  virtual VistaBoundingBox GetBoundingBox() const;
  virtual bool             GetWorldBoundingBox(VistaVector3D& v3Min, VistaVector3D& v3Max) const;
  virtual bool             GetWorldBoundingBox(VistaBoundingBox& oBox) const;
  virtual VistaBoundingBox GetWorldBoundingBox() const;

  IVistaNodeData*           GetData() const;
  virtual VistaType::uint64 GetTransformScore() const;

  // implementation of IVistaLocatable
  virtual bool GetTranslation(VistaVector3D& v3Translation) const;
  virtual bool GetTranslation(float& fX, float& fY, float& fZ) const;
  virtual bool GetTranslation(float a3fTranslation[3]) const;
  virtual bool GetTranslation(double a3dTranslation[3]) const;

  virtual bool GetWorldPosition(VistaVector3D& v3Position) const;
  virtual bool GetWorldPosition(float& fX, float& fY, float& fZ) const;
  virtual bool GetWorldPosition(float a3fPosition[3]) const;
  virtual bool GetWorldPosition(double a3dPosition[3]) const;

  virtual bool GetRotation(VistaQuaternion& qRotation) const;
  virtual bool GetRotation(float& fX, float& fY, float& fZ, float& fW) const;
  virtual bool GetRotation(float a4fRotation[4]) const;
  virtual bool GetRotation(double a4dRotation[4]) const;

  virtual bool GetWorldOrientation(VistaQuaternion& qOrientation) const;
  virtual bool GetWorldOrientation(float& fX, float& fY, float& fZ, float& fW) const;
  virtual bool GetWorldOrientation(float a4fOrientation[4]) const;
  virtual bool GetWorldOrientation(double a4dOrientation[4]) const;

  virtual bool GetScale(VistaVector3D& v3Scale) const;
  virtual bool GetScale(float& fX, float& fY, float& fZ) const;
  virtual bool GetScale(float a3fScale[3]) const;
  virtual bool GetScale(double a3dScale[3]) const;

  virtual bool GetWorldScale(VistaVector3D& v3Scale) const;
  virtual bool GetWorldScale(float& fX, float& fY, float& fZ) const;
  virtual bool GetWorldScale(float a3fScale[3]) const;
  virtual bool GetWorldScale(double a3dScale[3]) const;

  virtual bool GetTransform(VistaTransformMatrix& matTransform) const;
  virtual bool GetTransform(float a16fTransform[16], const bool bColumnMajor = false) const;
  virtual bool GetTransform(double a16dTransform[16], const bool bColumnMajor = false) const;

  virtual bool GetWorldTransform(VistaTransformMatrix& matTransform) const;
  virtual bool GetWorldTransform(float a16fTransform[16], const bool bColumnMajor = false) const;
  virtual bool GetWorldTransform(double a16dTransform[16], const bool bColumnMajor = false) const;

  virtual bool GetParentWorldTransform(VistaTransformMatrix& matTransform) const;
  virtual bool GetParentWorldTransform(
      float a16fTransform[16], const bool bColumnMajor = false) const;
  virtual bool GetParentWorldTransform(
      double a16dTransform[16], const bool bColumnMajor = false) const;

 protected:
  inline void IncTransScore() {
    ++m_nLocalTransScore;
  }

  // protect constructors
  VistaNode();
  VistaNode(const VistaNode&);
  VistaNode& operator=(const VistaNode&);

  VistaNode(VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData,
      const std::string& strName);

  // std::string			m_sName;
  VISTA_NODETYPE  m_nType;
  VistaGroupNode* m_pParent;

  IVistaNodeData*   m_pData;
  IVistaNodeBridge* m_pBridge;
  VistaType::uint64 m_nLocalTransScore;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANODE_H
