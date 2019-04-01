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


#include <GL/glew.h>

#include "VistaProximityBarrierTape.h"

#include <VistaBase/VistaStreamUtils.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>

#ifdef WIN32
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif

#ifdef VISTA_SYS_OPENSG
#if defined(WIN32)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4312)
#pragma warning(disable: 4267)
#pragma warning(disable: 4275)
#endif

#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaKernel/OpenSG/OSGVistaOpenGLDrawCore.h>

#include <OpenSG/OSGMaterialDrawable.h>
#include <OpenSG/OSGMaterial.h>
#include <OpenSG/OSGDepthChunk.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

namespace
{
	// creates an array containing 32bit-RGBA-data of a BarrierTape
	// nYSlope specifies the relative slope dy/dx, i.e. 1 is 45degree
	// the returned array is allocated on the heap, and should be delete[]-ed
	VistaType::byte* CreateTexture( const int nSize,
										const float nYSlope = 2.0f,
										const float nRelBlackSize = 0.33f )
	{
		VistaType::byte* pBuffer = new VistaType::byte[nSize*nSize];
		std::vector<VistaType::byte> vecBaseRow( nSize );
		const int nMaxBlack = (int)( (float)nSize  * nRelBlackSize );
		for( int i = 0; i < nSize; ++i )
		{
			if( i < nMaxBlack )
				vecBaseRow[i] = 0;
			else
				vecBaseRow[i] = 255;
		}
		VistaType::byte* pVal = pBuffer;
		for( int nY = 0; nY < nSize; ++nY )
		{
			int nRowPos = (int)( (float)nY / nYSlope );
			for( int nX = 0; nX < nSize; ++nX, ++pVal )
			{
				(*pVal) = vecBaseRow[nRowPos];
				if( ++nRowPos == nSize )
					nRowPos = 0;
			}
		}
		return pBuffer;
	}
}

class VistaProximityBarrierTape::DrawCallback : public IVistaOpenGLDraw
{
public:
	DrawCallback()
	: IVistaOpenGLDraw()
	, m_nTapeWidth( 0.2f )
	, m_nTapeOffset( 0 )
	, m_nTapeHeight( 1.5f )
	, m_nWarningLevel( 0.0f )
	, m_nOpacityFactor( 1.0f )
	, m_bFlashState( false )
	{
		glGenTextures( 1, &m_nTextureId );

		glBindTexture( GL_TEXTURE_2D, m_nTextureId );

		const int nRes = 256;
		VistaType::byte* pData = CreateTexture( nRes, 2.5f, 0.45f );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, nRes, nRes, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pData );
		delete[] pData;

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );	
	}

	~DrawCallback()
	{
		glDeleteTextures( 1, &m_nTextureId );
	}

	virtual bool Do() 
	{
		if( m_nWarningLevel <= 0 )
			return false;
		if( m_vecCorners.size() < 2 )
			return false;

		glPushAttrib( GL_ALL_ATTRIB_BITS );
		glBindTexture( GL_TEXTURE_2D, m_nTextureId );
		glEnable( GL_TEXTURE_2D );
		glDisable( GL_LIGHTING );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


		if( m_bFlashState )
			glColor4f( 1.0f, 0.0f, 0.0f, m_nWarningLevel * m_nOpacityFactor );
		else
			glColor4f( 1.0f, 1.0f, 0.0f, m_nWarningLevel * m_nOpacityFactor );

		int nNumRepeats = 0;
		if( m_nTapeOffset != 0 )
			nNumRepeats = 100.0f / m_nTapeOffset;

		for( float nYPos = m_nTapeHeight - nNumRepeats * m_nTapeOffset;
			nYPos <= m_nTapeHeight + nNumRepeats * m_nTapeOffset; ++nYPos )
		{
			glBegin( GL_TRIANGLE_STRIP );
			for( std::size_t i = 0; i < m_vecCorners.size(); ++i )
			{
				glTexCoord2f( m_vecCornerTextureCoordinats[i], 0 );
				glVertex3f( m_vecCorners[i][0], nYPos - 0.5f * m_nTapeWidth, m_vecCorners[i][2] );
				glTexCoord2f( m_vecCornerTextureCoordinats[i], 1 );
				glVertex3f( m_vecCorners[i][0], nYPos + 0.5f * m_nTapeWidth, m_vecCorners[i][2] );
			}
			glEnd();
		}
		glPopAttrib();

		return true;
	}

	void ClearCorners()
	{
		oSize = VistaBoundingBox();
		m_vecCorners.clear();
		m_vecCornerTextureCoordinats.clear();
	}
	void AddCorner( const VistaVector3D& v3Pos )
	{
		if( m_vecCornerTextureCoordinats.empty() )
			m_vecCornerTextureCoordinats.push_back( 0.0f );
		else
		{
			float nVal = m_vecCornerTextureCoordinats.back();
			float nDist = ( v3Pos - m_vecCorners.back() ).GetLength();
			nVal += nDist / m_nTapeWidth;
			m_vecCornerTextureCoordinats.push_back( nVal );
		}

		m_vecCorners.push_back( v3Pos );
		
		oSize.Include( v3Pos );	
		CalcYSize();
	}

	virtual bool GetBoundingBox( VistaBoundingBox &bb ) 
	{
		bb = oSize;
		return true;
	}

	float GetOpacityFactor() const { return m_nOpacityFactor; }
	void SetOpacityFactor( const float& oValue ) { m_nOpacityFactor = oValue; }

	float GetWarningLevel() const { return m_nWarningLevel; }
	void SetWarningLevel( const float& oValue ) { m_nWarningLevel = oValue; }
	
	float GetTapeWidth() const { return m_nTapeWidth; }
	void SetTapeWidth( const float& oValue ) { m_nTapeWidth = oValue; CalcYSize(); }

	float GetTapeHeight() const { return m_nTapeHeight; }
	void SetTapeHeight( const float& oValue ) { m_nTapeHeight = oValue; CalcYSize(); }
	
	float GetTapeOffset() const { return m_nTapeOffset; }
	void SetTapeOffset( const float& oValue ) { m_nTapeOffset = oValue; CalcYSize(); }

	bool GetFlashState() const { return m_bFlashState; }
	void SetFlashState( const bool& oValue ) { m_bFlashState = oValue; }

private:
	void CalcYSize()
	{
		if( m_nTapeOffset > 0 )
		{
			oSize.m_v3Max[Vista::Y] = 1e23f;
			oSize.m_v3Min[Vista::Y] = -1e23f;
		}
		else
		{
			oSize.m_v3Max[Vista::Y] = m_nTapeHeight + 0.5f * m_nTapeHeight;
			oSize.m_v3Min[Vista::Y] = m_nTapeHeight - 0.5f * m_nTapeHeight;
		}
	}

private:
	GLuint m_nTextureId;
	float m_nWarningLevel;
	float m_nOpacityFactor;
	bool m_bFlashState;

	std::vector<VistaVector3D> m_vecCorners;
	std::vector<float> m_vecCornerTextureCoordinats;

	VistaBoundingBox oSize;

	float m_nTapeWidth;
	float m_nTapeHeight;
	float m_nTapeOffset;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaProximityBarrierTape::VistaProximityBarrierTape( VistaEventManager* pManager,
													 const float nBeginWarningDistance,
													 const float nMaxWarningDistance,
													 const bool bDisableOcclusion,
													 VistaGraphicsManager* pGraphicsManager )
: IVistaProximityWarningBase( pManager, nBeginWarningDistance, nMaxWarningDistance )
, m_pDraw( new DrawCallback() )
, m_eHeightMode( HM_FIXED_HEIGHT )
, m_nTapeHeight( 0 )
{
	VistaSceneGraph* pSG = pGraphicsManager->GetSceneGraph();
	m_pDrawNode = pSG->NewOpenGLNode( pSG->GetRoot(), m_pDraw );
	m_pDrawNode->SetIsEnabled( false );	

	if( bDisableOcclusion )
	{
#ifdef VISTA_SYS_OPENSG
		VistaOpenSGOpenGLNodeData* pData = dynamic_cast<VistaOpenSGOpenGLNodeData*>(
														m_pDrawNode->GetData() );
		assert( pData );
				
		osg::VistaOpenGLDrawCorePtr pCore = osg::VistaOpenGLDrawCorePtr::dcast(
															pData->GetCore() );
		osg::ChunkMaterialPtr pMaterial = osg::ChunkMaterialPtr::dcast(
															pCore->getMaterial() );
		if( pMaterial == osg::NullFC )
		{
			// create new one
			pMaterial = osg::ChunkMaterial::create();
			pCore->setMaterial( pMaterial );
		}	
		osg::DepthChunkPtr pDepthCunk = osg::DepthChunk::create();
		beginEditCP( pDepthCunk );
		pDepthCunk->setEnable( false ); //disable depth testing
		endEditCP( pDepthCunk );
		beginEditCP( pMaterial );
		pMaterial->setSortKey( 42 );
		pMaterial->addChunk( pDepthCunk );
		endEditCP( pMaterial );
#endif
	}
}

VistaProximityBarrierTape::~VistaProximityBarrierTape()
{
	delete m_pDrawNode;
	delete m_pDraw;
}

bool VistaProximityBarrierTape::DoUpdate( const float nMinDistance,
										 const float nWarningLevel,
										 const VistaVector3D& v3PointOnBounds, 
										 const VistaVector3D& v3UserPosition,
										 const VistaQuaternion& qUserOrientation )
{
	if( nWarningLevel <= 0 )
	{
		m_pDrawNode->SetIsEnabled( false );
	}
	else
	{
		bool bChange = !m_pDrawNode->GetIsEnabled();
		m_pDrawNode->SetIsEnabled( true );
		m_pDraw->SetWarningLevel( nWarningLevel );
		switch( m_eHeightMode )
		{
			case HM_FIXED_HEIGHT:
				if( bChange )
					m_pDraw->SetTapeHeight( m_nTapeHeight );
				break;
			case HM_ADJUST_TO_VIEWER_POS_AT_START:
				if( bChange )
					m_pDraw->SetTapeHeight( v3UserPosition[Vista::Y] + m_nTapeHeight );
				break;
			case HM_ADJUST_TO_VIEWER_POS:
				m_pDraw->SetTapeHeight(  v3UserPosition[Vista::Y] + m_nTapeHeight );
				break;
			case HM_ADJUST_TO_OBJECT_POS_AT_START:
				if( bChange )
					m_pDraw->SetTapeHeight( v3PointOnBounds[Vista::Y] + m_nTapeHeight );
				break;
			case HM_ADJUST_TO_OBJECT_POS:
				m_pDraw->SetTapeHeight( v3PointOnBounds[Vista::Y] + m_nTapeHeight );
				break;
		}
	}
	return true;
}

void VistaProximityBarrierTape::AddHalfPlane( const VistaVector3D& v3Center, const VistaVector3D& v3Normal )
{
	// @todo: calc points?
	IVistaProximityWarningBase::AddHalfPlane( v3Center, v3Normal );
}

void VistaProximityBarrierTape::SetUseExtents( const VistaBoundingBox& bbExtents,
											  const VistaQuaternion& qBoxRotation )
{
	VistaVector3D v3Center = bbExtents.GetCenter();
	float nXSize = 0.5f * bbExtents.GetSize()[Vista::X] - m_nDistanceFromWall;
	float nZSize = 0.5f * bbExtents.GetSize()[Vista::Z] - m_nDistanceFromWall;
	m_pDraw->ClearCorners();
	m_pDraw->AddCorner( v3Center + qBoxRotation.Rotate( VistaVector3D( -nXSize, 0, nZSize ) ) );
	m_pDraw->AddCorner( v3Center + qBoxRotation.Rotate( VistaVector3D( nXSize, 0, nZSize ) ) );
	m_pDraw->AddCorner( v3Center + qBoxRotation.Rotate( VistaVector3D( nXSize, 0, -nZSize ) ) );
	m_pDraw->AddCorner( v3Center + qBoxRotation.Rotate( VistaVector3D( -nXSize, 0, -nZSize ) ) );
	// add first pos again to close loop
	m_pDraw->AddCorner( v3Center + qBoxRotation.Rotate( VistaVector3D( -nXSize, 0, nZSize ) ) );

	IVistaProximityWarningBase::SetUseExtents( bbExtents, qBoxRotation );
}

void VistaProximityBarrierTape::SetParentNode( VistaGroupNode* pParent )
{
	pParent->AddChild( m_pDrawNode );
}

float VistaProximityBarrierTape::GetTapeHeight() const
{
	return m_nTapeHeight;
}

void VistaProximityBarrierTape::SetTapeHeight( const float nHeight )
{
	m_nTapeHeight = nHeight;
}

float VistaProximityBarrierTape::GetTapeSpacing() const
{
	return m_pDraw->GetTapeOffset();
}

void VistaProximityBarrierTape::SetTapeSpacing( const float nSpacing )
{
	m_pDraw->SetTapeOffset( nSpacing );
}

float VistaProximityBarrierTape::GetTapeWidth() const
{
	return m_pDraw->GetTapeWidth();
}

void VistaProximityBarrierTape::SetTapeWidth( const float nWidth ) const
{
	m_pDraw->SetTapeWidth( nWidth );
}

VistaProximityBarrierTape::HeightMode VistaProximityBarrierTape::GetHeightMode() const
{
	return m_eHeightMode;
}

void VistaProximityBarrierTape::SetHeightMode( const HeightMode oValue )
{
	m_eHeightMode = oValue;
}

float VistaProximityBarrierTape::GetDistanceFromWall() const
{
	return m_nDistanceFromWall;
}

void VistaProximityBarrierTape::SetDistanceFromWall( const float oValue )
{
	m_nDistanceFromWall = oValue;
}

bool VistaProximityBarrierTape::GetIsEnabled() const
{
	return m_pDrawNode->GetIsEnabled();
}

bool VistaProximityBarrierTape::SetIsEnabled( const bool bSet )
{
	m_pDrawNode->SetIsEnabled( bSet );
	return true;
}

bool VistaProximityBarrierTape::DoTimeUpdate( VistaType::systemtime nTime, const float nOpacityScale, const bool bFlashState )
{
	m_pDraw->SetOpacityFactor( nOpacityScale );
	m_pDraw->SetFlashState( bFlashState );
	return true;
}
