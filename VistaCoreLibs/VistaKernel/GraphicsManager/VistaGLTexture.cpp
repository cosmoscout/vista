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

#include "VistaGLTexture.h"

#include "VistaImageAndTextureFactory.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaGLTexture::VistaGLTexture(DataHandling eDataHandlingMode /*= DH_COPY_ON_WRITE */)
    : IVistaSharedCoreOwner(IVistaImageAndTextureCoreFactory::GetSingleton()->CreateTextureCore(),
          eDataHandlingMode) {
}

VistaGLTexture::VistaGLTexture(const VistaGLTexture& oOther)
    : IVistaSharedCoreOwner(oOther) {
}

VistaGLTexture::~VistaGLTexture() {
}

VistaGLTexture& VistaGLTexture::operator=(const VistaGLTexture& oOther) {
  IVistaSharedCoreOwner::operator=(oOther);
  return (*this);
}

const IVistaTextureCore* VistaGLTexture::GetCore() const {
  return static_cast<const IVistaTextureCore*>(IVistaSharedCoreOwner::GetCore());
}

IVistaTextureCore* VistaGLTexture::GetCoreForWriting() {
  return static_cast<IVistaTextureCore*>(IVistaSharedCoreOwner::GetCoreForWriting());
}

bool VistaGLTexture::GetIsValid() const {
  return GetCore()->GetIsValid();
}

GLenum VistaGLTexture::GetTextureTarget() const {
  return GetCore()->GetTextureTarget();
}

GLenum VistaGLTexture::GetInternalFormat() const {
  return GetCore()->GetInternalFormat();
}

VistaImage VistaGLTexture::LoadTextureDataToImage() {
  // @todo: writing or not?
  return GetCoreForWriting()->LoadTextureDataToImage();
}

bool VistaGLTexture::Set1DData(const int nWidth, const VistaType::byte* pData,
    const bool bGenMipmaps /*= true*/, const GLenum eInternalFormat /*= GL_RGBA*/,
    const GLenum ePixelFormat /*= GL_RGBA*/, const GLenum eDataFormat /*= GL_UNSIGNED_BYTE */) {
  return GetCoreForWriting()->Set1DData(
      nWidth, pData, bGenMipmaps, eInternalFormat, ePixelFormat, eDataFormat);
}

bool VistaGLTexture::Set2DData(const int nWidth, const int nHeight, const VistaType::byte* pData,
    const bool bGenMipmaps /*= true*/, const GLenum eInternalFormat /*= GL_RGBA*/,
    const GLenum ePixelFormat /*= GL_RGBA*/, const GLenum eDataFormat /*= GL_UNSIGNED_BYTE */,
    const GLuint nMultiSampleNum) {
  return GetCoreForWriting()->Set2DData(
      nWidth, nHeight, pData, bGenMipmaps, eInternalFormat, ePixelFormat, eDataFormat);
}

bool VistaGLTexture::Set3DData(const int nWidth, const int nHeight, const int nDepth,
    const VistaType::byte* pData, const bool bGenMipmaps /*= true*/,
    const GLenum eInternalFormat /*= GL_RGBA*/, const GLenum ePixelFormat /*= GL_RGBA*/,
    const GLenum eDataFormat /*= GL_UNSIGNED_BYTE */) {
  return GetCoreForWriting()->Set3DData(
      nWidth, nHeight, nWidth, pData, bGenMipmaps, eInternalFormat, ePixelFormat, eDataFormat);
}

bool VistaGLTexture::SetData(
    const VistaImage& oImage, const bool bGenMipmaps, const GLenum eInternalFormat) {
  return GetCoreForWriting()->SetData(oImage, bGenMipmaps, eInternalFormat);
}

unsigned int VistaGLTexture::GetWidth() const {
  return GetCore()->GetWidth();
}

unsigned int VistaGLTexture::GetHeight() const {
  return GetCore()->GetHeight();
}

unsigned int VistaGLTexture::GetDepth() const {
  return GetCore()->GetDepth();
}

bool VistaGLTexture::GetHasMipmaps() const {
  return GetCore()->GetHasMipmaps();
}

bool VistaGLTexture::CreateMipmaps() {
  return GetCoreForWriting()->CreateMipmaps();
}

GLenum VistaGLTexture::GetMinFilter() const {
  return GetCore()->GetMinFilter();
}

void VistaGLTexture::SetMinFilter(const GLenum eMinFilterMode) {
  return GetCoreForWriting()->SetMinFilter(eMinFilterMode);
}

GLenum VistaGLTexture::GetMagFilter() const {
  return GetCore()->GetMagFilter();
}

void VistaGLTexture::SetMagFilter(const GLenum eMagFilterMode) {
  return GetCoreForWriting()->SetMagFilter(eMagFilterMode);
}

GLenum VistaGLTexture::GetWrapS() const {
  return GetCore()->GetWrapR();
}

void VistaGLTexture::SetWrapS(const GLenum eWrappingMode) {
  return GetCoreForWriting()->SetWrapS(eWrappingMode);
}

GLenum VistaGLTexture::GetWrapT() const {
  return GetCore()->GetWrapT();
}

void VistaGLTexture::SetWrapT(const GLenum eWrappingMode) {
  return GetCoreForWriting()->SetWrapT(eWrappingMode);
}

GLenum VistaGLTexture::GetWrapR() const {
  return GetCore()->GetWrapR();
}

void VistaGLTexture::SetWrapR(const GLenum eWrappingMode) {
  return GetCoreForWriting()->SetWrapR(eWrappingMode);
}

float VistaGLTexture::GetAnisotropy() const {
  return GetCore()->GetAnisotropy();
}

void VistaGLTexture::SetAnisotropy(const float nAnisotropy) {
  return GetCoreForWriting()->SetAnisotropy(nAnisotropy);
}

float VistaGLTexture::GetLodBias() const {
  return GetCore()->GetLodBias();
}

void VistaGLTexture::SetLodBias(const float nBias) {
  return GetCoreForWriting()->SetLodBias(nBias);
}

bool VistaGLTexture::Bind() const {
  // @todo: writing
  return GetCore()->Bind();
}

bool VistaGLTexture::Bind(const GLenum eTextureUnit) const {
  // @todo: writing
  return GetCore()->Bind(eTextureUnit);
}

bool VistaGLTexture::Release() const {
  // @todo: writing
  return GetCore()->Release();
}

bool VistaGLTexture::Release(const GLenum eTextureUnit) const {
  // @todo: writing
  return GetCore()->Release(eTextureUnit);
}

GLuint VistaGLTexture::GetGLId() const {
  return GetCore()->GetGLId();
}

GLenum VistaGLTexture::GetTextureEnvMode() const {
  return GetCore()->GetTextureEnvMode();
}

void VistaGLTexture::SetTextureEnvMode(GLenum eMode) {
  GetCoreForWriting()->SetTextureEnvMode(eMode);
}

bool VistaGLTexture::SwapTextures(VistaGLTexture& oOther) {
  std::swap(m_pCore, oOther.m_pCore);
  return true;
}

bool VistaGLTexture::GetIsMultisample() const {
  return (GetCore()->GetNumMultisamples() > 0);
}

int VistaGLTexture::GetNumMultisamples() const {
  return GetCore()->GetNumMultisamples();
}

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
