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

#ifndef _VISTAVIRTUALPLATFORMADAPTER_H
#define _VISTAVIRTUALPLATFORMADAPTER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaAspects/VistaTransformable.h>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaVirtualPlatform;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Makes a VistaVirtualPlatform available by means of the
 * IVistaTransformable interface. The calls for setting the virtual
 * platform parameters component-wise (trans, rot, scale) are
 * forwarded to the underlying virtual platform. If a whole
 * transformation matrix is set, it is decomposed first and the result
 * is then assigned to the virtual platform components.
 */
class VISTAKERNELAPI VistaVirtualPlatformAdapter : public IVistaTransformable {

 public:
  VistaVirtualPlatformAdapter();
  ~VistaVirtualPlatformAdapter();

  VistaVirtualPlatform* GetVirtualPlatform() const;
  void                  SetVirtualPlatform(VistaVirtualPlatform*);

  // #######################################################################
  // TRANSFORMABLE API
  // #######################################################################
  virtual bool SetTranslation(const VistaVector3D& v3Translation);
  virtual bool SetTranslation(const float fX, const float fY, const float fZ);
  virtual bool SetTranslation(const float a3fTranslation[3]);
  virtual bool SetTranslation(const double a3dTranslation[3]);

  virtual bool Translate(const VistaVector3D& v3Translation);
  virtual bool Translate(const float fX, const float fY, const float fZ);
  virtual bool Translate(const float a3fTranslation[3]);
  virtual bool Translate(const double a3dTranslation[3]);

  virtual bool SetRotation(const VistaQuaternion& qRotation);
  virtual bool SetRotation(const float fX, const float fY, const float fZ, const float fW);
  virtual bool SetRotation(const float a4fRotation[4]);
  virtual bool SetRotation(const double a4dRotation[4]);

  virtual bool Rotate(const VistaQuaternion& qRotation);
  virtual bool Rotate(const float fX, const float fY, const float fZ, const float fW);
  virtual bool Rotate(const float a4fRotation[4]);
  virtual bool Rotate(const double a4dRotation[4]);

  virtual bool SetScale(const VistaVector3D& v3Scale);
  virtual bool SetScale(const float fX, const float fY, const float fZ);
  virtual bool SetScale(const float a3fScale[3]);
  virtual bool SetScale(const double a3dScale[3]);

  virtual bool Scale(const VistaVector3D& v3Scale);
  virtual bool Scale(const float fX, const float fY, const float fZ);
  virtual bool Scale(const float a3fScale[3]);
  virtual bool Scale(const double a3dScale[3]);

  virtual bool SetTransform(const VistaTransformMatrix& matTransform);
  virtual bool SetTransform(const float a16fTransform[16], const bool bColumnMajor = false);
  virtual bool SetTransform(const double a16dTransform[16], const bool bColumnMajor = false);

  virtual bool Transform(const VistaTransformMatrix& matTransform);
  virtual bool Transform(const float a16fTransform[16], const bool bColumnMajor = false);
  virtual bool Transform(const double a16dTransform[16], const bool bColumnMajor = false);

  // #######################################################################
  // LOCATABLE API
  // #######################################################################
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

 private:
  VistaVirtualPlatform* m_pPlatform;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
