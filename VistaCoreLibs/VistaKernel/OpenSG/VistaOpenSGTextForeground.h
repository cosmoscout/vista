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

#ifndef _VISTA_OPENSG_TEXTFOREGROUND_H_
#define _VISTA_OPENSG_TEXTFOREGROUND_H_

#ifdef __sgi
#pragma once
#endif

#include <VistaKernel/OpenSG/VistaOpenSGTextForegroundBase.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGStatElemDesc.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

OSG_BEGIN_NAMESPACE

class TextTXFFace;

class VISTAKERNELAPI VistaOpenSGTextForeground : public VistaOpenSGTextForegroundBase {
 private:
  typedef VistaOpenSGTextForegroundBase Inherited;

  /*==========================  PUBLIC  =================================*/
 public:
  /*---------------------------------------------------------------------*/
  /*                             Sync                                    */
  /*---------------------------------------------------------------------*/

  virtual void changed(BitVector whichField, UInt32 origin);

  /*---------------------------------------------------------------------*/
  /*                            Output                                   */
  /*---------------------------------------------------------------------*/

  virtual void dump(UInt32 uiIndent = 0, const BitVector bvFlags = 0) const;

  /*---------------------------------------------------------------------*/
  /*                             Draw                                    */
  /*---------------------------------------------------------------------*/

  virtual void draw(DrawActionBase* action, Viewport* port);

  /*---------------------------------------------------------------------*/
  /*                      Convenience Functions                          */
  /*---------------------------------------------------------------------*/

  /*=========================  PROTECTED  ===============================*/
 protected:
  // Variables should all be in TextForegroundBase.

  /*---------------------------------------------------------------------*/
  /*                         Constructors                                */
  /*---------------------------------------------------------------------*/

  VistaOpenSGTextForeground(void);
  VistaOpenSGTextForeground(const VistaOpenSGTextForeground& source);

  /*---------------------------------------------------------------------*/
  /*                          Destructors                                */
  /*---------------------------------------------------------------------*/

  virtual ~VistaOpenSGTextForeground(void);

  /*==========================  PRIVATE  ================================*/
 private:
  _COSGFaceHlp* initTextFace(UInt32 nFaceType, UInt32 size);
  _COSGFaceHlp* getOrGetAndInitTexChunk(UInt32 nFaceType, UInt32 size);

  friend class FieldContainer;
  friend class VistaOpenSGTextForegroundBase;

  static void initMethod(void);

  //    static void initText(void);

  // prohibit default functions (move to 'public' if you need one)

  void operator=(const VistaOpenSGTextForeground& source);
};

typedef VistaOpenSGTextForeground* VistaOpenSGTextForegroundP;

OSG_END_NAMESPACE

#include "VistaOpenSGTextForeground.inl"
#include "VistaOpenSGTextForegroundBase.inl"

#endif
