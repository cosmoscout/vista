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
#include "VistaOpenSGTextForeground.h"

#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>

#include <VistaBase/VistaStreamUtils.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGNodePtr.h>

#include <OpenSG/OSGTextFaceFactory.h>
#include <OpenSG/OSGTextLayoutParam.h>
#include <OpenSG/OSGTextLayoutResult.h>
#include <OpenSG/OSGTextTXFFace.h>
#include <OpenSG/OSGTextTXFGlyph.h>

#include <OpenSG/OSGViewport.h>

#include <OpenSG/OSGStatisticsDefaultFont.h>

#ifdef WIN32
#pragma warning(pop)
#endif

#include <sstream>
#include <stdio.h>
#include <stdlib.h>

OSG_USING_NAMESPACE

/*----------------------- constructors & destructors ----------------------*/
VistaOpenSGTextForeground::VistaOpenSGTextForeground(void)
    : Inherited() {
}

/* */
VistaOpenSGTextForeground::VistaOpenSGTextForeground(const VistaOpenSGTextForeground& source)
    : Inherited(source) {
}

/* */
VistaOpenSGTextForeground::~VistaOpenSGTextForeground(void) {
  for (MField<void*>::const_iterator cit = m_vFaceLookup.begin(); cit != m_vFaceLookup.end();
       ++cit) {
    _COSGFaceHlp* p = static_cast<_COSGFaceHlp*>(*cit);
    delete p;
  }
}

/*----------------------------- class specific ----------------------------*/
void VistaOpenSGTextForeground::initMethod(void) {
}

/* */
void VistaOpenSGTextForeground::changed(BitVector whichField, UInt32 origin) {
  Inherited::changed(whichField, origin);
}

/* */
void VistaOpenSGTextForeground::dump(UInt32, const BitVector) const {
  SLOG << "Dump VistaOpenSGTextForeground NI" << std::endl;
}

VistaOpenSGTextForegroundBase::_COSGFaceHlp* VistaOpenSGTextForeground::initTextFace(
    UInt32 nFaceType, UInt32 size) {
  TextTXFParam param;
  param.size         = (UInt32)size;
  param.gap          = 2;
  param.textureWidth = 64;
  std::string sFaceType;

  switch (nFaceType) {
  case 1: {
    sFaceType = "TYPEWRITER";
    break;
  }
  case 2: {
    sFaceType = "SERIF";
    break;
  }
  case 0:
  default: {
    nFaceType = 0; // set to default, no matter what was passed
    //#ifdef WIN32
    //	sFaceType = "monospace";
    //#else
    sFaceType = "SANS";
    //#endif
    break;
  }
  }

  TextTXFFace* pFace = TextTXFFace::create(sFaceType, TextFace::STYLE_PLAIN, param);
  if (pFace != NULL) {
    // adds ref count
    ImagePtr        texture = pFace->getTexture();
    TextureChunkPtr p       = TextureChunk::create();
    beginEditCP(p);
    {
      p->setImage(texture);
      p->setWrapS(GL_CLAMP);
      p->setWrapT(GL_CLAMP);
      p->setEnvMode(GL_MODULATE);
    }
    endEditCP(p);

    _COSGFaceHlp* pHlp = new _COSGFaceHlp(pFace, nFaceType, size, p);
    return pHlp;
  } else {
    vstr::errp() << "[VistaOpenSGTextForeground::initTextFont] -- could not create TXFFace"
                 << std::endl;
    return NULL;
  }
}

VistaOpenSGTextForegroundBase::_COSGFaceHlp* VistaOpenSGTextForeground::getOrGetAndInitTexChunk(
    UInt32 nFaceType, UInt32 size) {
  _COSGFaceHlp* p = NULL;
  for (MField<void*>::const_iterator cit = m_vFaceLookup.begin(); cit != m_vFaceLookup.end();
       ++cit) {
    p = static_cast<_COSGFaceHlp*>(*cit);
    if ((*p).m_nSize == size && (*p).m_nFaceType == nFaceType)
      break;
    p = NULL;
  }

  if (!p) {
    // we did not find it!
    p = initTextFace(nFaceType, size);
    if (!p)
      return NULL; // failed...

    // add p to lookup vector
    m_vFaceLookup.push_back(p);
  }

  return p;
}

void VistaOpenSGTextForeground::draw(DrawActionBase* action, Viewport* port) {
  Real32 pw = Real32(port->getPixelWidth());
  Real32 ph = Real32(port->getPixelHeight());

  if (pw < 1 || ph < 1)
    return;

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  /**
   * @todo
   * something goes wrong with this when the shiny new
   * VistaOpenSGNormalMapMaterial-Shader is in use!?
   * -> check all gl state changes and corresponding
   */

  // glPushAttrib(
  ////GL_LIGHTING_BIT |
  // GL_POLYGON_BIT |
  // GL_DEPTH_BUFFER_BIT |
  // GL_ENABLE_BIT |
  // GL_TEXTURE_BIT // note: these may be altered by the manual call to texchunk->activate()
  //);

  glDisable(GL_LIGHTING);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_COLOR_MATERIAL);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  Real32 aspect = pw / ph;

  glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_ALPHA_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  // draw text
  int text_count = m_mfTexts.size();
  // std::string strText;
  for (int i = 0; i < text_count; i++) {
    Vista2DText* text = (Vista2DText*)m_mfTexts[i]; //.getValue(i) ;
    if ((*text).GetEnabled() == false)
      continue; // go on, skip this one

    Vista2DText::Vista2DTextFontFamily family;
    text->GetFontFamily(family);
    Real32 size;
    text->GetSize(size);
    _COSGFaceHlp* pHlp = getOrGetAndInitTexChunk(UInt32(family), UInt32(size));
    if (!pHlp)
      continue; // skip, we could not load the font

    TextureChunkPtr texChunk = (*pHlp).m_pTexture;
    if (texChunk == NullFC) {
      // fall back to default font, TYPEWRITER, size 20
      pHlp = getOrGetAndInitTexChunk(Vista2DText::TYPEWRITER, 20);
      if (!pHlp)
        continue; // serious error at this point? default font couldn't be loaded.
      if ((*pHlp).m_pTexture == NullFC)
        continue; // leave loop
    }

    // width and height of the projection plane
    Real32 ppwidth  = ph / size * aspect;
    Real32 ppheight = -ph / size;
    glLoadIdentity();
    glOrtho(0.0, ppwidth, ppheight, 0.0, 0, 1);

    TextLayoutParam layoutParam;
    layoutParam.majorAlignment = TextLayoutParam::ALIGN_BEGIN;
    layoutParam.minorAlignment = TextLayoutParam::ALIGN_BEGIN;
    TextLayoutResult layoutResult;

    const std::string& strText = text->GetTextConstRef();

    (*pHlp).m_pFace->layout(strText, layoutParam, layoutResult);
    texChunk->activate(action);

    unsigned char cr, cg, cb;
    text->GetColor(cr, cg, cb);
    float fr, fg, fb;
    fr = cr / 255.0f;
    fg = cg / 255.0f;
    fb = cb / 255.0f;

    float vec[] = {fr, fg, fb, 1.0f};
    glColor4fv((GLfloat*)vec);

    float pos_x, pos_y;
    text->GetPosition(pos_x, pos_y);

    pos_y = 1.0f - pos_y;

    glBegin(GL_QUADS);
    UInt32 n, numGlyphs = layoutResult.getNumGlyphs();
    Real32 horiAscent = pHlp->m_pFace->getHoriAscent();
    for (n = 0; n < numGlyphs; ++n) {
      const TextTXFGlyph& glyph  = (*pHlp).m_pFace->getTXFGlyph(layoutResult.indices[n]);
      Real32              width  = glyph.getWidth();
      Real32              height = glyph.getHeight();
      // No need to draw invisible glyphs
      if ((width <= 0.f) || (height <= 0.f))
        continue;

      // Calculate coordinates
      const Vec2f& pos = layoutResult.positions[n];

      Real32 posLeft   = pos.x() + ppwidth * pos_x;
      Real32 posTop    = horiAscent + pos.y() + ppheight * pos_y;
      Real32 posRight  = posLeft + width;
      Real32 posBottom = posTop - height;

      Real32 texCoordLeft   = glyph.getTexCoord(TextTXFGlyph::COORD_LEFT);
      Real32 texCoordTop    = glyph.getTexCoord(TextTXFGlyph::COORD_TOP);
      Real32 texCoordRight  = glyph.getTexCoord(TextTXFGlyph::COORD_RIGHT);
      Real32 texCoordBottom = glyph.getTexCoord(TextTXFGlyph::COORD_BOTTOM);

      // lower left corner
      glTexCoord2f(texCoordLeft, texCoordBottom);
      glVertex2f(posLeft, posBottom);

      // lower right corner
      glTexCoord2f(texCoordRight, texCoordBottom);
      glVertex2f(posRight, posBottom);

      // upper right corner
      glTexCoord2f(texCoordRight, texCoordTop);
      glVertex2f(posRight, posTop);

      // upper left corner
      glTexCoord2f(texCoordLeft, texCoordTop);
      glVertex2f(posLeft, posTop);
    }
    glEnd();
    texChunk->deactivate(action);
  }

  // glDisable(GL_ALPHA_TEST);
  // glDisable(GL_BLEND);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glPopAttrib();

  // if(depth == GL_TRUE)
  //	glEnable(GL_DEPTH_TEST);
  // if(light == GL_TRUE)
  //	glEnable(GL_LIGHTING);
  // if(colmat == GL_TRUE)
  //	glEnable(GL_COLOR_MATERIAL);

  // glPolygonMode(GL_FRONT_AND_BACK, fill[0]);
}

/*-------------------------------------------------------------------------*/
/*                              cvs id's                                   */
#ifdef __sgi
#pragma set woff 1174
#endif
#ifdef OSG_LINUX_ICC
#pragma warning(disable : 177)
#endif
