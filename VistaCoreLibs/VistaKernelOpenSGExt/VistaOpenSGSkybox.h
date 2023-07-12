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

#ifndef _VISTAOPENSGSKYBOX_H
#define _VISTAOPENSGSKYBOX_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#pragma once
#pragma warning(disable : 4231)
#pragma warning(disable : 4312)
#pragma warning(disable : 4267)
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#endif

#include "VistaKernelOpenSGExtConfig.h"

#include <set>
#include <string>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaViewport;
class SkyboxData;
class VistaColor;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELOPENSGEXTAPI VistaOpenSGSkybox {
 public:
  VistaOpenSGSkybox(const std::string& strTop, const std::string& strBottom,
      const std::string& strLeft, const std::string& strRight, const std::string& strFront,
      const std::string& strBack);

  ~VistaOpenSGSkybox();

  bool       AttachToViewport(VistaViewport* pViewport);
  bool       DetachFromViewport(VistaViewport* pViewport);
  bool       GetIsAttachedToViewport(VistaViewport* pViewport);
  bool       SetColor(float fR, float fG, float fB, float fA);
  bool       SetColor(const VistaColor& oColor);
  bool       GetColor(float& fR, float& fG, float& fB, float& fA) const;
  VistaColor GetColor() const;

 protected:
 private:
  SkyboxData* m_pData;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGSKYBOX_H
