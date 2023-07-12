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
// $Id: VistaAutoBuffer.h 31862 2012-08-31 22:54:08Z ingoassenmacher $

#ifndef _VISTAOPENSGIMAGEANDTEXTURECOREFACTORY_H_
#define _VISTAOPENSGIMAGEANDTEXTURECOREFACTORY_H_

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>

#include <VistaKernel/GraphicsManager/VistaGLTexture.h>
#include <VistaKernel/GraphicsManager/VistaImage.h>
#include <VistaKernel/GraphicsManager/VistaImageAndTextureFactory.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGTextureChunk.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaOpenSGImageAndTextureCoreFactory
    : public IVistaImageAndTextureCoreFactory {
 public:
  class ImageCore : public IVistaImageCore {
   public:
    ImageCore();
    virtual ~ImageCore();

    virtual bool GetIsValid() const;

    virtual bool LoadFromFile(const std::string& sFilename);
    virtual bool WriteToFile(const std::string& sFilename) const;

    virtual bool Set1DData(const int nWidth, VistaType::byte* pData, const GLenum ePixelFormat,
        const GLenum ePixelDataType, const bool bCopyData);
    virtual bool Set2DData(const int nWidth, const int nHeight, VistaType::byte* pData,
        const GLenum ePixelFormat, const GLenum ePixelDataType, const bool bCopyData);
    virtual bool Set3DData(const int nWidth, const int nHeight, const int nDepth,
        VistaType::byte* pData, const GLenum ePixelFormat, const GLenum ePixelDataType,
        const bool bCopyData);

    virtual unsigned int GetDimension() const;
    virtual unsigned int GetWidth() const;
    virtual unsigned int GetHeight() const;
    virtual unsigned int GetDepth() const;

    virtual unsigned int GetBytesPerPixel() const;
    virtual unsigned int GetComponentsPerPixel() const;
    virtual unsigned int GetBytesPerComponent() const;

    virtual bool GetHasAlphaChannel() const;
    virtual bool GetHasTransparency() const;
    virtual bool GetHasBinaryAlphaChannel() const;

    virtual bool Rescale(const unsigned int nNewWidth, const unsigned int nNewHeight);
    virtual bool Crop(const unsigned int nBottomLeftX, const unsigned int nBottomLeftY,
        const unsigned int nNewWidth, const unsigned int nNewHeight);

    virtual GLenum GetPixelFormat() const;
    virtual GLenum GetPixelDataType() const;

    virtual bool ReformatImage(const GLenum eNewPixelFormat, const GLenum eNewPixelDataType);

    virtual unsigned int GetDataSize() const;

    virtual const VistaType::byte* GetData() const;
    virtual VistaType::byte*       GetDataWrite();

    virtual IVistaImageCore* Clone() const;

    osg::ImagePtr GetOSGImage() const;

   private:
    osg::ImageRefPtr m_pImage;
  };

  class TextureCore : public IVistaTextureCore {
   public:
    TextureCore(osg::RenderAction* pRenderAction);
    virtual ~TextureCore();

    virtual bool GetIsValid() const;

    virtual GLenum GetTextureTarget() const;
    virtual GLenum GetInternalFormat() const;

    virtual VistaImage LoadTextureDataToImage();

    virtual bool Set1DData(const int nWidth, const VistaType::byte* pData,
        const bool bGenMipmaps = true, const GLenum eInternalFormat = GL_RGBA,
        const GLenum ePixelFormat = GL_RGBA, const GLenum eDataFormat = GL_UNSIGNED_BYTE);
    virtual bool Set2DData(const int nWidth, const int nHeight, const VistaType::byte* pData,
        const bool bGenMipmaps = true, const GLenum eInternalFormat = GL_RGBA,
        const GLenum ePixelFormat = GL_RGBA, const GLenum eDataFormat = GL_UNSIGNED_BYTE,
        const int nNumMultisamples = 0);
    virtual bool Set3DData(const int nWidth, const int nHeight, const int nDepth,
        const VistaType::byte* pData, const bool bGenMipmaps = true,
        const GLenum eInternalFormat = GL_RGBA, const GLenum ePixelFormat = GL_RGBA,
        const GLenum eDataFormat = GL_UNSIGNED_BYTE);
    virtual bool SetData(const VistaImage& oImage, const bool bGenMipmaps = true,
        const GLenum eInternalFormat = GL_RGBA);

    virtual unsigned int GetWidth() const;
    virtual unsigned int GetHeight() const;
    virtual unsigned int GetDepth() const;

    virtual bool GetHasMipmaps() const;
    virtual bool CreateMipmaps();

    virtual GLenum GetMinFilter() const;
    virtual void   SetMinFilter(const GLenum eMinFilterMode);
    virtual GLenum GetMagFilter() const;
    virtual void   SetMagFilter(const GLenum eMagFilterMode);

    virtual GLenum GetWrapS() const;
    virtual void   SetWrapS(const GLenum eWrappingMode);
    virtual GLenum GetWrapT() const;
    virtual void   SetWrapT(const GLenum eWrappingMode);
    virtual GLenum GetWrapR() const;
    virtual void   SetWrapR(const GLenum eWrappingMode);

    virtual float GetAnisotropy() const;
    virtual void  SetAnisotropy(const float nAnisotropy);

    virtual float GetLodBias() const;
    virtual void  SetLodBias(const float nBias);

    virtual int GetNumMultisamples() const;

    virtual bool Bind() const;
    virtual bool Bind(const GLenum eTextureUnit) const;
    virtual bool Release() const;
    virtual bool Release(const GLenum eTextureUnit) const;

    virtual GLenum GetTextureEnvMode() const;
    virtual void   SetTextureEnvMode(GLenum eMode);

    virtual GLuint GetGLId() const;

    virtual IVistaTextureCore* Clone() const;

    osg::TextureChunkPtr GetOSGTexture() const;

   protected:
    void ValidateChanges() const;

   private:
    osg::TextureChunkRefPtr m_pTexture;
    // we need the render action, in order to determine the window and
    // gl context for binding and retrieving the gl id
    osg::RenderAction* m_pDrawAction;
  };

 public:
  VistaOpenSGImageAndTextureCoreFactory(osg::RenderAction* pAction);
  ~VistaOpenSGImageAndTextureCoreFactory();

  virtual IVistaImageCore*   CreateImageCore();
  virtual IVistaTextureCore* CreateTextureCore();

 private:
  osg::RenderAction* m_pRenderAction;
};

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif /* _VISTAOPENSGIMAGEANDTEXTURECOREFACTORY_H_ */
