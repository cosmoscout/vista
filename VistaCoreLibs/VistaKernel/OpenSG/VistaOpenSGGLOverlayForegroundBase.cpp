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

#include <GL/glew.h>

#include "VistaOpenSGGLOverlayForeground.h"
#include "VistaOpenSGGLOverlayForegroundBase.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
// diable warnings from OpenSG includes
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGConfig.h>

#if defined(WIN32)
#pragma warning(pop)
#endif

OSG_USING_NAMESPACE

const BitVector VistaOpenSGGLOverlayForegroundBase::GLOverlaysFieldMask =
    (TypeTraits<BitVector>::One << VistaOpenSGGLOverlayForegroundBase::GLOverlaysFieldId);

const BitVector VistaOpenSGGLOverlayForegroundBase::MTInfluenceMask =
    (Inherited::MTInfluenceMask) | (static_cast<BitVector>(0x0) << Inherited::NextFieldId);

// Field descriptions

/*! \var std::string     TextForegroundBase::_mfFormats
        The format strings for the given StatElemDesc IDs. If not set, name and value are used.
*/
/*! \var Real32          TextForegroundBase::_sfSize
        Height of a single line, in  pixel.
*/
/*! \var Color4f         TextForegroundBase::_sfColor
        Color of the text.
*/

//! TextForeground description

FieldDescription* VistaOpenSGGLOverlayForegroundBase::_desc[] = {
    new FieldDescription(MField<void*>::getClassType(), "GLOverlays", GLOverlaysFieldId,
        GLOverlaysFieldMask, false,
        (FieldAccessMethod)&VistaOpenSGGLOverlayForegroundBase::getMFGLOverlays),
};

FieldContainerType VistaOpenSGGLOverlayForegroundBase::_type("VistaOpenSGGLOverlaysForeground",
    "Foreground", NULL, (PrototypeCreateF)&VistaOpenSGGLOverlayForegroundBase::createEmpty,
    VistaOpenSGGLOverlayForeground::initMethod, _desc, sizeof(_desc));

// OSG_FIELD_CONTAINER_DEF(TextForegroundBase, TextForegroundPtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType& VistaOpenSGGLOverlayForegroundBase::getType(void) {
  return _type;
}

const FieldContainerType& VistaOpenSGGLOverlayForegroundBase::getType(void) const {
  return _type;
}

FieldContainerPtr VistaOpenSGGLOverlayForegroundBase::shallowCopy(void) const {
  VistaOpenSGGLOverlayForegroundPtr returnValue;

  newPtr(returnValue, dynamic_cast<const VistaOpenSGGLOverlayForeground*>(this));

  return returnValue;
}

UInt32 VistaOpenSGGLOverlayForegroundBase::getContainerSize(void) const {
  return sizeof(VistaOpenSGGLOverlayForeground);
}

//#if !defined(OSG_FIXED_MFIELDSYNC)
void VistaOpenSGGLOverlayForegroundBase::executeSync(
    FieldContainer& other, const BitVector& whichField) {
  this->executeSyncImpl((VistaOpenSGGLOverlayForegroundBase*)&other, whichField);
}
//#else
/*void TextForegroundBase::executeSync(      FieldContainer &other,
                                                                        const BitVector &whichField,
const SyncInfo       &sInfo     )
{
        this->executeSyncImpl((TextForegroundBase *) &other, whichField, sInfo);
}
void TextForegroundBase::execBeginEdit(const BitVector &whichField,
                                                                                        UInt32
uiAspect, UInt32     uiContainerSize)
{
        this->execBeginEditImpl(whichField, uiAspect, uiContainerSize);
}

void TextForegroundBase::onDestroyAspect(UInt32 uiId, UInt32 uiAspect)
{
        Inherited::onDestroyAspect(uiId, uiAspect);

        _mfFormats.terminateShare(uiAspect, this->getContainerSize());
        }*/
//#endif

/*------------------------- constructors ----------------------------------*/

#ifdef OSG_WIN32_ICL
#pragma warning(disable : 383)
#endif

VistaOpenSGGLOverlayForegroundBase::VistaOpenSGGLOverlayForegroundBase(void)
    : //    _mfTexts                (),
    Inherited() {
}

#ifdef OSG_WIN32_ICL
#pragma warning(default : 383)
#endif

VistaOpenSGGLOverlayForegroundBase::VistaOpenSGGLOverlayForegroundBase(
    const VistaOpenSGGLOverlayForegroundBase& source)
    : Inherited(source) {
}

/*-------------------------- destructors ----------------------------------*/

VistaOpenSGGLOverlayForegroundBase::~VistaOpenSGGLOverlayForegroundBase(void) {
}

/*------------------------------ access -----------------------------------*/

UInt32 VistaOpenSGGLOverlayForegroundBase::getBinSize(const BitVector& whichField) {
  UInt32 returnValue = Inherited::getBinSize(whichField);

  if (FieldBits::NoField != (GLOverlaysFieldMask & whichField)) {
    returnValue += m_mfGLOverlays.getBinSize();
  }

  return returnValue;
}

void VistaOpenSGGLOverlayForegroundBase::copyToBin(
    BinaryDataHandler& pMem, const BitVector& whichField) {
  Inherited::copyToBin(pMem, whichField);

  if (FieldBits::NoField != (GLOverlaysFieldMask & whichField)) {
    m_mfGLOverlays.copyToBin(pMem);
  }
}

void VistaOpenSGGLOverlayForegroundBase::copyFromBin(
    BinaryDataHandler& pMem, const BitVector& whichField) {
  Inherited::copyFromBin(pMem, whichField);

  if (FieldBits::NoField != (GLOverlaysFieldMask & whichField)) {
    m_mfGLOverlays.copyFromBin(pMem);
  }
}

//#if !defined(OSG_FIXED_MFIELDSYNC)
void VistaOpenSGGLOverlayForegroundBase::executeSyncImpl(
    VistaOpenSGGLOverlayForegroundBase* pOther, const BitVector& whichField) {
  Inherited::executeSyncImpl(pOther, whichField);

  if (FieldBits::NoField != (GLOverlaysFieldMask & whichField))
    m_mfGLOverlays.syncWith(pOther->m_mfGLOverlays);
}
//#else
/*void TextForegroundBase::executeSyncImpl(      TextForegroundBase *pOther,
                                                                                const BitVector
&whichField, const SyncInfo          &sInfo      )
{

        Inherited::executeSyncImpl(pOther, whichField, sInfo);

        if(FieldBits::NoField != (SizeFieldMask & whichField))
                _sfSize.syncWith(pOther->_sfSize);

        if(FieldBits::NoField != (ColorFieldMask & whichField))
                _sfColor.syncWith(pOther->_sfColor);


        if(FieldBits::NoField != (FormatsFieldMask & whichField))
                _mfFormats.syncWith(pOther->_mfFormats, sInfo);


}

void TextForegroundBase::execBeginEditImpl (const BitVector &whichField,
                                                                                                 UInt32
uiAspect, UInt32     uiContainerSize)
{
        Inherited::execBeginEditImpl(whichField, uiAspect, uiContainerSize);

        if(FieldBits::NoField != (FormatsFieldMask & whichField))
                _mfFormats.beginEdit(uiAspect, uiContainerSize);

        }*/
//#endif

DataType FieldDataTraits<VistaOpenSGGLOverlayForegroundPtr>::_type(
    "VistaOpenSGGLOverlayForegroundPtr", "ForegroundPtr");

/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning(disable : 177)
#endif
