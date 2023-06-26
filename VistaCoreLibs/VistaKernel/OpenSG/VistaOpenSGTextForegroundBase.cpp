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

#include "VistaOpenSGTextForegroundBase.h"
#include "VistaOpenSGTextForeground.h"

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
#include <OpenSG/OSGTextTXFFace.h>

#if defined(WIN32)
#pragma warning(pop)
#endif

OSG_USING_NAMESPACE

const BitVector VistaOpenSGTextForegroundBase::TextsFieldMask =
    (TypeTraits<BitVector>::One << VistaOpenSGTextForegroundBase::TextsFieldId);

const BitVector VistaOpenSGTextForegroundBase::MTInfluenceMask =
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

FieldDescription* VistaOpenSGTextForegroundBase::_desc[] = {
    new FieldDescription(MField<void*>::getClassType(), "texts", TextsFieldId, TextsFieldMask,
        false, (FieldAccessMethod)&VistaOpenSGTextForegroundBase::getMFTexts),
};

FieldContainerType VistaOpenSGTextForegroundBase::_type("VistaOpenSGTextForeground", "Foreground",
    NULL, (PrototypeCreateF)&VistaOpenSGTextForegroundBase::createEmpty,
    VistaOpenSGTextForeground::initMethod, _desc, sizeof(_desc));

// OSG_FIELD_CONTAINER_DEF(TextForegroundBase, TextForegroundPtr)

/*------------------------------ get -----------------------------------*/

VistaOpenSGTextForegroundBase::_COSGFaceHlp::_COSGFaceHlp(
    TextTXFFace* face, UInt32 nFaceType, UInt32 nSize, TextureChunkPtr texture) {
  m_pFace = face;
  addRefP(face);
  m_nSize     = nSize;
  m_nFaceType = nFaceType;
  m_pTexture  = texture;
  addRefCP(m_pTexture);
}

VistaOpenSGTextForegroundBase::_COSGFaceHlp::~_COSGFaceHlp() {
  subRefP(m_pFace);
  subRefCP(m_pTexture);
}

FieldContainerType& VistaOpenSGTextForegroundBase::getType(void) {
  return _type;
}

const FieldContainerType& VistaOpenSGTextForegroundBase::getType(void) const {
  return _type;
}

FieldContainerPtr VistaOpenSGTextForegroundBase::shallowCopy(void) const {
  VistaOpenSGTextForegroundPtr returnValue;

  newPtr(returnValue, dynamic_cast<const VistaOpenSGTextForeground*>(this));

  return returnValue;
}

UInt32 VistaOpenSGTextForegroundBase::getContainerSize(void) const {
  return sizeof(VistaOpenSGTextForeground);
}

//#if !defined(OSG_FIXED_MFIELDSYNC)
void VistaOpenSGTextForegroundBase::executeSync(
    FieldContainer& other, const BitVector& whichField) {
  this->executeSyncImpl((VistaOpenSGTextForegroundBase*)&other, whichField);
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

VistaOpenSGTextForegroundBase::VistaOpenSGTextForegroundBase(void)
    : //    _mfTexts                (),
    Inherited() {
  // we have 5 face types, currently
  m_vFaceLookup.resize(5);
}

#ifdef OSG_WIN32_ICL
#pragma warning(default : 383)
#endif

VistaOpenSGTextForegroundBase::VistaOpenSGTextForegroundBase(
    const VistaOpenSGTextForegroundBase& source)
    : //    _mfTexts                (source._mfTexts                ),
    Inherited(source) {
}

/*-------------------------- destructors ----------------------------------*/

VistaOpenSGTextForegroundBase::~VistaOpenSGTextForegroundBase(void) {
}

/*------------------------------ access -----------------------------------*/

UInt32 VistaOpenSGTextForegroundBase::getBinSize(const BitVector& whichField) {
  UInt32 returnValue = Inherited::getBinSize(whichField);

  if (FieldBits::NoField != (TextsFieldMask & whichField)) {
    returnValue += m_mfTexts.getBinSize();
  }

  return returnValue;
}

void VistaOpenSGTextForegroundBase::copyToBin(
    BinaryDataHandler& pMem, const BitVector& whichField) {
  Inherited::copyToBin(pMem, whichField);

  if (FieldBits::NoField != (TextsFieldMask & whichField)) {
    m_mfTexts.copyToBin(pMem);
  }
}

void VistaOpenSGTextForegroundBase::copyFromBin(
    BinaryDataHandler& pMem, const BitVector& whichField) {
  Inherited::copyFromBin(pMem, whichField);

  if (FieldBits::NoField != (TextsFieldMask & whichField)) {
    m_mfTexts.copyFromBin(pMem);
  }
}

//#if !defined(OSG_FIXED_MFIELDSYNC)
void VistaOpenSGTextForegroundBase::executeSyncImpl(
    VistaOpenSGTextForegroundBase* pOther, const BitVector& whichField) {
  Inherited::executeSyncImpl(pOther, whichField);

  if (FieldBits::NoField != (TextsFieldMask & whichField))
    m_mfTexts.syncWith(pOther->m_mfTexts);
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

DataType FieldDataTraits<VistaOpenSGTextForegroundPtr>::_type(
    "VistaOpenSGTextForegroundPtr", "ForegroundPtr");

/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning(disable : 177)
#endif
