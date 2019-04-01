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

#ifndef _VISTAGLTEXTURE_H_
#define _VISTAGLTEXTURE_H_

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaAtomicCounter.h>

#include <VistaAspects/VistaSharedCore.h>
#include <VistaAspects/VistaSharedCoreOwner.h>

#ifdef WIN32
	#include <Windows.h>
#endif
#include <GL/gl.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaTextureCore;
class VistaImage;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaGLTexture : public IVistaSharedCoreOwner
{
public:
	//class VISTAKERNELAPI BindInfo
	//{
	//public:
	//	BindInfo( const GLenum eTextureUnit );
	//	BindInfo( const BindInfo& oCopy );
	//	GLenum GetTexureUnit() const;
	//	void SetTextureUnit( const GLenum eTextureUnit ) const;

	//	VistaGLTexture* GetTexture() const;
	//	void SetTexture( VistaGLTexture* pTexture );

	//	void Bind();
	//	void Release();
	//private:
	//	VistaGLTexture* m_pTexture;
	//	GLenum m_eTextureUnit;
	//};
	//class VISTAKERNELAPI BindScope
	//{
	//	BindScope( VistaGLTexture* pTexture, const GLenum eTextureUnit );
	//	BindScope( const BindInfo& oBind );
	//	~BindScope();
	//private:
	//	BindInfo m_oBind;
	//};
public:
	VistaGLTexture( DataHandling eDataHandlingMode = DH_ALWAYS_SHARE );
	VistaGLTexture( const VistaGLTexture& oOther );
	virtual ~VistaGLTexture();

	VistaGLTexture& operator= ( const VistaGLTexture& oOther );

	bool SwapTextures( VistaGLTexture& oOther );

	const IVistaTextureCore* GetCore() const;
	IVistaTextureCore* GetCoreForWriting();

	// @todo: compressed formats?
	// @TODO. env mode

	bool GetIsValid() const;
	
	/**
	 * @return texture target, i.e. GL_TEXTURE_1D, GL_TEXTURE_2D, etc.
	 */
	GLenum GetTextureTarget() const;
	/**
	 * @return internal data format, e.g. GL_RGBA, GL_RGBA16, GL_R16F, GL_R8I, etc.
	 */
	GLenum GetInternalFormat() const;

	VistaImage LoadTextureDataToImage();

	bool Set1DData( const int nWidth,
						const VistaType::byte* pData,
						const bool bGenMipmaps = true,
						const GLenum eInternalFormat = GL_RGB,
						const GLenum ePixelFormat = GL_RGB,
						const GLenum eDataFormat = GL_UNSIGNED_BYTE );
	bool Set2DData( const int nWidth, const int nHeight,
						const VistaType::byte* pData,
						const bool bGenMipmaps = true,
						const GLenum eInternalFormat = GL_RGB,
						const GLenum ePixelFormat = GL_RGB,
						const GLenum eDataFormat = GL_UNSIGNED_BYTE,
						const GLuint nMultiSampleNum = 0 );
	bool Set3DData( const int nWidth, const int nHeight, const int nDepth,
						const VistaType::byte* pData,
						const bool bGenMipmaps = true,
						const GLenum eInternalFormat = GL_RGB,
						const GLenum ePixelFormat = GL_RGB,
						const GLenum eDataFormat = GL_UNSIGNED_BYTE );
	bool SetData( const VistaImage& oImage, const bool bGenMipmaps = true, const GLenum eInternalFormat = GL_RGB );
	// @IMAGETODO: option to query image that has set the data?

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetDepth() const;

	bool GetHasMipmaps() const;
	bool CreateMipmaps();

	GLenum GetMinFilter() const;
	void SetMinFilter( const GLenum eMinFilterMode );
	GLenum GetMagFilter() const;
	void SetMagFilter( const GLenum eMagFilterMode );

	GLenum GetWrapS()const ;
	void SetWrapS( const GLenum eWrappingMode );
	GLenum GetWrapT()const ;
	void SetWrapT( const GLenum eWrappingMode );
	GLenum GetWrapR()const ;
	void SetWrapR( const GLenum eWrappingMode );

	float GetAnisotropy() const;
	void SetAnisotropy( const float nAnisotropy );

	float GetLodBias() const;
	void SetLodBias( const float nBias );

	bool GetIsMultisample() const;
	int GetNumMultisamples() const;

	/**
	 * G/Sets texture env mode, i.e. GL_MODULATE, GL_REPLACE, GL_BLEND, GL_DECAL
	 */
	GLenum GetTextureEnvMode() const;
	void SetTextureEnvMode( GLenum eMode );
		
	bool Bind()const ;
	bool Bind( const GLenum eTextureUnit )const ;
	bool Release()const ;
	bool Release( const GLenum eTextureUnit )const ;

	GLuint GetGLId() const;
};


class VISTAKERNELAPI IVistaTextureCore : public IVistaSharedCore
{
public:
	virtual bool GetIsValid() const = 0;
	
	/**
	 * @return texture target, i.e. GL_TEXTURE_1D, GL_TEXTURE_2D, etc.
	 */
	virtual GLenum GetTextureTarget() const = 0;
	/**
	 * @return internal data format, e.g. GL_RGBA, GL_RGBA16, GL_R16F, GL_R8I, etc.
	 */
	virtual GLenum GetInternalFormat() const = 0;

	virtual VistaImage LoadTextureDataToImage() = 0;

	virtual bool Set1DData( const int nWidth,
						const VistaType::byte* pData,
						const bool bGenMipmaps = true,
						const GLenum eInternalFormat = GL_RGBA,
						const GLenum ePixelFormat = GL_RGBA,
						const GLenum eDataFormat = GL_UNSIGNED_BYTE ) = 0;
	virtual bool Set2DData( const int nWidth, const int nHeight,
						const VistaType::byte* pData,
						const bool bGenMipmaps = true,
						const GLenum eInternalFormat = GL_RGBA,
						const GLenum ePixelFormat = GL_RGBA,
						const GLenum eDataFormat = GL_UNSIGNED_BYTE,
						const int nMultiSampleNum = 0 ) = 0;
	virtual bool Set3DData( const int nWidth, const int nHeight, const int nDepth,
						const VistaType::byte* pData,
						const bool bGenMipmaps = true,
						const GLenum eInternalFormat = GL_RGBA,
						const GLenum ePixelFormat = GL_RGBA,
						const GLenum eDataFormat = GL_UNSIGNED_BYTE ) = 0;
	virtual bool SetData( const VistaImage& oImage, const bool bGenMipmaps = true, const GLenum eInternalFormat = GL_RGBA ) = 0;
	// @IMAGETODO: option to query image that has set the data?

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;
	virtual unsigned int GetDepth() const = 0;

	virtual bool GetHasMipmaps() const = 0;
	virtual bool CreateMipmaps() = 0;

	virtual GLenum GetMinFilter() const = 0;
	virtual void SetMinFilter( const GLenum eMinFilterMode ) = 0;
	virtual GLenum GetMagFilter() const = 0;
	virtual void SetMagFilter( const GLenum eMagFilterMode ) = 0;

	virtual GLenum GetWrapS() const = 0;
	virtual void SetWrapS( const GLenum eWrappingMode ) = 0;
	virtual GLenum GetWrapT() const = 0;
	virtual void SetWrapT( const GLenum eWrappingMode ) = 0;
	virtual GLenum GetWrapR() const = 0;
	virtual void SetWrapR( const GLenum eWrappingMode ) = 0;

	virtual float GetAnisotropy() const = 0;
	virtual void SetAnisotropy( const float nAnisotropy ) = 0;

	virtual float GetLodBias() const = 0;
	virtual void SetLodBias( const float nBias ) = 0;

	virtual int GetNumMultisamples() const = 0;
		
	virtual bool Bind() const = 0;
	virtual bool Bind( const GLenum eTextureUnit ) const = 0;
	virtual bool Release() const = 0;
	virtual bool Release( const GLenum eTextureUnit ) const = 0;

	virtual GLenum GetTextureEnvMode() const = 0;
	virtual void SetTextureEnvMode( GLenum eMode  ) = 0;

	virtual GLuint GetGLId() const = 0;

protected:
	IVistaTextureCore() {};
	virtual ~IVistaTextureCore() {};

	virtual IVistaTextureCore* Clone() const = 0;
};


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif /* _VISTAGLTEXTURE_H_ */
