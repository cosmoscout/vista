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

#ifndef _VISTA_OPENSG_GLOVERLAYFOREGROUND_FIELDS_H_
#define _VISTA_OPENSG_GLOVERLAYFOREGROUND_FIELDS_H_

#ifdef __sgi
#pragma once
#endif

#if defined(WIN32)
// diable warnings from OpenSG includes
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGConfig.h>

#include <OpenSG/OSGFieldContainerPtr.h>
#include <OpenSG/OSGNodeCoreFieldDataType.h>
#include <OpenSG/OSGSystemDef.h>

#include <OpenSG/OSGForegroundFields.h>

#if defined(WIN32)
#pragma warning(pop)
#endif

OSG_BEGIN_NAMESPACE

class VistaOpenSGGLOverlayForeground;

typedef FCPtr<ForegroundPtr, VistaOpenSGGLOverlayForeground> VistaOpenSGGLOverlayForegroundPtr;

template <>
struct FieldDataTraits<VistaOpenSGGLOverlayForegroundPtr>
    : public FieldTraitsRecurseMapper<VistaOpenSGGLOverlayForegroundPtr, true> {
  static DataType _type;

  enum { StringConvertable = 0x00 };
  enum { bHasParent = 0x01 };

  static DataType& getType(void) {
    return _type;
  }

  static const char* getSName(void) {
    return "SFGLOverlayForegroundPtr";
  }
  static const char* getMName(void) {
    return "MFGLOverlayForegroundPtr";
  }
};

typedef SField<VistaOpenSGGLOverlayForegroundPtr> SFVistaOpenSGGLOverlayForegroundPtr;
typedef MField<VistaOpenSGGLOverlayForegroundPtr> MFVistaOpenSGGLOverlayForegroundPtr;

OSG_END_NAMESPACE

#endif
