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

#ifndef _AC3DSCENEFILETYPE_H_
#define _AC3DSCENEFILETYPE_H_
#ifdef __sgi
#pragma once
#endif

#include <VistaKernel/VistaKernelConfig.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGImageFields.h>
#include <OpenSG/OSGSceneFileType.h>

#ifdef WIN32
#pragma warning(pop)
#endif

#include <map>
#include <string>

OSG_BEGIN_NAMESPACE

class VISTAKERNELAPI OSGAC3DSceneFileType : public SceneFileType {
  /*==========================  PUBLIC  =================================*/
 public:
  /*---------------------------------------------------------------------*/
  /*! \name                   Class Get                                  */
  /*! \{                                                                 */

  static OSGAC3DSceneFileType& the(void);

  /*! \}                                                                 */
  /*---------------------------------------------------------------------*/
  /*! \name                   Destructors                                */
  /*! \{                                                                 */

  virtual ~OSGAC3DSceneFileType(void);

  /*! \}                                                                 */
  /*---------------------------------------------------------------------*/
  /*! \name                   Get                                        */
  /*! \{                                                                 */

  virtual const Char8* getName(void) const;

  /*! \}                                                                 */
  /*---------------------------------------------------------------------*/
  /*! \name                   Read                                       */
  /*! \{                                                                 */

  virtual NodePtr read(std::istream& is, const Char8* fileNameOrExtension) const;

  /*! \}                                                                 */
  /*=========================  PROTECTED  ===============================*/
 protected:
  /*---------------------------------------------------------------------*/
  /*! \name                      Member                                  */
  /*! \{                                                                 */

  static const Char8*         _suffixA[];
  static OSGAC3DSceneFileType _the;

  /*! \}                                                                 */
  /*---------------------------------------------------------------------*/
  /*! \name                   Constructors                               */
  /*! \{                                                                 */

  OSGAC3DSceneFileType(const Char8* suffixArray[], UInt16 suffixByteCount, bool override,
      UInt32 overridePriority, UInt32 flags);

  OSGAC3DSceneFileType(const OSGAC3DSceneFileType& obj);

  /*! \}                                                                 */
  /*==========================  PRIVATE  ================================*/
 private:
  typedef SceneFileType Inherited;

  /* prohibit default function (move to 'public' if needed) */
  void operator=(const OSGAC3DSceneFileType& source);
};

typedef OSGAC3DSceneFileType* OSGAC3DSceneFileTypeP;

OSG_END_NAMESPACE

#define OSGAC3DSCENEFILETYPE_HEADER_CVSID "@(#)$Id$"

#endif // _AC3DSCENEFILETYPE_H_
