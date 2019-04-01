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


#ifndef _VISTA_OPENSG_TEXTFOREGROUND_FIELDS_H_
#define _VISTA_OPENSG_TEXTFOREGROUND_FIELDS_H_

#ifdef __sgi
#pragma once
#endif

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif

#include <OpenSG/OSGConfig.h>

#include <OpenSG/OSGFieldContainerPtr.h>
#include <OpenSG/OSGNodeCoreFieldDataType.h>
#include <OpenSG/OSGSystemDef.h>

#include <OpenSG/OSGForegroundFields.h>

#ifdef WIN32
#pragma warning(pop)
#endif

OSG_BEGIN_NAMESPACE

class VistaOpenSGTextForeground;

typedef FCPtr<ForegroundPtr, VistaOpenSGTextForeground> VistaOpenSGTextForegroundPtr;

template <>
struct FieldDataTraits<VistaOpenSGTextForegroundPtr> :
	public FieldTraitsRecurseMapper<VistaOpenSGTextForegroundPtr, true>
{
	static DataType             _type;

	enum                        { StringConvertable = 0x00 };
	enum                        { bHasParent        = 0x01 };

	static DataType &getType (void) { return _type;        }

	static const char     *getSName(void) { return "SFTextForegroundPtr"; }
	static const char     *getMName(void) { return "MFTextForegroundPtr"; }
};


typedef SField<VistaOpenSGTextForegroundPtr> SFVistaOpenSGTextForegroundPtr;
typedef MField<VistaOpenSGTextForegroundPtr> MFVistaOpenSGTextForegroundPtr;

OSG_END_NAMESPACE

#endif
