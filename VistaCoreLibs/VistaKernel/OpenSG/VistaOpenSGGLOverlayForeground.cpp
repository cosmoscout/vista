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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include "VistaOpenSGGLOverlayForeground.h"

#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGNodePtr.h>
#include <OpenSG/OSGViewport.h>

#ifdef WIN32
#pragma warning(pop)
#endif

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

OSG_USING_NAMESPACE

/*----------------------- constructors & destructors ----------------------*/
VistaOpenSGGLOverlayForeground::VistaOpenSGGLOverlayForeground(void)
    : Inherited() {
}

/* */
VistaOpenSGGLOverlayForeground::VistaOpenSGGLOverlayForeground(
    const VistaOpenSGGLOverlayForeground& source)
    : Inherited(source) {
}

/* */
VistaOpenSGGLOverlayForeground::~VistaOpenSGGLOverlayForeground(void) {
}

/*----------------------------- class specific ----------------------------*/
void VistaOpenSGGLOverlayForeground::initMethod(void) {
}

/* */
void VistaOpenSGGLOverlayForeground::changed(BitVector whichField, UInt32 origin) {
  Inherited::changed(whichField, origin);
}

/* */
void VistaOpenSGGLOverlayForeground::dump(UInt32, const BitVector) const {
  SLOG << "Dump VistaOpenSGGLOverlayForeground NI" << std::endl;
}

void VistaOpenSGGLOverlayForeground::draw(DrawActionBase* action, Viewport* port) {
  int count = m_mfGLOverlays.size();
  for (int i = 0; i < count; ++i) {
    IVistaOpenGLDraw* pOpenGLDraw = static_cast<IVistaOpenGLDraw*>(m_mfGLOverlays[i]);
    pOpenGLDraw->Do();
  }
}

/*-------------------------------------------------------------------------*/
/*                              cvs id's                                   */
#ifdef __sgi
#pragma set woff 1174
#endif
#ifdef OSG_LINUX_ICC
#pragma warning(disable : 177)
#endif
