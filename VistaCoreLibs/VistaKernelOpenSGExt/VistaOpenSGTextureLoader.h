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


#ifndef _VISTAOPENSGTEXTURELOADER_H
#define _VISTAOPENSGTEXTURELOADER_H

#include "VistaKernelOpenSGExtConfig.h"

#include <string>
#include <map>
#include <vector>

namespace osg
{
	template< class Ref > class RefPtr;
	template< class BasePtrTypeT, class FieldContainerTypeT > class FCPtr;

	class AttachmentPtr;
	class AttachmentContainerPtr;
	class StateChunk;
	class TextureChunk;
	class CubeTextureChunk;
	class Image;

	typedef FCPtr< AttachmentPtr, StateChunk > StateChunkPtr;
	typedef FCPtr< StateChunkPtr, TextureChunk > TextureChunkPtr;
	typedef FCPtr< TextureChunkPtr, CubeTextureChunk > CubeTextureChunkPtr;
	typedef FCPtr< AttachmentContainerPtr, Image > ImagePtr;
	
	typedef RefPtr< TextureChunkPtr > TextureChunkRefPtr;
	typedef RefPtr< CubeTextureChunkPtr > CubeTextureChunkRefPtr;
}

class VISTAKERNELOPENSGEXTAPI VistaOpenSGTextureLoader
{
public:
	VistaOpenSGTextureLoader();
	virtual ~VistaOpenSGTextureLoader();
	
	int GetDefaultAnisotropy() const;
	void SetDefaultAnisotropy( const int oValue );

	virtual osg::TextureChunkPtr LoadTexture( const std::string& sFilename );
	virtual osg::TextureChunkPtr LoadNormalMapTexture( const std::string& sFilename );
	virtual osg::TextureChunkPtr LoadNormalMapTextureFromBumpMap( const std::string& sFilename,
																const float nNormalMagnitude,
																const bool bSaveConvertedImage );
	virtual osg::CubeTextureChunkPtr LoadCubeMapTexture( const std::string& sTopFile,
													const std::string& sBottomFile,
													const std::string& sLeftFile,
													const std::string& sRightFile,
													const std::string& sFrontFile,
													const std::string& sBackFile );
	virtual osg::CubeTextureChunkPtr LoadCubeMapTexture( const std::string& sFolder );

	virtual bool ConvertBumpMapImageToNormalMap( osg::ImagePtr, const float nNormalMagnitude );

	virtual osg::TextureChunkPtr RetrieveTexture( const std::string& sFilename );
	virtual osg::TextureChunkPtr RetrieveNormalMapTexture( const std::string& sFilename );
	virtual osg::TextureChunkPtr RetrieveNormalMapTextureFromBumpMap( const std::string& sFilename,
																const float nNormalMagnitude,
																const bool bSaveConvertedImage );

		
	virtual osg::CubeTextureChunkPtr RetrieveCubemapTexture( const std::string& sTopFile,
													const std::string& sBottomFile,
													const std::string& sLeftFile,
													const std::string& sRightFile,
													const std::string& sFrontFile,
													const std::string& sBackFile );
	virtual osg::CubeTextureChunkPtr RetrieveCubemapTexture( const std::string& sFolder );

	const std::vector< std::string >& GetImageSearchPathes() const;
	void SetImageSearchPathes( const std::vector< std::string >& vecSearchPathes );
	void AddImageSearchPath( const std::string& sPath );

	const std::vector< std::string >& GetImageFileExtensions() const;
	void SetImageFileExtensions( const std::vector< std::string >& vecExtensions );
	void AddImageFileExtensions( const std::string& sExtension );
protected:
	/**
	 * Searches for the image file with the specified name. Default implementation
	 * finds images with absolute path or relative path to one of the search dirs,
	 * and also searches for different image file extensions
	 */
	virtual std::string GetImageFileLocation( const std::string& sImage );

	/**
	 * Looks for cubemaps in the specified folder. Default implementation looks for files
	 * [pos|neg][x|y|], or [front|back|left|right|bottom|top],
	 * but specialized implementations can extend this
	 */
	virtual bool GetCubemapFileLocations( const std::string& sFolder,
													std::string& sTopFile,
													std::string& sBottomFile,
													std::string& sLeftFile,
													std::string& sRightFile,
													std::string& sFrontFile,
													std::string& sBackFile );

	virtual std::string GetNormalMapFilenameForBumpmap( const std::string& sBumpmapImage, const float nNormalMagnitude ) const;
	virtual std::string GetCubemapIndexString( const std::string& sTopFile,
													const std::string& sBottomFile,
													const std::string& sLeftFile,
													const std::string& sRightFile,
													const std::string& sFrontFile,
													const std::string& sBackFile ) const;
	
private:
	int m_nDefaultAnisotropy;

	std::vector< std::string > m_vecImageFileExtensions;
	std::vector< std::string > m_vecSearchPathes;
	
	std::map< std::string, osg::TextureChunkRefPtr > m_mapLoadedTextures;
	std::map< std::string, osg::TextureChunkRefPtr > m_mapLoadedNormalTextures;
	std::map< std::string, osg::CubeTextureChunkRefPtr > m_mapCubemapTextures;
};

#endif // _VISTAOPENSGTEXTURELOADER_H
