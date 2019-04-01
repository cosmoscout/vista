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


#if defined(WIN32)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4275)
#pragma warning(disable: 4267)
#pragma warning(disable: 4251)
#pragma warning(disable: 4231)
#endif

#include "VistaKernelOpenSGExtConfig.h"
#include "VistaOpenSGNormalMapMaterial.h"

#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/GraphicsManager/VistaLightNode.h>

#include <VistaTools/VistaRandomNumberGenerator.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>

#ifdef WIN32
#pragma
#endif

#include <OpenSG/OSGChunkMaterial.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGTextureChunk.h>
#include <OpenSG/OSGSHLChunk.h>
#include <OpenSG/OSGImageFunctions.h>
#include <OpenSG/OSGImageFileHandler.h>
#include <OpenSG/OSGGeoFunctions.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGraphOp.h>
#include <OpenSG/OSGVerifyGeoGraphOp.h>
#include <OpenSG/OSGVerifyGraphOp.h>
#include <OpenSG/OSGNode.h>

using namespace OSG;
static int iBaseMapUnit   = 2;
static int iNormalMapUnit = 3;

namespace
{
	float GetLocalTexCoord( const float nCoord, GLenum eTextureWrapMode )
	{
		// @TODO: extended wrap modes
		float nRes = nCoord;
		switch( eTextureWrapMode )
		{
			case GL_CLAMP_TO_EDGE:
			{
				nRes = Vista::Clamp( nCoord, 0.0f, 1.0f );
				break;
			}
			case GL_REPEAT:
			{
				if( nCoord < 0 )
					nRes = 1.0f - ( -nCoord - std::floor( -nCoord ) ); 
				else if( nCoord > 1 )
					nRes = nCoord - std::floor( nCoord );
				break;
			}
			default:
				VISTA_THROW( "invalid or unsupported texture wrap mode", -1 );
		}
		return nRes;
	}
	template<typename TInternal>
	VistaColor GetPixelEntry( const int nX, const int nY, const int nSizeX, const int nSizeY,
						const int nNumComponents, osg::UInt8* pData, const bool bIsIntegerValue = true )
	{
		TInternal* pTypedData = reinterpret_cast<TInternal*>( pData );
		assert( nX >= 0 && nX < nSizeX );
		assert( nY >= 0 && nY < nSizeY );
		int nIndex = nX + nY * nSizeX;
		int nOffset = nNumComponents * nIndex;
		VistaColor oResult;
		float nScale = 1.0f;
		if( bIsIntegerValue )
			nScale = 1.0f / (float)std::numeric_limits<TInternal>::max();
		switch( nNumComponents )
		{
			case 1:
			{
				float nValue = nScale * (float)pTypedData[nOffset];
				oResult.SetRed( nValue );
				oResult.SetGreen( nValue );
				oResult.SetBlue( nValue );
				oResult.SetAlpha( 1.0f );
				break;
			}
			case 2:
			{
				float nValue = nScale * (float)pTypedData[nOffset];
				oResult.SetRed( nValue );
				oResult.SetGreen( nValue );
				oResult.SetBlue( nValue );
				oResult.SetAlpha( nScale * (float)pTypedData[nOffset + 1] );
				break;
			}
			case 3:
			{
				oResult.SetRed( nScale * (float)pTypedData[nOffset] );
				oResult.SetGreen( nScale * (float)pTypedData[nOffset + 1] );
				oResult.SetBlue( nScale * (float)pTypedData[nOffset + 2] );
				oResult.SetAlpha( 1.0f );
				break;
			}
			case 4:
			{
				oResult.SetRed( nScale * (float)pTypedData[nOffset] );
				oResult.SetGreen( nScale * (float)pTypedData[nOffset + 1] );
				oResult.SetBlue( nScale * (float)pTypedData[nOffset + 2] );
				oResult.SetAlpha( nScale * (float)pTypedData[nOffset + 3] );
				break;
			}
			default:
				VISTA_THROW( "Invalid number of components in pixel", -1 );
		}
		return oResult;

	}
	bool GetEntryFromTexture( const float nTexCoordX, const float nTexCoordY, osg::TextureChunkPtr pTexture, VistaColor& oResult )
	{
		ImagePtr pImage = pTexture->getImage();

		float nLocalCoordS = GetLocalTexCoord( nTexCoordX, pTexture->getWrapS() );
		float nLocalCoordT = GetLocalTexCoord( nTexCoordY, pTexture->getWrapT() );
		int nSizeX = pImage->getWidth();
		int nSizeY = pImage->getHeight();

		float nXPos = nLocalCoordS * (float)( nSizeX - 1 );
		int nPixelLowerX = Vista::Clamp( (int)std::floor( nXPos ), 0, nSizeX - 1 );
		int nPixelUpperX = Vista::Clamp( (int)std::ceil( nXPos ), 0, nSizeX - 1 );
		float nInterpolateX = 1.0f - ( nXPos - nPixelLowerX );
		float nYPos = nLocalCoordT * (float)( nSizeY - 1 );
		int nPixelLowerY = Vista::Clamp( (int)std::floor( nYPos ), 0, nSizeY - 1 );
		int nPixelUpperY = Vista::Clamp( (int)std::ceil( nYPos ), 0, nSizeY - 1 );
		float nInterpolateY = 1.0f - ( nYPos - nPixelLowerY );

		int nNumComponents;
		switch( pImage->getPixelFormat() )
		{
			case osg::Image::OSG_A_PF:
			case osg::Image::OSG_I_PF:
			case osg::Image::OSG_L_PF:
				nNumComponents = 1;
				break;
			case osg::Image::OSG_LA_PF:
				nNumComponents = 2;
				break;
			case osg::Image::OSG_RGB_PF:
				nNumComponents = 3;
				break;
			case osg::Image::OSG_RGBA_PF:
				nNumComponents = 4;
				break;
			default:
				vstr::warnp() << "VistaOpenSGNormalMapMaterial - unsupported pixel format for color retrieval" << std::endl;
				return false;
		}

		osg::Int32 nDatatype = pImage->getDataType();
		osg::UInt8* pData = pImage->getData();

		VistaColor colBottomLeft, colBottomRight, colTopLeft, colTopRight;
		int nRenormalizeSize = 1;
		switch( nDatatype )
		{
			case osg::Image::OSG_UINT8_IMAGEDATA:
				colBottomLeft = GetPixelEntry<osg::UInt8>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colBottomRight = GetPixelEntry<osg::UInt8>( nPixelUpperX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colTopLeft = GetPixelEntry<osg::UInt8>( nPixelLowerX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData );
				colTopRight = GetPixelEntry<osg::UInt8>( nPixelUpperX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData );
				break;
			case osg::Image::OSG_UINT16_IMAGEDATA:
				colBottomLeft = GetPixelEntry<osg::UInt16>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colBottomRight = GetPixelEntry<osg::UInt16>( nPixelUpperX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colTopLeft = GetPixelEntry<osg::UInt16>( nPixelLowerX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData );
				colTopRight = GetPixelEntry<osg::UInt16>( nPixelUpperX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData );
				break;
			case osg::Image::OSG_UINT32_IMAGEDATA:
				colBottomLeft = GetPixelEntry<osg::UInt32>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colBottomRight = GetPixelEntry<osg::UInt32>( nPixelUpperX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colTopLeft = GetPixelEntry<osg::UInt32>( nPixelLowerX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData );
				colTopRight = GetPixelEntry<osg::UInt32>( nPixelUpperX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData );
				break;
			case osg::Image::OSG_FLOAT16_IMAGEDATA:
				colBottomLeft = GetPixelEntry<osg::Real16>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colBottomRight = GetPixelEntry<osg::Real16>( nPixelUpperX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colTopLeft = GetPixelEntry<osg::Real16>( nPixelLowerX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData );
				colTopRight = GetPixelEntry<osg::Real16>( nPixelUpperX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData );
				break;
			case osg::Image::OSG_FLOAT32_IMAGEDATA:
				colBottomLeft = GetPixelEntry<osg::Real32>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData, false );
				colBottomRight = GetPixelEntry<osg::Real32>( nPixelUpperX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData, false );
				colTopLeft = GetPixelEntry<osg::Real32>( nPixelLowerX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData, false );
				colTopRight = GetPixelEntry<osg::Real32>( nPixelUpperX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData, false );
				break;
			case osg::Image::OSG_INT16_IMAGEDATA:
				colBottomLeft = GetPixelEntry<osg::Int16>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData, false );
				colBottomRight = GetPixelEntry<osg::Int16>( nPixelUpperX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData, false );
				colTopLeft = GetPixelEntry<osg::Int16>( nPixelLowerX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData, false );
				colTopRight = GetPixelEntry<osg::Int16>( nPixelUpperX, nPixelUpperY, nSizeX, nSizeY, nNumComponents, pData, false );
				break;
			case osg::Image::OSG_INT32_IMAGEDATA:
				colBottomLeft = GetPixelEntry<osg::Int32>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colBottomRight = GetPixelEntry<osg::Int32>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colTopLeft = GetPixelEntry<osg::Int32>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				colTopRight = GetPixelEntry<osg::Int32>( nPixelLowerX, nPixelLowerY, nSizeX, nSizeY, nNumComponents, pData );
				break;
			default:
				VISTA_THROW( "invalid pixel data type for image", -1 );
		};

		VistaColor colBottom = colBottomLeft.Mix( colBottomRight, nInterpolateX );
		VistaColor colTop = colTopLeft.Mix( colTopRight, nInterpolateX );
		oResult = colBottom.Mix( colTop, nInterpolateY );
		oResult /= (float)nRenormalizeSize;
		return true;
	}
}


class VistaOpenSGNormalMapMaterial::NativeData
{
public:

	// shader parameters
	Vec4f fvAmbient;
	Vec4f fvDiffuse;
	Vec4f fvSpecular;
	Real32 fSpecularPower;
	Vec3f fvLightPosition;

	// data
	ChunkMaterialRefPtr material;
	SHLChunkRefPtr shl;
	TextureChunkRefPtr nmap;
	TextureChunkRefPtr bmap;


	NativeData()
		// OpenGL default colors
		: fvAmbient (Vec4f(0.2f, 0.2f, 0.2f, 1.0f))
		, fvDiffuse (Vec4f(0.8f, 0.8f, 0.8f, 1.0f))
		, fvSpecular(Vec4f(0.0f, 0.0f, 0.0f, 1.0f))
		, fSpecularPower(50.0)
		, fvLightPosition(Vec3f(0, 0, 1))
		, material(ChunkMaterial::create())
		, shl(SHLChunk::create())
		, nmap(TextureChunk::create())
		, bmap(TextureChunk::create())
	{
		/** @todo only create on explicit user request!? */
		ImageRefPtr ptrDefaultMap;
		if(ptrDefaultMap == NullFC)
		{
			ptrDefaultMap = Image::create();
			createNoise(ptrDefaultMap, Image::OSG_L_PF, 6, 256);
			createNormalMapFromBump(ptrDefaultMap, NullFC, Vec3f(2, 2, 1));
		}

		beginEditCP(material);
			material->clearChunks();
			material->addChunk(shl);
			material->addChunk(nmap, iNormalMapUnit);
			material->addChunk(bmap, iBaseMapUnit);
		endEditCP  (material);

		beginEditCP(shl);
			shl->setVertexProgram  (_vs_normalmap);
			shl->setFragmentProgram(_fs_normalmap);
			shl->setUniformParameter("normalMap", iNormalMapUnit);
		endEditCP  (shl);

		beginEditCP(nmap);
			// gcc likes to have it this way, as osg does not qualify
			// the imageptr as const ref, but only ref
			osg::ImagePtr im = ptrDefaultMap.get();
			nmap->setImage(im);
			nmap->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
			nmap->setMagFilter(GL_LINEAR);
			nmap->setWrapS(GL_REPEAT);
			nmap->setWrapT(GL_REPEAT);
		endEditCP  (nmap);

		updateUniforms();
	}

	virtual ~NativeData()
	{
		//as we use refptrs, we dont need to cleanup....
		//only the globally used default map will stay alive as long as a NormalMapMaterial is alive
		//subRefCP(sPtrDefaultMap);
	}


	void setNormalMap(ImagePtr img)
	{
		beginEditCP(nmap, TextureChunk::ImageFieldMask);
			nmap->setImage(img);
		endEditCP  (nmap, TextureChunk::ImageFieldMask);
		//img->write("nmap_out.png");
	}

	void setBaseMap(ImagePtr img)
	{
		ImageRefPtr ref(img);
		// remove basemap
		if(img == NullFC)
		{
			beginEditCP(material, ChunkMaterial::ChunksFieldMask);
				material->clearChunks();
				material->addChunk(shl);
				material->addChunk(nmap, iNormalMapUnit);
			endEditCP  (material, ChunkMaterial::ChunksFieldMask);

			beginEditCP(shl, SHLChunk::FragmentProgramFieldMask
						   | SHLChunk::ParametersFieldMask);
				shl->setFragmentProgram(_fs_normalmap);
				shl->subUniformParameter("baseMap");
			endEditCP  (shl, SHLChunk::FragmentProgramFieldMask
						   | SHLChunk::ParametersFieldMask);
			updateUniforms();
			return;
		}
		//img->write("bmap_out.png");

		if(!bmap)
		{
			bmap = TextureChunk::create();
			beginEditCP(bmap);
				if(img->getMipMapCount() > 1)
				{
					bmap->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
				}
				else
				{
					bmap->setMinFilter(GL_LINEAR);
				}
				bmap->setMagFilter(GL_LINEAR);
				bmap->setWrapS(GL_REPEAT);
				bmap->setWrapT(GL_REPEAT);
			endEditCP  (bmap);
		}

		beginEditCP(bmap, TextureChunk::ImageFieldMask);
			bmap->setImage(img);
		endEditCP  (bmap, TextureChunk::ImageFieldMask);

		beginEditCP(material, ChunkMaterial::ChunksFieldMask);
			material->clearChunks();
			material->addChunk(shl);
			material->addChunk(nmap, iNormalMapUnit);
			material->addChunk(bmap, iBaseMapUnit);
		endEditCP  (material, ChunkMaterial::ChunksFieldMask);

		beginEditCP(shl, SHLChunk::FragmentProgramFieldMask
					   | SHLChunk::ParametersFieldMask);
			shl->setFragmentProgram(_fs_texturednormalmap);
			shl->setUniformParameter("normalMap", iNormalMapUnit);
			shl->setUniformParameter("baseMap", iBaseMapUnit);
		endEditCP  (shl, SHLChunk::FragmentProgramFieldMask
					   | SHLChunk::ParametersFieldMask);

		updateUniforms();
	}


	void updateUniforms()
	{
		beginEditCP(shl, SHLChunk::ParametersFieldMask);
			shl->setUniformParameter("fvAmbient", fvAmbient);
			shl->setUniformParameter("fvDiffuse", fvDiffuse);
			shl->setUniformParameter("fvSpecular", fvSpecular);
			shl->setUniformParameter("fSpecularPower", fSpecularPower);
			shl->setUniformParameter("fvLightPosition", fvLightPosition);
		endEditCP  (shl, SHLChunk::ParametersFieldMask);
	}


	// in eye space
	void setLightPosition(const Vec3f &p)
	{
		fvLightPosition = p;
		beginEditCP(shl, SHLChunk::ParametersFieldMask);
			shl->setUniformParameter("fvLightPosition", fvLightPosition);
		endEditCP  (shl, SHLChunk::ParametersFieldMask);
	}

	//// DEBUG
	//void overrideShaders(const char *vpFile, const char *fpFile)
	//{
	//	beginEditCP(shl);
	//		shl->readVertexProgram  (vpFile);
	//		shl->readFragmentProgram(fpFile);
	//		shl->setUniformParameter("normalMap", iNormalMapUnit);
	//		shl->setUniformParameter(  "baseMap",   iBaseMapUnit);
	//	endEditCP(shl);
	//	updateUniforms();
	//}

};

//ImagePtr VistaOpenSGNormalMapMaterial::CNativeData::sPtrDefaultMap;

//// DEBUG INTERFACE
//void VistaOpenSGNormalMapMaterial::OverrideShaders(const char *vpFile, const char *fpFile) const
//{
//	m_pData->overrideShaders(vpFile, fpFile);
//}


VistaOpenSGNormalMapMaterial::VistaOpenSGNormalMapMaterial(VistaEventManager *pEvMgr)
: m_pData(new VistaOpenSGNormalMapMaterial::NativeData)
, m_pTrackedLight(NULL)
, m_pEvMgr(pEvMgr)
{
}

VistaOpenSGNormalMapMaterial::~VistaOpenSGNormalMapMaterial()
{
	if(m_pEvMgr->GetIsObserver(this, VistaSystemEvent::GetTypeId()))
		m_pEvMgr->UnregisterObserver(this, VistaSystemEvent::GetTypeId());

	delete m_pData;
}

void VistaOpenSGNormalMapMaterial::ApplyToGeometry(VistaGeometry *pGeo, bool generateTangents)
{
	VistaOpenSGGeometryData *pData = static_cast<VistaOpenSGGeometryData*>(pGeo->GetData());
	GeometryPtr ptrGeo = pData->GetGeometry();
	if(generateTangents)
	{
		// create temp node for "traversal"
		osg::NodeRefPtr node(makeNodeFor(ptrGeo));

		// run geometry verifier
		VerifyGraphOp gop;
		gop.setRepair(true);
		gop.setVerbose(true);

		// gcc likes to have it this way:
		osg::NodePtr nd = node.get();
		if(gop.traverse(nd))
		{
			// create single indexed geometry, as calculating binormals for
			// multi-indexed geo is non-trivial and not implemented for OpenSG
			createSingleIndex(ptrGeo);
			// calculate vertex tangents and binormals and sort them as
			// secondary and tertiary texture coordinates
			// -> as OpenSG does not support generic vertex attributes in 1.8
			calcVertexTangents(ptrGeo);
		}
		else
		{
			vstr::errp() << "[VistaOpenSGNormalMapMaterial] - invalid geometry detected, cannot generate vertex tangents for normal mapping!" << std::endl;
		}
	}

	// if the old material had a MaterialChunk, get the values and use them in the shader
	ChunkMaterialRefPtr ptrOldMat(
		ChunkMaterialPtr::dcast(ptrGeo->getMaterial()));

	MaterialChunkRefPtr ptrMatChunk(
		MaterialChunkPtr::dcast(ptrOldMat->find(MaterialChunk::getClassType())));
	if(ptrMatChunk)
	{
		Color4f c;
		c =	ptrMatChunk->getAmbient();
		SetAmbientColor(c[0], c[1], c[2], c[3]);

		c =	ptrMatChunk->getDiffuse();
		SetDiffuseColor(c[0], c[1], c[2], c[3]);

		c =	ptrMatChunk->getSpecular();
		SetSpecularColor(c[0], c[1], c[2], c[3]);

		Real32 shine = ptrMatChunk->getShininess();
		SetSpecularPower(shine);
	}

	beginEditCP(ptrGeo, Geometry::MaterialFieldMask);
		ptrGeo->setMaterial(m_pData->material);
	endEditCP  (ptrGeo, Geometry::MaterialFieldMask);
}

void VistaOpenSGNormalMapMaterial::SetAmbientColor(const float &r, const float &g,
													const float &b, const float &a) const
{
	m_pData->fvAmbient.setValues(r, g, b, a);
	m_pData->updateUniforms();
}

void VistaOpenSGNormalMapMaterial::SetDiffuseColor(const float &r, const float &g,
													const float &b, const float &a) const
{
	m_pData->fvDiffuse.setValues(r, g, b, a);
	m_pData->updateUniforms();
}

void VistaOpenSGNormalMapMaterial::SetSpecularColor(const float &r, const float &g,
													const float &b, const float &a) const
{
	m_pData->fvSpecular.setValues(r, g, b, a);
	m_pData->updateUniforms();
}

void VistaOpenSGNormalMapMaterial::SetSpecularPower(const float &ex) const
{
	m_pData->fSpecularPower = ex;
	m_pData->updateUniforms();
}

bool VistaOpenSGNormalMapMaterial::SetNormalMap(const char *pFileName, bool convertFromBumpMap) const
{
	// read image
	ImageRefPtr img(ImageFileHandler::the().read(pFileName));
	if(!img) return false;


	if(convertFromBumpMap)
	{
		// convert to greyscale (heightmap)
		if(img->getPixelFormat() != Image::OSG_L_PF)
		{
			beginEditCP(img);
			img->reformat(Image::OSG_L_PF);
			endEditCP(img);
		}

		if(!createNormalMapFromBump(img, NullFC, Vec3f(1.0f, 1.0f, 1.0f)))
		{
			return false;
		}
	}

	m_pData->setNormalMap(img);

	return true;
}

bool VistaOpenSGNormalMapMaterial::SetBaseMap(const char *pFileName) const
{
	if(!pFileName)
	{
		// remove base texture from material
		m_pData->setBaseMap(NullFC);
	}
	else
	{
		ImageRefPtr ptrImg(ImageFileHandler::the().read(pFileName));
		m_pData->setBaseMap(ptrImg);
		if(!ptrImg) return false;
	}
	return true;
}


void VistaOpenSGNormalMapMaterial::SetLightPosition(const float &x, const float &y, const float &z) const
{
	m_pData->setLightPosition(osg::Vec3f(x, y, z));
}

void VistaOpenSGNormalMapMaterial::SetVistaLight(VistaLightNode *pObj)
{
	if(m_pEvMgr == NULL)
	{
		vstr::warnp() << "[VistaOpenSGNormalMapMaterial]: trying to register light node, but no event manager registered" << std::endl;
		return;
	}

	m_pTrackedLight = pObj;

	//attach to event manager
	if(!m_pEvMgr->GetIsObserver(this, VistaSystemEvent::GetTypeId()))
		m_pEvMgr->RegisterObserver(this, VistaSystemEvent::GetTypeId());
}

void VistaOpenSGNormalMapMaterial::Notify(const VistaEvent *pEvent)
{
	if(pEvent->GetId() != VistaSystemEvent::VSE_POSTAPPLICATIONLOOP)
		return;

	if(!m_pTrackedLight)
	{
		// if there's no light to track anymore, detach from event manager
		m_pEvMgr->UnregisterObserver(this, VistaSystemEvent::GetTypeId());
		// finished
		return;
	}

	VistaVector3D lpos(0,0,0);
	switch( m_pTrackedLight->GetLightType() )
	{
		case VISTA_POINT_LIGHT:
		{
			VistaPointLight *light = dynamic_cast<VistaPointLight*>(m_pTrackedLight);
			lpos = light->GetPosition();
			break;
		}
		case VISTA_SPOT_LIGHT:
		{
			VistaSpotLight *light = dynamic_cast<VistaSpotLight*>(m_pTrackedLight);
			lpos = light->GetPosition();
			break;
		}
		case VISTA_DIRECTIONAL_LIGHT:
		{
			VistaDirectionalLight *light = dynamic_cast<VistaDirectionalLight*>(m_pTrackedLight);
			lpos = light->GetDirection();
			lpos.Normalize();
			// approximate distant light
			lpos *= 10000.0;
			break;
		}
		default:
			break;

	}


	VistaTransformMatrix m;
	m_pTrackedLight->GetWorldTransform(m);

	VistaVector3D pos = m * lpos;
	SetLightPosition(pos[0], pos[1], pos[2]);
}

bool VistaOpenSGNormalMapMaterial::GetBaseMapEntry( const float nTexCoordX, const float nTexCoordY, VistaColor& oResult )
{
	return GetEntryFromTexture( nTexCoordX, nTexCoordY, m_pData->bmap, oResult );
}

bool VistaOpenSGNormalMapMaterial::GetNormalMapEntry( const float nTexCoordX, const float nTexCoordY, VistaColor& oResult )
{
	return GetEntryFromTexture( nTexCoordX, nTexCoordY, m_pData->nmap, oResult );
}


