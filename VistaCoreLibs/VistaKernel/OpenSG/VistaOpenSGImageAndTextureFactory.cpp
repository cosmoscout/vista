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
// $Id: VistaAutoBuffer.cpp 31862 2012-08-31 22:54:08Z ingoassenmacher $

#include <GL/glew.h>

#include "VistaOpenSGImageAndTextureFactory.h"

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include "../GraphicsManager/VistaNativeGLImageAndTextureFactory.h"
#include "../Stuff/VistaKernelProfiling.h"
#include "VistaTools/VistaFileSystemFile.h"
#include <fstream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* VistaOpenSGVistaImageAndTextureCoreFactory                                 */
/*============================================================================*/
VistaOpenSGImageAndTextureCoreFactory::VistaOpenSGImageAndTextureCoreFactory(
    osg::RenderAction* pAction)
    : m_pRenderAction(pAction) {
}

VistaOpenSGImageAndTextureCoreFactory::~VistaOpenSGImageAndTextureCoreFactory() {
}

IVistaImageCore* VistaOpenSGImageAndTextureCoreFactory::CreateImageCore() {
  return new VistaOpenSGImageAndTextureCoreFactory::ImageCore();
}

IVistaTextureCore* VistaOpenSGImageAndTextureCoreFactory::CreateTextureCore() {
  // @IMGTODO
  // return new VistaOpenSGImageAndTextureCoreFactory::TextureCore( m_pRenderAction );
  return new VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore();
}

/*============================================================================*/
/* ImageCore                                                                  */
/*============================================================================*/

VistaOpenSGImageAndTextureCoreFactory::ImageCore::ImageCore()
    : m_pImage(osg::NullFC) {
}

VistaOpenSGImageAndTextureCoreFactory::ImageCore::~ImageCore() {
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetIsValid() const {
  return (m_pImage != osg::NullFC);
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::LoadFromFile(const std::string& sFilename) {
  VistaFileSystemFile oFile(sFilename);
  if (oFile.Exists() == false) {
    vstr::warnp() << "OpenSGImage::LoadFromFile() -- Image file \"" << sFilename
                  << "\" does not exist" << std::endl;
    return false;
  }
  osg::ImageRefPtr pLoadedImage;
  pLoadedImage = osg::Image::create();
  beginEditCP(pLoadedImage);
  if (pLoadedImage->read(sFilename.c_str()) == false) {
    vstr::warnp() << "OpenSGImage::LoadFromFile() -- Loading of file \"" << sFilename << "\" failed"
                  << std::endl;
    return false;
  }
  endEditCP(pLoadedImage);
  m_pImage = pLoadedImage;
  return true;
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::WriteToFile(
    const std::string& sFilename) const {
  if (m_pImage == osg::NullFC)
    return false;
  if (m_pImage->write(sFilename.c_str()) == false) {
    vstr::warnp() << "OpenSGImage::WriteToFile() -- Writing file \"" << sFilename << "\" failed"
                  << std::endl;
    return false;
  }
  return true;
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::Set1DData(const int nWidth,
    VistaType::byte* pData, const GLenum ePixelFormat, const GLenum ePixelDataType,
    const bool bCopyData) {
  osg::UInt32 nPixelFormat     = ePixelFormat;
  osg::UInt32 nComponentFormat = ePixelDataType;

  osg::ImageRefPtr pNewImage;
  pNewImage = osg::Image::create();
  beginEditCP(pNewImage);
  pNewImage->set(nPixelFormat, nWidth, 1, 1, 1, 1, 0.0, pData, nComponentFormat, true, 1);
  endEditCP(pNewImage);
  m_pImage = pNewImage;
  return true;
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::Set2DData(const int nWidth,
    const int nHeight, VistaType::byte* pData, const GLenum ePixelFormat,
    const GLenum ePixelDataType, const bool bCopyData) {
  osg::UInt32 nPixelFormat     = ePixelFormat;
  osg::UInt32 nComponentFormat = ePixelDataType;

  osg::ImageRefPtr pNewImage;
  pNewImage = osg::Image::create();
  beginEditCP(pNewImage);
  pNewImage->set(nPixelFormat, nWidth, nHeight, 1, 1, 1, 0.0, pData, nComponentFormat, true, 1);
  endEditCP(pNewImage);
  m_pImage = pNewImage;
  return true;
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::Set3DData(const int nWidth,
    const int nHeight, const int nDepth, VistaType::byte* pData, const GLenum ePixelFormat,
    const GLenum ePixelDataType, const bool bCopyData) {
  osg::UInt32 nPixelFormat     = ePixelFormat;
  osg::UInt32 nComponentFormat = ePixelDataType;

  osg::ImageRefPtr pNewImage;
  pNewImage = osg::Image::create();
  beginEditCP(pNewImage);
  pNewImage->set(
      nPixelFormat, nWidth, nHeight, nDepth, 1, 1, 0.0, pData, nComponentFormat, true, 1);
  endEditCP(pNewImage);
  m_pImage = pNewImage;
  return true;
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetWidth() const {
  if (m_pImage == osg::NullFC)
    return 0;
  return m_pImage->getWidth();
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetHeight() const {
  if (m_pImage == osg::NullFC)
    return 0;
  return m_pImage->getHeight();
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetDepth() const {
  if (m_pImage == osg::NullFC)
    return 0;
  return m_pImage->getDepth();
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetBytesPerPixel() const {
  VISTA_THROW_NOT_IMPLEMENTED;
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetComponentsPerPixel() const {
  VISTA_THROW_NOT_IMPLEMENTED;
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetBytesPerComponent() const {
  VISTA_THROW_NOT_IMPLEMENTED;
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetHasAlphaChannel() const {
  if (m_pImage == osg::NullFC)
    return false;
  return m_pImage->hasAlphaChannel();
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetHasTransparency() const {
  VISTA_THROW_NOT_IMPLEMENTED;
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetHasBinaryAlphaChannel() const {
  if (m_pImage == osg::NullFC)
    return false;
  return m_pImage->isAlphaBinary();
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::Rescale(
    const unsigned int nNewWidth, const unsigned int nNewHeight) {
  if (m_pImage == osg::NullFC)
    return false;
  return m_pImage->scale(nNewWidth, nNewHeight);
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::Crop(const unsigned int nBottomLeftX,
    const unsigned int nBottomLeftY, const unsigned int nNewWidth, const unsigned int nNewHeight) {
  if (m_pImage == osg::NullFC)
    return false;
  return m_pImage->subImage(nBottomLeftX, nBottomLeftY, 0, nNewWidth, nNewHeight, 0);
}

GLenum VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetPixelFormat() const {
  if (m_pImage == osg::NullFC)
    return GL_NONE;
  return m_pImage->getPixelFormat();
}

GLenum VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetPixelDataType() const {
  if (m_pImage == osg::NullFC)
    return GL_NONE;
  return m_pImage->getDataType();
}

bool VistaOpenSGImageAndTextureCoreFactory::ImageCore::ReformatImage(
    const GLenum eNewFormat, const GLenum ePixelDataType) {
  if (m_pImage == osg::NullFC)
    return false;
  osg::Image::PixelFormat ePix = (osg::Image::PixelFormat)eNewFormat;

  return (m_pImage->reformat(ePix) && m_pImage->convertDataTypeTo(ePixelDataType));
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetDataSize() const {
  VISTA_THROW_NOT_IMPLEMENTED;
}

const VistaType::byte* VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetData() const {
  return (const VistaType::byte*)m_pImage->getData();
}

VistaType::byte* VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetDataWrite() {
  return (VistaType::byte*)m_pImage->getData();
}

IVistaImageCore* VistaOpenSGImageAndTextureCoreFactory::ImageCore::Clone() const {
  ImageCore* pCore = new ImageCore();
  // @IMGTODO: verify this is a deep copy
  pCore->m_pImage        = osg::Image::create();
  *pCore->m_pImage.get() = *m_pImage.get();
  return pCore;
}

osg::ImagePtr VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetOSGImage() const {
  return m_pImage;
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::ImageCore::GetDimension() const {
  if (m_pImage == osg::NullFC)
    return 0;
  return m_pImage->getDimension();
}

/*============================================================================*/
/* TextureCore                                                                */
/*============================================================================*/

VistaOpenSGImageAndTextureCoreFactory::TextureCore::TextureCore(osg::RenderAction* pAction)
    : IVistaTextureCore()
    , m_pTexture(osg::TextureChunk::create())
    , m_pDrawAction(pAction) {
}
VistaOpenSGImageAndTextureCoreFactory::TextureCore::~TextureCore() {
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetIsValid() const {
  return (m_pTexture->getImage() != osg::NullFC && m_pTexture->getImage()->getDimension() > 0);
}

GLenum VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetTextureTarget() const {
  osg::ImagePtr pImage = m_pTexture->getImage();
  if (pImage == osg::NullFC || pImage->getDimension() == 0)
    return GL_NONE;
  if (pImage->getSideCount() > 1) {
    return GL_TEXTURE_CUBE_MAP;
  } else if (pImage->getDepth() > 1)
    return GL_TEXTURE_3D;
  else if (pImage->getHeight() > 1)
    return GL_TEXTURE_2D;
  else
    return GL_TEXTURE_1D;
}

GLenum VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetInternalFormat() const {
  VISTA_THROW_NOT_IMPLEMENTED;
}

VistaImage VistaOpenSGImageAndTextureCoreFactory::TextureCore::LoadTextureDataToImage() {
  VISTA_THROW_NOT_IMPLEMENTED;
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::Set1DData(const int nWidth,
    const VistaType::byte* pData, const bool bGenMipmaps /*= true*/,
    const GLenum eInternalFormat /*= GL_RGBA*/, const GLenum ePixelFormat /*= GL_RGBA*/,
    const GLenum eDataFormat /*= GL_UNSIGNED_BYTE */) {
  VISTA_THROW_NOT_IMPLEMENTED;
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::Set2DData(const int nWidth,
    const int nHeight, const VistaType::byte* pData, const bool bGenMipmaps /*= true*/,
    const GLenum eInternalFormat /*= GL_RGBA*/, const GLenum ePixelFormat /*= GL_RGBA*/,
    const GLenum eDataFormat /*= GL_UNSIGNED_BYTE */, const int nNumMultisamples) {
  // @IMGTODO: multisample?

  osg::UInt32 nPixelFormat     = ePixelFormat;
  osg::UInt32 nComponentFormat = eDataFormat;

  osg::ImagePtr pNewImage = osg::Image::create();
  beginEditCP(pNewImage);
  pNewImage->set(nPixelFormat, nWidth, nHeight, 1, 1, 1, 0.0, pData, nComponentFormat, true, 1);
  endEditCP(pNewImage);

  beginEditCP(m_pTexture);
  if (bGenMipmaps) {
    m_pTexture->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
  };
  m_pTexture->setImage(pNewImage);
  endEditCP(m_pTexture);

  ValidateChanges();

  return true;
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::Set3DData(const int nWidth,
    const int nHeight, const int nDepth, const VistaType::byte* pData,
    const bool bGenMipmaps /*= true*/, const GLenum eInternalFormat /*= GL_RGBA*/,
    const GLenum ePixelFormat /*= GL_RGBA*/, const GLenum eDataFormat /*= GL_UNSIGNED_BYTE */) {
  VISTA_THROW_NOT_IMPLEMENTED;
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::SetData(
    const VistaImage& oImage, const bool bGenMipmaps, const GLenum eInternalFormat) {
  if (oImage.GetIsValid() == false)
    return false;
  const ImageCore* pImageCore = dynamic_cast<const ImageCore*>(oImage.GetCore());
  osg::ImagePtr    pImage     = pImageCore->GetOSGImage();
  assert(pImageCore);
  beginEditCP(m_pTexture);
  if (bGenMipmaps) {
    m_pTexture->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
  };
  m_pTexture->setInternalFormat(eInternalFormat);
  m_pTexture->setImage(pImage);
  endEditCP(m_pTexture);

  ValidateChanges();

  return true;
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetWidth() const {
  osg::ImagePtr pImage = m_pTexture->getImage();
  if (pImage == osg::NullFC)
    return 0;
  return pImage->getWidth();
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetHeight() const {
  osg::ImagePtr pImage = m_pTexture->getImage();
  if (pImage == osg::NullFC)
    return 0;
  return pImage->getHeight();
}

unsigned int VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetDepth() const {
  osg::ImagePtr pImage = m_pTexture->getImage();
  if (pImage == osg::NullFC)
    return 0;
  return pImage->getDepth();
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetHasMipmaps() const {
  VISTA_THROW_NOT_IMPLEMENTED;
  // osg::ImagePtr pImage = m_pTexture->getImage();
  // if( pImage == osg::NullFC )
  //	return false;
  // return pImage->get
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::CreateMipmaps() {
  beginEditCP(m_pTexture, osg::TextureChunk::MinFilterFieldId);
  m_pTexture->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
  endEditCP(m_pTexture, osg::TextureChunk::MinFilterFieldId);

  ValidateChanges();

  return true;
}

GLenum VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetMinFilter() const {
  return m_pTexture->getMinFilter();
}

void VistaOpenSGImageAndTextureCoreFactory::TextureCore::SetMinFilter(const GLenum eMinFilterMode) {
  beginEditCP(m_pTexture, osg::TextureChunk::MinFilterFieldId);
  m_pTexture->setMinFilter(eMinFilterMode);
  endEditCP(m_pTexture, osg::TextureChunk::MinFilterFieldId);
  ValidateChanges();
}

GLenum VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetMagFilter() const {
  return m_pTexture->getMagFilter();
}

void VistaOpenSGImageAndTextureCoreFactory::TextureCore::SetMagFilter(const GLenum eMagFilterMode) {
  beginEditCP(m_pTexture, osg::TextureChunk::MagFilterFieldId);
  m_pTexture->setMagFilter(eMagFilterMode);
  endEditCP(m_pTexture, osg::TextureChunk::MagFilterFieldId);
  ValidateChanges();
}

GLenum VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetWrapS() const {
  return m_pTexture->getWrapS();
  ;
}

void VistaOpenSGImageAndTextureCoreFactory::TextureCore::SetWrapS(const GLenum eWrappingMode) {
  beginEditCP(m_pTexture, osg::TextureChunk::WrapSFieldId);
  m_pTexture->setWrapS(eWrappingMode);
  endEditCP(m_pTexture, osg::TextureChunk::WrapSFieldId);
  ValidateChanges();
}

GLenum VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetWrapT() const {
  return m_pTexture->getWrapT();
}

void VistaOpenSGImageAndTextureCoreFactory::TextureCore::SetWrapT(const GLenum eWrappingMode) {
  beginEditCP(m_pTexture, osg::TextureChunk::WrapTFieldId);
  m_pTexture->setWrapT(eWrappingMode);
  endEditCP(m_pTexture, osg::TextureChunk::WrapTFieldId);
  ValidateChanges();
}

GLenum VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetWrapR() const {
  return m_pTexture->getWrapR();
}

void VistaOpenSGImageAndTextureCoreFactory::TextureCore::SetWrapR(const GLenum eWrappingMode) {
  beginEditCP(m_pTexture, osg::TextureChunk::WrapRFieldId);
  m_pTexture->setWrapR(eWrappingMode);
  endEditCP(m_pTexture, osg::TextureChunk::WrapRFieldId);
  ValidateChanges();
}

float VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetAnisotropy() const {
  return m_pTexture->getAnisotropy();
}

void VistaOpenSGImageAndTextureCoreFactory::TextureCore::SetAnisotropy(const float nAnisotropy) {
  beginEditCP(m_pTexture, osg::TextureChunk::AnisotropyFieldId);
  m_pTexture->setAnisotropy(nAnisotropy);
  endEditCP(m_pTexture, osg::TextureChunk::AnisotropyFieldId);
  ValidateChanges();
}

float VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetLodBias() const {
  return m_pTexture->getLodBias();
}

void VistaOpenSGImageAndTextureCoreFactory::TextureCore::SetLodBias(const float nBias) {
  beginEditCP(m_pTexture, osg::TextureChunk::LodBiasFieldId);
  m_pTexture->setLodBias(nBias);
  endEditCP(m_pTexture, osg::TextureChunk::LodBiasFieldId);
  ValidateChanges();
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::Bind() const {
  GLenum eTarget = GetTextureTarget();
  if (eTarget == GL_NONE)
    return false;

  // ValidateChanges();

  m_pTexture->activate(m_pDrawAction);
  // glBindTexture( GetTextureTarget(), m_pWindow->getGLObjectId( m_pTexture->getGLId() ) );
  // glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_pTexture->getEnvMode() );

  return true;
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::Bind(const GLenum eTextureUnit) const {
  // glActiveTexture( eTextureUnit );
  // return Bind();
  GLenum eTarget = GetTextureTarget();
  if (eTarget == GL_NONE)
    return false;

  // ValidateChanges();

  m_pTexture->activate(m_pDrawAction, eTextureUnit - GL_TEXTURE0);

  return true;
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::Release() const {
  GLenum eTarget = GetTextureTarget();
  if (eTarget == GL_NONE)
    return false;
  // glBindTexture( eTarget, 0 );
  m_pTexture->deactivate(m_pDrawAction);
  return true;
}

bool VistaOpenSGImageAndTextureCoreFactory::TextureCore::Release(const GLenum eTextureUnit) const {
  GLenum eTarget = GetTextureTarget();
  if (eTarget == GL_NONE)
    return false;
  // glActiveTexture( eTextureUnit );
  // glBindTexture( eTarget, 0 );
  m_pTexture->deactivate(m_pDrawAction, eTextureUnit - GL_TEXTURE0);
  return true;
}

GLuint VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetGLId() const {
  return m_pDrawAction->getWindow()->getGLObjectId(m_pTexture->getGLId());
}

IVistaTextureCore* VistaOpenSGImageAndTextureCoreFactory::TextureCore::Clone() const {
  VISTA_THROW_NOT_IMPLEMENTED;
}

osg::TextureChunkPtr VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetOSGTexture() const {
  return m_pTexture;
}

GLenum VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetTextureEnvMode() const {
  return m_pTexture->getEnvMode();
}

void VistaOpenSGImageAndTextureCoreFactory::TextureCore::SetTextureEnvMode(GLenum eMode) {
  beginEditCP(m_pTexture, osg::TextureChunk::EnvModeFieldId);
  m_pTexture->setEnvMode(eMode);
}

void VistaOpenSGImageAndTextureCoreFactory::TextureCore::ValidateChanges() const {
  m_pDrawAction->getWindow()->validateGLObject(m_pTexture->getGLId());
}

int VistaOpenSGImageAndTextureCoreFactory::TextureCore::GetNumMultisamples() const {
  return 0; // @IMGTODO: multisampling possible?
}
