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
#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>

#include "VistaImage.h"

#include "VistaImageAndTextureFactory.h"

#include <VistaBase/VistaExceptionBase.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

unsigned int VistaImage::GetNumberOfComponentsForPixelFormat( const GLenum eFormat )
{
	switch( eFormat )
	{
		case GL_RED:
		case GL_LUMINANCE:
		case GL_INTENSITY:
		case GL_DEPTH_COMPONENT:
			return 1;
		case GL_RG:
		case GL_LUMINANCE_ALPHA:
		case GL_DEPTH_STENCIL:
			return 2;
		case GL_RGB:
		case GL_BGR:
			return 3;
		case GL_RGBA: 
		case GL_BGRA:
			return 4;
		default:
			VISTA_THROW( "VistaImage: Invalid image format specifier", -1 );
	}	
}


std::string VistaImage::GetNameForPixelFormat( const GLenum eFormat )
{
	switch( eFormat )
	{
		case GL_RED:
			return "GL_RED";
		case GL_RG:
			return "GL_RG";
		case GL_RGB:
			return "GL_RGB";
		case GL_BGR:
			return "GL_BGR";
		case GL_RGBA: 
			return "GL_RGBA";
		case GL_BGRA:
			return "GL_BRGA";
		case GL_RED_INTEGER:
			return "GL_RED_INTEGER";
		case GL_RG_INTEGER:
			return "GL_RG_INTEGER";
		case GL_RGB_INTEGER:
			return "GL_RGB_INTEGER";
		case GL_BGR_INTEGER:
			return "GL_BGR_INTEGER";
		case  GL_RGBA_INTEGER:
			return "GL_RGBA_INTEGER";
		case GL_BGRA_INTEGER:
			return "GL_BGRA_INTEGER";
		case GL_STENCIL_INDEX:
			return "GL_STENCIL_INDEX";
		case GL_DEPTH_COMPONENT:
			return "GL_DEPTH_COMPONENT";
		case GL_DEPTH_STENCIL:
			return "GL_DEPTH_STENCIL";
		default:
			VISTA_THROW( "VistaImage: Invalid image format specifier", -1 );
	}
}

bool VistaImage::GetPixelFormatIsCompressed( const GLenum eFormat )
{
VISTA_THROW( "VistaImage: Invalid image format specifier", -1 );
}


unsigned int VistaImage::GetNumberOfBytesForPixelDataType( const GLenum eDataType )
{
	switch( eDataType )
	{	
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:
				return 1;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
			return 2;
		case GL_UNSIGNED_INT:
		case GL_INT:
			return 4;
		case GL_FLOAT:
			return 4;
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			VISTA_THROW( "VistaImage: component format currently not supported", -1 );
		default:
			VISTA_THROW( "VistaImage: Unknown component format specifier", -1 );
	}
	
}


std::string VistaImage::GetNameForPixelDataType( const GLenum eDataType )
{
	switch( eDataType )
	{	
		case GL_UNSIGNED_BYTE:
			return "GL_UNSIGNED_BYTE";
		case GL_BYTE:
			return "GL_BYTE";
		case GL_UNSIGNED_SHORT:
			return "GL_UNSIGNED_SHORT";
		case GL_SHORT:
			return "GL_SHORT";
		case GL_UNSIGNED_INT:
			return "GL_UNSIGNED_INT";
		case GL_INT:
			return "GL_INT";
		case GL_FLOAT:
			return "GL_FLOAT";
		case GL_UNSIGNED_BYTE_3_3_2:
			return "GL_UNSIGNED_BYTE_3_3_2";
		case GL_UNSIGNED_BYTE_2_3_3_REV:
			return "GL_UNSIGNED_BYTE_2_3_3_REV";
		case GL_UNSIGNED_SHORT_5_6_5:
			return "GL_UNSIGNED_SHORT_5_6_5";
		case GL_UNSIGNED_SHORT_5_6_5_REV:
			return "GL_UNSIGNED_SHORT_5_6_5_REV";
		case GL_UNSIGNED_SHORT_4_4_4_4:
			return "GL_UNSIGNED_SHORT_4_4_4_4";
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
			return "GL_UNSIGNED_SHORT_4_4_4_4_REV";
		case GL_UNSIGNED_SHORT_5_5_5_1:
			return "GL_UNSIGNED_SHORT_5_5_5_1";
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			return "GL_UNSIGNED_SHORT_1_5_5_5_REV";
		case GL_UNSIGNED_INT_8_8_8_8:
			return "GL_UNSIGNED_INT_8_8_8_8";
		case GL_UNSIGNED_INT_8_8_8_8_REV:
			return "GL_UNSIGNED_INT_8_8_8_8_REV";
		case GL_UNSIGNED_INT_10_10_10_2:
			return "GL_UNSIGNED_INT_10_10_10_2";
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			return "GL_UNSIGNED_INT_2_10_10_10_REV";
		default:
			VISTA_THROW( "VistaImage: Unknown component format specifier", -1 );
	}
}



VistaImage::VistaImage( DataHandling eDataHandlingMode /*= DH_COPY_ON_WRITE */ )
: IVistaSharedCoreOwner( IVistaImageAndTextureCoreFactory::GetSingleton()->CreateImageCore(), eDataHandlingMode )
{
}

VistaImage::VistaImage( const VistaImage& oOther )
: IVistaSharedCoreOwner( oOther )
{
}

VistaImage::~VistaImage()
{
}

VistaImage& VistaImage::operator=( const VistaImage& oOther )
{
	IVistaSharedCoreOwner::operator =( oOther );
	return (*this);
}


const IVistaImageCore* VistaImage::GetCore() const
{
	return static_cast<const IVistaImageCore*>( IVistaSharedCoreOwner::GetCore() );
}

IVistaImageCore* VistaImage::GetCoreForWriting()
{
	return static_cast<IVistaImageCore*>( IVistaSharedCoreOwner::GetCoreForWriting() );
}


bool VistaImage::GetIsValid() const
{
	return GetCore()->GetIsValid();
}

bool VistaImage::LoadFromFile( const std::string& sFilename, const bool bUseCaching )
{
	// Caching here - note that the individual for caching in the factory are thread safe
	// However, since only the querying and storing are locked, but not the loading process inbetween,
	// it may happen that the same image is loaded twice from concurrent threads.
	// However, that shouldn't be much of a problem - the final state will still be valid
	if( bUseCaching )
	{
		VistaImage* pCachedImage = IVistaImageAndTextureCoreFactory::GetSingleton()->GetCachedImage( sFilename );
		if( pCachedImage != NULL )
		{
			(*this) = (*pCachedImage);
			return true;
		}
	}
	bool bSuccess = GetCoreForWriting()->LoadFromFile( sFilename );
	if( bUseCaching && bUseCaching )
	{
		IVistaImageAndTextureCoreFactory::GetSingleton()->StoreCachedImage( sFilename, (*this) );
	}
	return bSuccess;
}

bool VistaImage::WriteToFile( const std::string& sFilename ) const
{
	return GetCore()->WriteToFile( sFilename );
}


bool VistaImage::Set1DData( const int nWidth,
						   VistaType::byte* pData,
						   const GLenum ePixelFormat,
						   const GLenum ePixelDataType,
						   const bool bCopyData )
{
	return GetCoreForWriting()->Set1DData( nWidth, pData, ePixelFormat, ePixelDataType, bCopyData );
}

bool VistaImage::Set2DData( const int nWidth, const int nHeight, 
						   VistaType::byte* pData,
						   const GLenum ePixelFormat,
						   const GLenum ePixelDataType,
						   const bool bCopyData )
{
	return GetCoreForWriting()->Set2DData( nWidth, nHeight, pData, ePixelFormat, ePixelDataType, bCopyData );
}

bool VistaImage::Set3DData( const int nWidth, const int nHeight, const int nDepth,
						   VistaType::byte* pData, 
						   const GLenum ePixelFormat,
						   const GLenum ePixelDataType,
						   const bool bCopyData )
{
	return GetCoreForWriting()->Set3DData( nWidth, nHeight, nDepth, pData, ePixelFormat, ePixelDataType, bCopyData );
}


unsigned int VistaImage::GetWidth() const
{
	return GetCore()->GetWidth();
}

unsigned int VistaImage::GetHeight() const
{
	return GetCore()->GetHeight();
}

unsigned int VistaImage::GetDepth() const
{
	return GetCore()->GetDepth();
}

unsigned int VistaImage::GetBytesPerPixel() const
{
	return GetCore()->GetBytesPerPixel();
}

unsigned int VistaImage::GetComponentsPerPixel() const
{
	return GetCore()->GetComponentsPerPixel();
}

unsigned int VistaImage::GetBytesPerComponent() const
{
	return GetCore()->GetBytesPerComponent();
}

bool VistaImage::GetHasAlphaChannel() const
{
	return GetCore()->GetHasAlphaChannel();
}

bool VistaImage::GetHasTransparency() const
{
	return GetCore()->GetHasTransparency();
}

bool VistaImage::GetHasBinaryAlphaChannel() const
{
	return GetCore()->GetHasBinaryAlphaChannel();
}

bool VistaImage::Rescale( const unsigned int nNewWidth, const unsigned int nNewHeight )
{
	return GetCoreForWriting()->Rescale( nNewWidth, nNewHeight );
}

bool VistaImage::Crop( const unsigned int nBottomLeftX, const unsigned int nBottomLeftY, const unsigned int nNewWidth, const unsigned int nNewHeight )
{
	return GetCoreForWriting()->Crop( nBottomLeftX, nBottomLeftY, nNewWidth, nNewHeight );
}

GLenum VistaImage::GetPixelFormat() const
{
	return GetCore()->GetPixelFormat();
}

GLenum VistaImage::GetPixelDataType() const
{
	return GetCore()->GetPixelDataType();
}

bool VistaImage::ReformatImage( const GLenum eNewPixelFormat, const GLenum eNewPixelDataType )
{
	return GetCoreForWriting()->ReformatImage( eNewPixelFormat, eNewPixelDataType );
}

unsigned int VistaImage::GetDataSize() const
{
	return GetCore()->GetDataSize();
}

const VistaType::byte* VistaImage::GetData() const
{
	return GetCore()->GetData();
}

VistaType::byte* VistaImage::GetDataWrite()
{
	return GetCoreForWriting()->GetDataWrite();
}

unsigned int VistaImage::GetDimension() const
{
	return GetCore()->GetDimension();
}
