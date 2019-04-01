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




#include "VistaOpenSGTextureLoader.h"

#if defined(WIN32)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4275)
#pragma warning(disable: 4267)
#pragma warning(disable: 4251)
#pragma warning(disable: 4231)
#endif

#include <OpenSG/OSGTextureChunk.h>
#include <OpenSG/OSGCubeTextureChunk.h>
#include <OpenSG/OSGImageFunctions.h>
#include "VistaAspects/VistaConversion.h"
#include "VistaTools/VistaFileSystemFile.h"

namespace
{
	osg::TextureChunkPtr LoadOSGTextureFromFile( const std::string& sFilename, int nAnisotropy )
	{
		osg::ImagePtr pImage = osg::Image::create();

		beginEditCP( pImage );
			bool bSuccess = pImage->read( sFilename.c_str() );
		endEditCP( pImage );
		if( bSuccess == false )
		{
			subRefCP( pImage );
			return osg::NullFC;
		}
		
		osg::TextureChunkPtr pTex( osg::TextureChunk::create() );
		beginEditCP( pTex );
			pTex->setImage( pImage );
			pTex->setWrapR( GL_REPEAT );
			pTex->setWrapS( GL_REPEAT );
			pTex->setMinFilter( GL_LINEAR_MIPMAP_LINEAR );	
			pTex->setMagFilter( GL_LINEAR );
			pTex->setEnvMode( GL_MODULATE );
			pTex->setAnisotropy( nAnisotropy );
		endEditCP( pTex );

		return pTex;
	}

	std::string GetImageFileFromPrefix( const std::string& sPrefix, const std::vector< std::string >& vecExtension )
	{
		std::string sFile = sPrefix;
		if( VistaFileSystemFile( sFile ).Exists() )
			return sFile;
		for( std::vector< std::string >::const_iterator itExtension = vecExtension.begin(); itExtension != vecExtension.end(); ++itExtension )
		{
			sFile = sPrefix + "." + (*itExtension);
			if( VistaFileSystemFile( sFile ).Exists() )
				return sFile;
		}
		return "";
	}
}


VistaOpenSGTextureLoader::VistaOpenSGTextureLoader()
: m_nDefaultAnisotropy( 16 )
{
	m_vecImageFileExtensions.push_back( "jpg" );
	m_vecImageFileExtensions.push_back( "png" );
	m_vecImageFileExtensions.push_back( "bmp" );
	m_vecImageFileExtensions.push_back( "gif" );
	m_vecImageFileExtensions.push_back( "tif" );

	m_vecSearchPathes.push_back( "." );
}

VistaOpenSGTextureLoader::~VistaOpenSGTextureLoader()
{
}

osg::TextureChunkPtr VistaOpenSGTextureLoader::LoadTexture( const std::string& sFilename )
{
	std::string sFullFilename = GetImageFileLocation( sFilename );
	if( sFullFilename.empty() )
		return osg::NullFC;
	return LoadOSGTextureFromFile( sFullFilename, GetDefaultAnisotropy() );
}

osg::TextureChunkPtr VistaOpenSGTextureLoader::LoadNormalMapTexture( const std::string& sFilename )
{
	return LoadTexture( sFilename );
}

osg::TextureChunkPtr VistaOpenSGTextureLoader::LoadNormalMapTextureFromBumpMap( const std::string& sFilename,
																			const float nNormalMagnitude,
																			const bool bSaveConvertedImage )
{
	//osg::TextureChunkRefPtr pTex( LoadNormalMapTexture( sFilename ) );
	//if( pTex == osg::NullFC )
	//	return osg::NullFC;

	//if( ConvertBumpMapTextureToNormalMap( pTex, nNormalMagnitude ) == false )
	//	return osg::NullFC;

	std::string sFullFilename = GetImageFileLocation( sFilename );
	if( sFullFilename.empty() )
		return osg::NullFC;

	osg::ImagePtr pImage( osg::Image::create() );

	beginEditCP( pImage );
		bool bSuccess = pImage->read( sFullFilename.c_str() );
		if( bSuccess && pImage->getPixelFormat() != osg::Image::OSG_L_PF )
			bSuccess = pImage->reformat( osg::Image::OSG_L_PF );
	endEditCP( pImage );
	if( bSuccess == false || ConvertBumpMapImageToNormalMap( pImage, nNormalMagnitude ) == false )
	{
		subRefCP( pImage );
		return osg::NullFC;
	}

	if( bSaveConvertedImage )
	{
		std::string sSaveFilename = GetNormalMapFilenameForBumpmap( sFullFilename, nNormalMagnitude );
		pImage->write( sSaveFilename.c_str() ); 
	}
		
	osg::TextureChunkPtr pTex( osg::TextureChunk::create() );
	beginEditCP( pTex );
		pTex->setImage( pImage );
		pTex->setWrapR( GL_REPEAT );
		pTex->setWrapS( GL_REPEAT );
		pTex->setMinFilter( GL_LINEAR_MIPMAP_LINEAR );	
		pTex->setMagFilter( GL_LINEAR );
		pTex->setEnvMode( GL_MODULATE );
		pTex->setAnisotropy( GetDefaultAnisotropy() );
	endEditCP( pTex );

	return pTex;
}


osg::CubeTextureChunkPtr VistaOpenSGTextureLoader::LoadCubeMapTexture( const std::string& sTopFile, 
																	const std::string& sBottomFile, 
																	const std::string& sLeftFile,
																	const std::string& sRightFile,
																	const std::string& sFrontFile, 
																	const std::string& sBackFile )
{
	bool bSuccess = true;

	std::string sActualTopFilename = GetImageFileLocation( sTopFile );
	std::string sActualBottomFilename = GetImageFileLocation( sBottomFile );
	std::string sActualLeftFilename = GetImageFileLocation( sLeftFile );
	std::string sActualRightFilename = GetImageFileLocation( sRightFile );
	std::string sActualFrontFilename = GetImageFileLocation( sFrontFile );
	std::string sActualBackFilename = GetImageFileLocation( sBackFile );

	if( sActualTopFilename.empty() || sActualBottomFilename.empty() || sActualLeftFilename.empty()
		|| sActualRightFilename.empty() || sActualFrontFilename.empty() || sActualBackFilename.empty() )
	{
		return osg::NullFC;
	}

	osg::ImageRefPtr pTopImage( osg::Image::create() );
	beginEditCP( pTopImage );
		bSuccess &= pTopImage->read( sActualTopFilename.c_str() );
	endEditCP( pTopImage );

	osg::ImageRefPtr pBottomImage( osg::Image::create() );
	beginEditCP( pBottomImage );
		bSuccess &= pBottomImage->read( sActualBottomFilename.c_str() );
	endEditCP( pBottomImage );

	osg::ImageRefPtr pLeftImage( osg::Image::create() );
	beginEditCP( pLeftImage );
		bSuccess &= pLeftImage->read( sActualLeftFilename.c_str() );
	endEditCP( pLeftImage );

	osg::ImageRefPtr pRightImage( osg::Image::create() );
	beginEditCP( pRightImage );
		bSuccess &= pRightImage->read( sActualRightFilename.c_str() );
	endEditCP( pRightImage );

	osg::ImageRefPtr pFrontImage( osg::Image::create() );
	beginEditCP( pFrontImage );
		bSuccess &= pFrontImage->read( sActualFrontFilename.c_str() );
	endEditCP( pFrontImage );

	osg::ImageRefPtr pBackImage( osg::Image::create() );
	beginEditCP( pBackImage );
		bSuccess &= pBackImage->read( sActualBackFilename.c_str() );
	endEditCP( pBackImage );

	if( bSuccess == false )
		return osg::NullFC;

	osg::CubeTextureChunkPtr pTex( osg::CubeTextureChunk::create() );
	beginEditCP( pTex );
		osg::ImagePtr pImage = pFrontImage;
		pTex->setImage( pImage );
		pImage = pBackImage;
		pTex->setPosZImage( pImage );
		pImage = pRightImage;
		pTex->setPosXImage( pImage );
		pImage = pLeftImage;
		pTex->setNegXImage( pImage );
		pImage = pTopImage;
		pTex->setPosYImage( pImage );
		pImage = pBottomImage;
		pTex->setNegYImage( pImage );

		pTex->setWrapR( GL_CLAMP_TO_EDGE );
		pTex->setWrapS( GL_CLAMP_TO_EDGE );
		pTex->setWrapT( GL_CLAMP_TO_EDGE );
		pTex->setMinFilter( GL_LINEAR_MIPMAP_LINEAR );
		pTex->setMagFilter( GL_LINEAR );
		pTex->setEnvMode( GL_MODULATE );
	endEditCP( pTex );

	return pTex;
}

osg::CubeTextureChunkPtr VistaOpenSGTextureLoader::LoadCubeMapTexture( const std::string& sFolder )
{
	std::string sTop, sBottom, sLeft, sRight, sFront, sBack;

	if( GetCubemapFileLocations( sFolder, sTop, sBottom, sLeft, sRight, sFront, sBack ) == false )
		return osg::NullFC;

	return LoadCubeMapTexture( sTop, sBottom, sLeft, sRight, sFront, sBack );
}


bool VistaOpenSGTextureLoader::ConvertBumpMapImageToNormalMap( osg::ImagePtr pImage, const float nNormalMagnitude )
{
	if( pImage->getPixelFormat() != osg::Image::OSG_L_PF )
	{
		beginEditCP( pImage );
		pImage->reformat( osg::Image::OSG_L_PF );
		endEditCP( pImage );
	}
	return osg::createNormalMapFromBump( pImage, osg::NullFC, osg::Vec3f( 1.0f, 1.0f, 1.0f / nNormalMagnitude ) );
}



int VistaOpenSGTextureLoader::GetDefaultAnisotropy() const
{
	return m_nDefaultAnisotropy;
}

void VistaOpenSGTextureLoader::SetDefaultAnisotropy( const int oValue )
{
	m_nDefaultAnisotropy = oValue;
}

osg::TextureChunkPtr VistaOpenSGTextureLoader::RetrieveTexture( const std::string& sFilename )
{
	std::map< std::string, osg::TextureChunkRefPtr >::const_iterator itEntry = m_mapLoadedTextures.find( sFilename );
	if( itEntry != m_mapLoadedTextures.end() )
	{
		assert( (*itEntry).second != osg::NullFC );
		return (*itEntry).second;
	}
	else
	{
		osg::TextureChunkPtr pTex = LoadTexture( sFilename );
		if( pTex != osg::NullFC )
		{
			m_mapLoadedTextures[ sFilename ] = pTex;
		}
		return pTex;
	}
}

osg::TextureChunkPtr VistaOpenSGTextureLoader::RetrieveNormalMapTexture( const std::string& sFilename )
{
	std::map< std::string, osg::TextureChunkRefPtr >::const_iterator itEntry = m_mapLoadedNormalTextures.find( sFilename );
	if( itEntry != m_mapLoadedNormalTextures.end() )
	{
		assert( (*itEntry).second != osg::NullFC );
		return (*itEntry).second;
	}
	else
	{
		osg::TextureChunkPtr pTex = LoadNormalMapTexture( sFilename );
		if( pTex != osg::NullFC )
		{
			m_mapLoadedNormalTextures[ sFilename ] = pTex;
		}
		return pTex;
	}
}

osg::TextureChunkPtr VistaOpenSGTextureLoader::RetrieveNormalMapTextureFromBumpMap( const std::string& sFilename,
																				const float nNormalMagnitude,
																				const bool bSaveConvertedImage )
{
	std::string sNormalMapName = GetNormalMapFilenameForBumpmap( sFilename, nNormalMagnitude );
	if( sNormalMapName.empty() )
		return osg::NullFC;

	std::map< std::string, osg::TextureChunkRefPtr >::const_iterator itEntry = m_mapLoadedNormalTextures.find( sNormalMapName );
	if( itEntry != m_mapLoadedNormalTextures.end() )
	{
		assert( (*itEntry).second != osg::NullFC );
		return (*itEntry).second;
	}

	osg::TextureChunkPtr pTex = osg::NullFC;

	if( bSaveConvertedImage )
	{
		pTex = LoadTexture( sNormalMapName );
	}
	if( pTex == osg::NullFC )
	{
		pTex = LoadNormalMapTextureFromBumpMap( sFilename, nNormalMagnitude, bSaveConvertedImage );
	}

	if( pTex != osg::NullFC )
	{
		m_mapLoadedNormalTextures[ sNormalMapName ] = pTex;
	}

	return pTex;
}


osg::CubeTextureChunkPtr VistaOpenSGTextureLoader::RetrieveCubemapTexture( const std::string& sTopFile,
																			const std::string& sBottomFile,
																			const std::string& sLeftFile,
																			const std::string& sRightFile,
																			const std::string& sFrontFile,
																			const std::string& sBackFile )
{
	std::string sIndexString = GetCubemapIndexString( sTopFile, sBottomFile, sLeftFile, sRightFile, sFrontFile, sBottomFile );
	
	std::map< std::string, osg::CubeTextureChunkRefPtr >::const_iterator itEntry = m_mapCubemapTextures.find( sIndexString );
	if( itEntry != m_mapCubemapTextures.end() )
	{
		assert( (*itEntry).second != osg::NullFC );
		return (*itEntry).second;
	}

	osg::CubeTextureChunkPtr pTex = LoadCubeMapTexture( sTopFile, sBottomFile, sLeftFile, sRightFile, sFrontFile, sBackFile );

	if( pTex )
	{
		m_mapCubemapTextures[ sIndexString ] = pTex;
	}

	return pTex;
}

osg::CubeTextureChunkPtr VistaOpenSGTextureLoader::RetrieveCubemapTexture( const std::string& sFolder )
{
	std::string sIndexString = sFolder;
	
	std::map< std::string, osg::CubeTextureChunkRefPtr >::const_iterator itEntry = m_mapCubemapTextures.find( sIndexString );
	if( itEntry != m_mapCubemapTextures.end() )
	{
		assert( (*itEntry).second != osg::NullFC );
		return (*itEntry).second;
	}

	osg::CubeTextureChunkPtr pTex = LoadCubeMapTexture( sFolder );

	if( pTex )
	{
		m_mapCubemapTextures[ sIndexString ] = pTex;
	}

	return pTex;
}


std::string VistaOpenSGTextureLoader::GetNormalMapFilenameForBumpmap( const std::string& sBumpmapImage,
																		const float nNormalMagnitude ) const
{
	std::size_t nDotPos = sBumpmapImage.rfind( "." );
	if( nDotPos == std::string::npos )
		return "";
	std::string sExtension = sBumpmapImage.substr( nDotPos );
	std::string sBaseName = sBumpmapImage.substr( 0, nDotPos );
	std::string sNormalMapName = sBaseName + "_normal_" + VistaConversion::ToString( nNormalMagnitude ) + sExtension;
	return sNormalMapName;
}

std::string VistaOpenSGTextureLoader::GetCubemapIndexString( const std::string& sTopFile,
															const std::string& sBottomFile,
															const std::string& sLeftFile,
															const std::string& sRightFile,
															const std::string& sFrontFile, 
															const std::string& sBackFile ) const
{
	std::string sIndex = sTopFile + "|" + sBackFile + "|" + sLeftFile + "|"
						+ sRightFile + "|" + sFrontFile + "|" + sBackFile;
	return sIndex;
}

const std::vector< std::string >& VistaOpenSGTextureLoader::GetImageSearchPathes() const
{
	return m_vecSearchPathes;
}

void VistaOpenSGTextureLoader::SetImageSearchPathes( const std::vector< std::string >& vecSearchPathes )
{
	m_vecSearchPathes = vecSearchPathes;
}

void VistaOpenSGTextureLoader::AddImageSearchPath( const std::string& sPath )
{
	m_vecSearchPathes.push_back( sPath );
}

const std::vector< std::string >& VistaOpenSGTextureLoader::GetImageFileExtensions() const
{
	return m_vecImageFileExtensions;
}

void VistaOpenSGTextureLoader::SetImageFileExtensions( const std::vector< std::string >& vecExtensions )
{
	m_vecImageFileExtensions = vecExtensions;
}

void VistaOpenSGTextureLoader::AddImageFileExtensions( const std::string& sExtension )
{
	m_vecImageFileExtensions.push_back( sExtension );
}

std::string VistaOpenSGTextureLoader::GetImageFileLocation( const std::string& sImage )
{
	std::string sFullFilename = GetImageFileFromPrefix( sImage, m_vecImageFileExtensions );
	if( sFullFilename.empty() == false )
		return sFullFilename;
	
	for( std::vector< std::string >::const_iterator itPath = m_vecSearchPathes.begin();
			itPath != m_vecSearchPathes.end(); ++itPath )
	{
		sFullFilename = GetImageFileFromPrefix( (*itPath) + "/" + sImage, m_vecImageFileExtensions );
		if( sFullFilename.empty() == false )
			return sFullFilename;
	}
	return "";
}

bool VistaOpenSGTextureLoader::GetCubemapFileLocations( const std::string& sFolder,
														std::string& sTopFile, std::string& sBottomFile,
														std::string& sLeftFile, std::string& sRightFile,
														std::string& sFrontFile, std::string& sBackFile )
{
	sTopFile = GetImageFileLocation( sFolder + "/" + "top" );
	if( sTopFile.empty() )
		sTopFile = GetImageFileLocation( sFolder + "/" + "posy" );

	sBottomFile = GetImageFileLocation( sFolder + "/" + "bottom" );
	if( sBottomFile.empty() )
		sBottomFile = GetImageFileLocation( sFolder + "/" + "negy" );

	sLeftFile = GetImageFileLocation( sFolder + "/" + "left" );
	if( sLeftFile.empty() )
		sLeftFile = GetImageFileLocation( sFolder + "/" + "negx" );

	sRightFile = GetImageFileLocation( sFolder + "/" + "right" );
	if( sRightFile.empty() )
		sRightFile = GetImageFileLocation( sFolder + "/" + "posx" );

	sFrontFile = GetImageFileLocation( sFolder + "/" + "front" );
	if( sFrontFile.empty() )
		sFrontFile = GetImageFileLocation( sFolder + "/" + "negz" );

	sBackFile = GetImageFileLocation( sFolder + "/" + "back" );
	if( sBackFile.empty() )
		sBackFile = GetImageFileLocation( sFolder + "/" + "posx" );

	return( !sTopFile.empty() && !sBottomFile.empty() && !sLeftFile.empty()
			&& !sRightFile.empty() && !sFrontFile.empty() && !sBackFile.empty() );
}

