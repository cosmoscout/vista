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

#include "VistaNativeGLImageAndTextureFactory.h"

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include "VistaAspects/VistaAspectsUtils.h"
#include "VistaTools/VistaFileSystemFile.h"

#include <fstream>
#include <iosfwd>
#include <cstring>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

namespace
{
	#ifdef DEBUG
	#define CHECKFORGLERROR( sFuncName ) \
	{ \
		GLuint nError = glGetError(); \
		if( nError != GL_NO_ERROR ) \
		{ \
		vstr::warnp() << "[VistaNativeGLTexture::" << sFuncName << "] gl error at line "  << __LINE__  << "\n"; \
			vstr::warn() << vstr::singleindent << gluErrorString( nError ) << std::endl; \
		} \
	}
	#else
	#define CHECKFORGLERROR( sMessage )
	#endif

	VistaType::byte* LoadImageFromTgaStream( std::istream& oStream, int& nWidth, int& nHeight, int& nChannels )
	{
		// read in TGA header data - bug out, if we encounter an error
		if (oStream.get() != 0)	// id length
			return NULL;
		if (oStream.get() != 0)	// color map type
			return NULL;
		if (oStream.get() != 2)	// data type (=2)
			return NULL;
		for (int i=0; i<5; ++i)		// some stuff...
		{
			if (oStream.get() != 0)
				return NULL;
		}
		for (int i=0; i<4; ++i)		// ignore image origin
			oStream.get();

		// retrieve image dimensions
		nWidth = oStream.get();
		nWidth += ((int)oStream.get()) << 8;

		nHeight = oStream.get();
		nHeight += ((int)oStream.get()) << 8;

		nChannels  = oStream.get() / 8;	// color depth
		if (nChannels  != 3 && nChannels  != 4)
			return NULL;

		oStream.get();		// image descriptor

		if (oStream.eof())			
			return NULL;

		// alright, let's allocate some data...
		unsigned char *pData = new unsigned char[nWidth * nHeight * nChannels ];

		// open mouth, close eyes...
		oStream.read((char *)pData, nWidth * nHeight * nChannels );

		if (oStream.eof())
		{
			vstr::warnp() << " [LoadImageFromTgaStream] - ERROR - unexpected end of file..." << std::endl;
			delete [] pData;
			return NULL;
		}

		// as data was in BGR(A) format, swizzle it around to get RGB(A)...
		int iPixelCount = nWidth * nHeight;
		unsigned char *pPos = pData;
		for (int i=0; i<iPixelCount; ++i)
		{
			std::swap( pPos[0], pPos[2] );
			pPos += nChannels ;
		}

		// that's all, folks
		return pData;
	}

	bool WriteTGAImageToStream( std::ostream& oStream, int nWidth, int nHeight, int nChannels, VistaType::byte* pData )
	{
		oStream.put( 0 ); // id length
		oStream.put( 0 ); // no color
		oStream.put( 2 ); // data type = uncompressed tru-color
		for( int i = 0; i < 5; ++i )
			oStream.put( 0 ); // empty color map		

		for( int i = 0; i < 4; ++i )
			oStream.put( 0 ); // zero origin

		// write size
		oStream.put( (char)( nWidth % 8 ) );
		oStream.put( (char)( nWidth / 8 ) );
		oStream.put( (char)( nHeight % 8 ) );
		oStream.put( (char)( nHeight / 8 ) );
		
		// write color depth
		oStream.put( (char)( nChannels ) );
		// write last image descriptor
		oStream.put( 0 );

		// write data
		int nNumPixels = nWidth * nHeight;
		VistaType::byte* pDataIter = pData;
		for( int i = 0; i < nNumPixels; ++i, pDataIter += nChannels )
		{
			oStream.put( pDataIter[2] );
			oStream.put( pDataIter[1] );
			oStream.put( pDataIter[0] );
			if( nChannels == 4 )
				oStream.put( pDataIter[3] );
		}

		return true;
	}
};

/*============================================================================*/
/* VistaNativeOpenGLVistaImageAndTextureCoreFactory                           */
/*============================================================================*/
VistaNativeOpenGLImageAndTextureCoreFactory::VistaNativeOpenGLImageAndTextureCoreFactory()
{

}

VistaNativeOpenGLImageAndTextureCoreFactory::~VistaNativeOpenGLImageAndTextureCoreFactory()
{

}


VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore* VistaNativeOpenGLImageAndTextureCoreFactory::CreateImageCore()
{
	return new VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore();
}

VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore* VistaNativeOpenGLImageAndTextureCoreFactory::CreateTextureCore()
{
	return new VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore();
}


/*============================================================================*/
/* ImageCore                                                                  */
/*============================================================================*/


VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::ImageCore()
: m_pData( NULL )
, m_bOwnData( false )
, m_nHeight( 0 )
, m_nWidth( 0 )
, m_nDepth( 0 )
, m_nDimension( 0 )
, m_ePixelFormat( GL_NONE )
, m_ePixelDataType( GL_NONE )
{
}

VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::~ImageCore()
{
	if( m_bOwnData )
	{
		delete [] m_pData;
	}
}


bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetIsValid() const
{
	return m_pData != NULL;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::LoadFromFile( const std::string& sFilename )
{
	std::size_t nDotPos = sFilename.rfind( '.' );
	if( nDotPos == std::string::npos )
	{
		vstr::warnp() << "VistaNativeOpenGL::LoadFromFile() -- filename \"" << sFilename << "\" has no extension" << std::endl;
		return false;
	}
	std::string sExtension = sFilename.substr( nDotPos );
	if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sExtension, ".tga" ) == false )
	{
		vstr::warnp() << "VistaNativeOpenGL::LoadFromFile() -- cannot read file \"" << sFilename << "\" - only TGA format is supported" << std::endl;
		return false;
	}

	std::ifstream oFile( sFilename.c_str(), std::ios::binary );
	if( oFile.is_open() == false )
	{
		vstr::warnp() << "VistaNativeOpenGL::LoadFromFile() -- could not open file \"" << sFilename << "\"" << std::endl;
		return false;
	}
	int nWidth, nHeight, nNumComponents;
	VistaType::byte* pData = LoadImageFromTgaStream( oFile, nWidth, nHeight, nNumComponents );
	if( pData == NULL )
	{
		vstr::warnp() << "VistaNativeOpenGL::LoadFromFile() -- reading data from file \"" << sFilename << "\" failed" << std::endl;
		return false;
	}

	if( m_bOwnData )
	{
		delete [] m_pData;
	}
	m_pData = pData;
	m_bOwnData = true;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nDepth = 0;
	m_nDimension = 2;
	m_ePixelDataType = GL_UNSIGNED_BYTE;
	if( nNumComponents == 3 )
		m_ePixelFormat = GL_RGB;
	else
		m_ePixelFormat = GL_RGBA;

	return true;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::WriteToFile( const std::string& sFilename ) const
{
	if( GetIsValid() == false )
	{
		vstr::warnp() << "VistaNativeOpenGL::WriteToFile() -- file \"" << sFilename
					<< "\" cannot be written - image is invalid" << std::endl;
	}

	int nChannels = 0;
	switch( m_ePixelFormat )
	{
		case GL_RGB:
			nChannels = 3;
			break;
		case GL_RGBA:
			nChannels = 4;
			break;
		default:
			vstr::warnp() << "VistaNativeOpenGL::WriteToFile() -- file \"" << sFilename
					<< "\" cannot be written - image has wrong format (requied GL_RGB or GL_RGBA)" << std::endl;
			return false;
	}

	std::string sActualFilename = sFilename;
	std::size_t nDotPos = sFilename.rfind( '.' );
	if( nDotPos == std::string::npos )
	{
		sActualFilename = sFilename + ".tga";
	}
	else
	{
		std::string sExtension = sFilename.substr( nDotPos );
		if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sExtension, ".tga" ) == false )
		{
			sActualFilename = sFilename.substr( 0, nDotPos ) + ".tga";
			vstr::warnp() << "VistaNativeOpenGL::WriteToFile() -- file \"" << sFilename
						<< "\" - only TGA is supported, writing to \"" << sActualFilename << "\" instead" << std::endl;
		}
	}
	VistaFileSystemFile oFile( sActualFilename );
	if( oFile.Exists() == false && oFile.CreateWithParentDirectories() == false )
	{
		vstr::warnp() << "VistaNativeOpenGL::WriteToFile() -- cannot create file \"" << sActualFilename << "\"" << std::endl;
		return false;
	}

	std::ofstream oFileStream( sActualFilename.c_str(), std::ios::binary );
	if( oFileStream.is_open() == false )
	{
		vstr::warnp() << "VistaNativeOpenGL::WriteToFile() -- could not open file \"" << sActualFilename << "\"" << std::endl;
		return false;
	}
	
	return WriteTGAImageToStream( oFileStream, m_nWidth, m_nHeight, nChannels, m_pData );
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::Set1DData( const int nWidth,
																			VistaType::byte* pData,
																			const GLenum ePixelFormat,
																			const GLenum ePixelDataType,
																			const bool bCopyData )
{
	unsigned int nPixelSize = VistaImage::GetNumberOfComponentsForPixelFormat( ePixelFormat )
						* VistaImage::GetNumberOfBytesForPixelDataType( ePixelDataType );

	if( nPixelSize == 0 )
	{
		vstr::warnp() << "VistaNativeOpenGL::SetData() -- invalid format specification" << std::endl;
		return false;
	}

	if( m_pData && m_bOwnData )
	{
		delete [] m_pData;
	}

	unsigned int nDataSize = nWidth * nPixelSize;
	if( bCopyData || pData == NULL )
	{
		m_pData = new VistaType::byte[nDataSize];
		if( m_pData )
			memcpy( m_pData, pData, nDataSize );
		else
			memset( m_pData, 0, nDataSize );
		m_bOwnData = true;
	}
	else
	{
		m_pData = pData;
		m_bOwnData = false;
	}

	m_ePixelFormat = ePixelFormat;
	m_ePixelDataType = ePixelDataType;
	m_nWidth = nWidth;
	m_nHeight = 1;
	m_nDepth = 1;
	m_nDimension = 1;

	return true;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::Set2DData( const int nWidth, const int nHeight,
																			VistaType::byte* pData,
																			const GLenum ePixelFormat,
																			const GLenum ePixelDataType,
																			const bool bCopyData )
{
	unsigned int nPixelSize = VistaImage::GetNumberOfComponentsForPixelFormat( ePixelFormat )
						* VistaImage::GetNumberOfBytesForPixelDataType( ePixelDataType );

	if( nPixelSize == 0 )
	{
		vstr::warnp() << "VistaNativeOpenGL::SetData() -- invalid format specification" << std::endl;
		return false;
	}

	if( m_pData && m_bOwnData )
	{
		delete [] m_pData;
	}

	unsigned int nDataSize = nWidth * nHeight * nPixelSize;
	if( bCopyData || pData == NULL )
	{
		m_pData = new VistaType::byte[nDataSize];
		if( pData )
			memcpy( m_pData, pData, nDataSize );
		else
			memset( m_pData, 0, nDataSize );
		m_bOwnData = true;
	}
	else
	{
		m_pData = pData;
		m_bOwnData = false;
	}

	m_ePixelFormat = ePixelFormat;
	m_ePixelDataType = ePixelDataType;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nDepth = 1;
	m_nDimension = 2;

	return true;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::Set3DData( const int nWidth, const int nHeight, const int nDepth,
																			VistaType::byte* pData, 
																			const GLenum ePixelFormat,
																			const GLenum ePixelDataType,
																			const bool bCopyData )
{
	unsigned int nPixelSize = VistaImage::GetNumberOfComponentsForPixelFormat( ePixelFormat )
						* VistaImage::GetNumberOfBytesForPixelDataType( ePixelDataType );

	if( nPixelSize == 0 )
	{
		vstr::warnp() << "VistaNativeOpenGL::SetData() -- invalid format specification" << std::endl;
		return false;
	}

	if( m_pData && m_bOwnData )
	{
		delete [] m_pData;
	}

	unsigned int nDataSize = nWidth * nHeight * nDepth * nPixelSize;
	if( bCopyData || pData == NULL )
	{
		m_pData = new VistaType::byte[nDataSize];
		if( m_pData )
			memcpy( m_pData, pData, nDataSize );
		else
			memset( m_pData, 0, nDataSize );
		m_bOwnData = true;
	}
	else
	{
		m_pData = pData;
		m_bOwnData = false;
	}

	m_ePixelFormat = ePixelFormat;
	m_ePixelDataType = ePixelDataType;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nDepth = nDepth;
	m_nDimension = 3;

	return true;
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetWidth() const
{
	return m_nWidth;
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetHeight() const
{
	return m_nHeight;
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetDepth() const
{
	return m_nDepth;
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetBytesPerPixel() const
{
	return ( GetComponentsPerPixel() * GetBytesPerComponent() );
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetComponentsPerPixel() const
{
	return VistaImage::GetNumberOfComponentsForPixelFormat( m_ePixelFormat );
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetBytesPerComponent() const
{
	return VistaImage::GetNumberOfBytesForPixelDataType( m_ePixelDataType );
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetHasAlphaChannel() const
{
	// @TODO: complete
	return ( m_ePixelFormat == GL_RGBA || m_ePixelFormat == GL_ALPHA );
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetHasTransparency() const
{
	VISTA_THROW_NOT_IMPLEMENTED;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetHasBinaryAlphaChannel() const
{
	VISTA_THROW_NOT_IMPLEMENTED;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::Rescale( const unsigned int nNewWidth, const unsigned int nNewHeight )
{
	vstr::warnp() << "VistaNativeOpenGL::rescale() -- rescaling not supported" << std::endl;
	return false;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::Crop( const unsigned int nBottomLeftX, const unsigned int nBottomLeftY, const unsigned int nNewWidth, const unsigned int nNewHeight )
{
	vstr::warnp() << "VistaNativeOpenGL::Crop() -- cropping not supported" << std::endl;
	return false;
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetPixelFormat() const
{
	return m_ePixelFormat;
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetPixelDataType() const
{
	return m_ePixelDataType;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::ReformatImage( const GLenum eNewFormat, const GLenum ePixelDataType )
{
	vstr::warnp() << "VistaNativeOpenGL::Crop() -- reformating images not supported" << std::endl;
	return false;
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetDataSize() const
{
	return ( GetBytesPerPixel() * m_nWidth * m_nHeight * m_nDepth );
}

const VistaType::byte* VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetData() const
{
	return m_pData;
}

VistaType::byte* VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetDataWrite()
{
	return m_pData;
}

IVistaImageCore* VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::Clone() const
{
	VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore* pClone = new VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore();
	if( GetIsValid() )
	{
		pClone->m_ePixelFormat = m_ePixelFormat;
		pClone->m_ePixelDataType = m_ePixelDataType;
		pClone->m_nWidth = m_nWidth;
		pClone->m_nHeight = m_nHeight;
		pClone->m_nDepth = m_nDepth;
		pClone->m_nDimension = m_nDimension;
		
		pClone->m_bOwnData = true;
		int nDataSize = GetDataSize();
		pClone->m_pData = new VistaType::byte[nDataSize];
		memcpy( pClone->m_pData, m_pData, nDataSize );
	}
	return pClone;
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::ImageCore::GetDimension() const
{
	return m_nDimension;
}

/*============================================================================*/
/* TextureCore                                                                */
/*============================================================================*/

VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::TextureCore()
: IVistaTextureCore()
, m_nTextureId( 0 )
, m_eTextureTarget( GL_TEXTURE_2D )
, m_bIsValid( false )
, m_bHasMipmaps( false )
, m_nDepth( 0 )
, m_nHeight( 0 )
, m_nWidth( 0 )
, m_eTextureEnvMode( GL_MODULATE )
{
	glewInit(); //@IMGTODO: relocate
	glGenTextures( 1, &m_nTextureId );
	CHECKFORGLERROR( "ctor" );
}
VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::~TextureCore()
{
	glDeleteTextures( 1, &m_nTextureId );
	CHECKFORGLERROR( "dtor" );
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetIsValid() const
{
	return m_bIsValid;
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetTextureTarget() const
{
	return m_eTextureTarget;
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetInternalFormat() const
{
	return m_eInternalFormat;
}

VistaImage VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::LoadTextureDataToImage()
{
	VistaImage oImage;
	// @IMGTODO: properly load non-rgb formats
	switch( m_eTextureTarget )
	{
		case GL_TEXTURE_1D:
		{
			oImage.Set1DData( m_nWidth, NULL, GL_RGB, GL_UNSIGNED_BYTE );
			glGetTexImage( GL_TEXTURE_1D, 0, GL_RGB, GL_UNSIGNED_BYTE, oImage.GetDataWrite() );
			break;
		}
		case GL_TEXTURE_2D:
		{
			oImage.Set2DData( m_nWidth, m_nHeight, NULL, GL_RGB, GL_UNSIGNED_BYTE );
			glGetTexImage( GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, oImage.GetDataWrite() );
			break;
		}
		case GL_TEXTURE_3D:
		{
			oImage.Set3DData( m_nWidth, m_nHeight, m_nDepth, NULL, GL_RGB, GL_UNSIGNED_BYTE );
			glGetTexImage( GL_TEXTURE_3D, 0, GL_RGB, GL_UNSIGNED_BYTE, oImage.GetDataWrite() );
			break;
		}
		default:
			VISTA_THROW( "VistaImage::LoadTextureDataToImage cant handle texture type", -1 );
	};
	CHECKFORGLERROR( "LoadTextureDataToImage" );
	return oImage;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::Set1DData( const int nWidth, const VistaType::byte* pData, const bool bGenMipmaps /*= true*/, const GLenum eInternalFormat /*= GL_RGBA*/, const GLenum ePixelFormat /*= GL_RGBA*/, const GLenum eDataFormat /*= GL_UNSIGNED_BYTE */ )
{
	Release(); // in case the tex is still bound on another target
	m_eTextureTarget = GL_TEXTURE_1D;
	
	glBindTexture( m_eTextureTarget, m_nTextureId );

	glTexImage1D( m_eTextureTarget, 0, eInternalFormat, nWidth, 0, ePixelFormat, eDataFormat, pData );

	GLuint nError = glGetError();
	if(nError != GL_NO_ERROR)
	{
		vstr::warnp() << "VistaNativeOpenGL::Set1DData(): uploading data failed with gl error:\n";
		vstr::warn() << vstr::singleindent << gluErrorString( nError ) << std::endl;
		return false;
	}

	m_bHasMipmaps = false;
	if( bGenMipmaps )
		CreateMipmaps();

	m_nWidth = nWidth;
	m_nHeight = 0;
	m_nDepth = 0;

	m_bIsValid = true;

	CHECKFORGLERROR( "Set1DData" );

	return true;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::Set2DData( const int nWidth, const int nHeight, const VistaType::byte* pData, const bool bGenMipmaps /*= true*/, const GLenum eInternalFormat /*= GL_RGBA*/, const GLenum ePixelFormat /*= GL_RGBA*/, const GLenum eDataFormat /*= GL_UNSIGNED_BYTE */, const int nNumMultisamples )
{
	GLuint nError = glGetError();
	if( nError != GL_NO_ERROR )
	{
		vstr::warnp() << "VistaNativeOpenGL::Set2DData(): Lingering gl error before entering:\n";
		vstr::warn() << vstr::singleindent << gluErrorString( nError ) << std::endl;
	}

	Release(); // in case the tex is still bound on another target
	m_nNumMultisamples = nNumMultisamples;
	m_eTextureTarget = GL_TEXTURE_2D;
	if( m_nNumMultisamples > 0 )
	{
		if( pData != NULL )
		{
			vstr::warnp() << "[VistaGLTexture::Set2DData]: Cannot use multisampling when passing data - using non-multisampled texture instead" << std::endl;
			m_nNumMultisamples = 0;
		}
		else
		{
			m_eTextureTarget = GL_TEXTURE_2D_MULTISAMPLE;
		}
	}		
	glBindTexture( m_eTextureTarget, m_nTextureId );

	if( m_nNumMultisamples > 0 )
		glTexImage2DMultisample( m_eTextureTarget, m_nNumMultisamples, eInternalFormat, nWidth, nHeight, false ); // @TODO: fixedsamplelocations?
	else
		glTexImage2D( m_eTextureTarget, 0, eInternalFormat, nWidth, nHeight, 0, ePixelFormat, eDataFormat, pData );

	nError = glGetError();
	if( nError != GL_NO_ERROR )
	{
		vstr::warnp() << "VistaNativeOpenGL::Set2DData(): uploading data failed with gl error:\n";
		vstr::warn() << vstr::singleindent << gluErrorString( nError ) << std::endl;
		return false;
	}

	m_bHasMipmaps = false;
	if( bGenMipmaps )
		CreateMipmaps();

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nDepth = 0;

	m_bIsValid = true;

	glBindTexture( m_eTextureTarget, 0 );

	CHECKFORGLERROR( "Set2DData" );
	
	return true;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::Set3DData( const int nWidth, const int nHeight, const int nDepth, const VistaType::byte* pData, const bool bGenMipmaps /*= true*/, const GLenum eInternalFormat /*= GL_RGBA*/, const GLenum ePixelFormat /*= GL_RGBA*/, const GLenum eDataFormat /*= GL_UNSIGNED_BYTE */ )
{
	// @TODO. maintain original texture target if it fails

	Release(); // in case the tex is still bound on another target
	m_eTextureTarget = GL_TEXTURE_3D;
	m_nNumMultisamples = 0;
	
	glBindTexture( m_eTextureTarget, m_nTextureId );

	glTexImage3D( m_eTextureTarget, 0, eInternalFormat, nWidth, nHeight, nDepth, 0, ePixelFormat, eDataFormat, pData );

	GLuint nError = glGetError();
	if(nError != GL_NO_ERROR)
	{
		vstr::warnp() << "VistaNativeOpenGL::Set2DData(): uploading data failed with gl error:\n";
		vstr::warn() << vstr::singleindent << gluErrorString( nError ) << std::endl;
		return false;
	}

	m_bHasMipmaps = false;
	if( bGenMipmaps )
		CreateMipmaps();

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nDepth = nDepth;

	m_bIsValid = true;

	CHECKFORGLERROR( "Set3DData" );
	
	return true;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::SetData( const VistaImage& oImage, const bool bGenMipmaps, const GLenum eInternalFormat )
{
	if( oImage.GetIsValid() == false )
	{
		vstr::warnp() << "VistaNativeOpenGL::SetData(): invalid image";
	}
	GLenum ePixelFormat = oImage.GetPixelFormat();
	GLenum eDataFormat = oImage.GetPixelDataType();
	
	if( oImage.GetDimension() == 3 )
	{
		return Set3DData( oImage.GetWidth(), oImage.GetHeight(), oImage.GetDepth(),
					oImage.GetData(), bGenMipmaps, eInternalFormat, ePixelFormat, eDataFormat );
	}
	else if( oImage.GetDimension() == 2 )
	{
		return Set2DData( oImage.GetWidth(), oImage.GetHeight(), 
					oImage.GetData(), bGenMipmaps, eInternalFormat, ePixelFormat, eDataFormat );
	}
	else if( oImage.GetDimension() == 1 )
	{
		return Set1DData( oImage.GetWidth(),
					oImage.GetData(), bGenMipmaps, eInternalFormat, ePixelFormat, eDataFormat );
	}
	else
	{
		vstr::warnp() << "VistaNativeOpenGL::SetData(): Image has no size" << std::endl;
		return false;
	}
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetWidth() const
{
	return m_nWidth;
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetHeight() const
{
	return m_nHeight;
}

unsigned int VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetDepth() const
{
	return m_nDepth;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetHasMipmaps() const
{
	return m_bHasMipmaps;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::CreateMipmaps()
{
	if( m_bHasMipmaps )
		return true;
	Bind();
	glGenerateMipmap( m_eTextureTarget );
	
	GLuint nError = glGetError();
	if(nError != GL_NO_ERROR)
	{
		vstr::warnp() << "VistaNativeOpenGL::CreateMipmaps(): mipmap generation failed with gl error\n";
		vstr::warn() << vstr::singleindent << gluErrorString( nError ) << std::endl;
		return false;
	}
	m_bHasMipmaps = true;
	CHECKFORGLERROR( "CreateMipmaps" );
	return true;
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetMinFilter() const
{
	GLint eState;
	Bind();
	glGetTexParameteriv( m_eTextureTarget, GL_TEXTURE_MIN_FILTER, &eState );
	CHECKFORGLERROR( "GetMinFilter" );
	return eState;
}

void VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::SetMinFilter( const GLenum eMinFilterMode )
{
	Bind();
	glTexParameteri( m_eTextureTarget, GL_TEXTURE_MIN_FILTER, eMinFilterMode );
	CHECKFORGLERROR( "SetMinFilter" );
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetMagFilter() const
{
	GLint eState;
	Bind();
	glGetTexParameteriv( m_eTextureTarget, GL_TEXTURE_MAG_FILTER, &eState );
	CHECKFORGLERROR( "GetMagFilter" );
	return eState;
}

void VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::SetMagFilter( const GLenum eMagFilterMode )
{
	Bind();
	glTexParameteri( m_eTextureTarget, GL_TEXTURE_MAG_FILTER, eMagFilterMode );
	CHECKFORGLERROR( "SetMagFilter" );
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetWrapS() const
{	
	GLint eState;
	Bind();
	glGetTexParameteriv( m_eTextureTarget, GL_TEXTURE_WRAP_S, &eState );
	CHECKFORGLERROR( "GetWrapS" );
	return eState;
}

void VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::SetWrapS( const GLenum eWrappingMode )
{
	Bind();
	glTexParameteri( m_eTextureTarget, GL_TEXTURE_WRAP_S, eWrappingMode );
	CHECKFORGLERROR( "SetWrapS" );
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetWrapT() const
{
	GLint eState;
	Bind();
	glGetTexParameteriv( m_eTextureTarget, GL_TEXTURE_WRAP_T, &eState );
	CHECKFORGLERROR( "GetWrapT" );
	return eState;
}

void VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::SetWrapT( const GLenum eWrappingMode )
{
	Bind();
	glTexParameteri( m_eTextureTarget, GL_TEXTURE_WRAP_T, eWrappingMode );
	CHECKFORGLERROR( "SetWrapT" );
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetWrapR() const
{
	GLint eState;
	Bind();
	glGetTexParameteriv( m_eTextureTarget, GL_TEXTURE_WRAP_R, &eState );
	CHECKFORGLERROR( "GetWrapR" );
	return eState;;
}

void VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::SetWrapR( const GLenum eWrappingMode )
{
	Bind();
	glTexParameteri( m_eTextureTarget, GL_TEXTURE_WRAP_R, eWrappingMode );
	CHECKFORGLERROR( "SetWrapR" );
}

float VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetAnisotropy() const
{
	float nValue;
	Bind();
	glGetTexParameterfv( m_eTextureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, &nValue );
	CHECKFORGLERROR( "GetAnisotropy" );
	return nValue;
}

void VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::SetAnisotropy( const float nAnisotropy )
{
	Bind();
	glTexParameterf( m_eTextureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, nAnisotropy );
	CHECKFORGLERROR( "SetAnisotropy" );
}

float VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetLodBias() const
{
	float nValue;
	Bind();
	glGetTexParameterfv( m_eTextureTarget, GL_TEXTURE_LOD_BIAS, &nValue );
	CHECKFORGLERROR( "GetLodBias" );
	return nValue;
}

void VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::SetLodBias( const float nBias )
{
	Bind();
	glTexParameterf( m_eTextureTarget, GL_TEXTURE_LOD_BIAS, nBias );
	CHECKFORGLERROR( "SetLodBias" );
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::Bind() const
{
	if( m_bIsValid == false )
		return false;
	CHECKFORGLERROR( "PreBind" );
	glBindTexture( m_eTextureTarget, m_nTextureId );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_eTextureEnvMode );
	CHECKFORGLERROR( "Bind" );
	return true;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::Bind( const GLenum eTextureUnit ) const
{
	if( m_bIsValid == false )
		return false;
	glActiveTexture( eTextureUnit );
	glBindTexture( m_eTextureTarget, m_nTextureId );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_eTextureEnvMode );
	CHECKFORGLERROR( "Bind" );
	return true;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::Release() const
{
	glBindTexture( m_eTextureTarget, 0 );
	return m_bIsValid;
}

bool VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::Release( const GLenum eTextureUnit ) const
{
	glActiveTexture( eTextureUnit );
	glBindTexture( m_eTextureTarget, 0 );
	
	glActiveTexture( GL_TEXTURE0 );
	return m_bIsValid;
}

GLuint VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetGLId() const
{
	return m_nTextureId;
}

IVistaTextureCore* VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::Clone() const
{
	VISTA_THROW_NOT_IMPLEMENTED;
}

GLenum VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetTextureEnvMode() const
{
	return m_eTextureEnvMode;
}

void VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::SetTextureEnvMode( GLenum eMode )
{
	m_eTextureEnvMode = eMode;
}

int VistaNativeOpenGLImageAndTextureCoreFactory::TextureCore::GetNumMultisamples() const
{
	return m_nNumMultisamples;
}
