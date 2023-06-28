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

#if !defined _VISTAVIRTUALPLATFORM_H
#define _VISTAVIRTUALPLATFORM_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaAspects/VistaNameable.h>
#include <VistaAspects/VistaObserveable.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * An extension to VistaReferenceFrame which allows observers to
 * listen for changes to the reference frame.
 */
class VISTAKERNELAPI VistaVirtualPlatform : public VistaReferenceFrame,
                                            public IVistaObserveable,
                                            public IVistaNameable {
 public:
  /**
   * MSG tags for observers
   */
  enum {
    MSG_TRANSLATION_CHANGE = IVistaObserveable::MSG_LAST,
    MSG_ROTATION_CHANGE,
    MSG_SCALE_CHANGE,
    MSG_NAME_CHANGE,
    MSG_LAST
  };

  VistaVirtualPlatform();
  VistaVirtualPlatform(
      const VistaVector3D& refTranslation, const VistaQuaternion& refRotation, float fScale = 1.0f);
  virtual ~VistaVirtualPlatform();

  //###############################################################
  // REFERENCEFRAME SETTERS (adds notification)
  //###############################################################
  bool SetTranslation(const VistaVector3D& refTranslation);
  bool SetRotation(const VistaQuaternion& refRotation);
  bool SetScale(float fScale);

  //###############################################################
  // NAMEABLEINTERFACE
  //###############################################################
  virtual std::string GetNameForNameable() const;
  virtual void        SetNameForNameable(const std::string& sNewName);

 private:
  std::string m_strNameForNameable;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTAVIRTUALPLATFORM_H)
