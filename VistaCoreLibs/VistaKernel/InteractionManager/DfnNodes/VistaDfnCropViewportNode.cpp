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

#include "VistaDfnCropViewportNode.h" 

#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/DisplayManager/VistaProjection.h>
#include <VistaAspects/VistaObserver.h>

#ifdef WIN32	
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaDfnCropViewportNode::CropViewportOverlay : public IVistaSceneOverlay
{
private:
	class ProjectionObserver : public IVistaObserver
	{
	public:
		ProjectionObserver( CropViewportOverlay *pOverlay, VistaProjection* pProjection )
		: IVistaObserver()
		, m_pProjectionProps( pProjection->GetProjectionProperties() )
		, m_pOverlay( pOverlay )
		{
			m_pProjectionProps->AttachObserver( this );
		}

		~ProjectionObserver()
		{
			m_pProjectionProps->DetachObserver( this );
		}

		virtual bool ObserveableDeleteRequest( IVistaObserveable* pObserveable,
											int nTicket = IVistaObserveable::TICKET_NONE )
		{
			return true;
		}

		virtual void ObserveableDelete( IVistaObserveable* pObserveable,
											int nTicket = IVistaObserveable::TICKET_NONE )
		{
			if( pObserveable == static_cast<IVistaObserveable*>( m_pProjectionProps ) )
				m_pProjectionProps = NULL;
		}

		virtual void ReleaseObserveable( IVistaObserveable* pObserveable,
											int nTicket = IVistaObserveable::TICKET_NONE )
		{
			if( pObserveable == static_cast<IVistaObserveable*>( m_pProjectionProps ) )
				m_pProjectionProps = NULL;
		}

		virtual void ObserverUpdate( IVistaObserveable* pObserveable,
											int nMsg, int nTicket )
		{
			if( nMsg == VistaProjection::VistaProjectionProperties::MSG_PROBABLY_ALL_CHANGED
				|| nMsg == VistaProjection::VistaProjectionProperties::MSG_PROJ_PLANE_EXTENTS_CHANGE
				|| nMsg == VistaProjection::VistaProjectionProperties::MSG_PROJ_PLANE_MIDPOINT_CHANGE )
			{
				m_pOverlay->RecalculateDrawArea();
			}
		}

		virtual bool Observes( IVistaObserveable* pObserveable )
		{
			return ( pObserveable == static_cast<IVistaObserveable*>( m_pProjectionProps ) );
		}

		virtual void Observe( IVistaObserveable* pObserveable, 
											int nTicket = IVistaObserveable::TICKET_NONE )
		{
			// won't happen
		}

	private:
		CropViewportOverlay*		m_pOverlay;
		VistaProjection::VistaProjectionProperties*	m_pProjectionProps;
	};

public:
	CropViewportOverlay( VistaViewport* pVp, bool bUseProjExtents, const VistaColor& oCropColor )
	: IVistaSceneOverlay( pVp )
	, m_pProjObserver( NULL )
	, m_oCropColor( oCropColor )
	, m_nWidth( 0 )
	, m_nHeight( 0 )
	, m_nCropLeft( 0 )
	, m_nCropRight( 0 )
	, m_nCropTop( 0 )
	, m_nCropBottom( 0 )
	, m_nDrawLeft( 0 )
	, m_nDrawRight( 0 )
	, m_nDrawTop( 0 )
	, m_nDrawBottom( 0 )
	, m_bEnabled( true )
	{
		VistaViewport* pAttachedViewport = GetAttachedViewport();
		if( pAttachedViewport )
		{
			if( bUseProjExtents )
				m_pProjObserver = new ProjectionObserver( this, pAttachedViewport->GetProjection() );
			pAttachedViewport->GetViewportProperties()->GetSize( m_nWidth, m_nHeight );
			RecalculateDrawArea();
		}
	}

	~CropViewportOverlay()
	{
		delete m_pProjObserver;
	}

	virtual bool GetIsEnabled() const
	{
		return m_bEnabled;
	}

	virtual void SetIsEnabled( bool bEnabled ) 
	{
		m_bEnabled = bEnabled;
	}

	virtual void UpdateOnViewportChange( int iWidth, int iHeight, int iPosX, int iPosY ) 
	{
		m_nWidth = iWidth;
		m_nHeight = iHeight;
		RecalculateDrawArea();
	}

	virtual bool Do()
	{
		if( m_nDrawLeft >= m_nDrawRight || m_nDrawBottom >= m_nDrawTop )
			return false;

		glPushAttrib( GL_ALL_ATTRIB_BITS );
		glDisable( GL_LIGHTING );
		glDisable( GL_DEPTH_TEST );

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D( 0, m_nWidth, 0, m_nHeight );

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glColor4fv( &m_oCropColor[0] );
		glBegin( GL_QUADS );
			glVertex2f( m_nDrawLeft, m_nDrawBottom );
			glVertex2f( m_nDrawLeft, m_nDrawTop );
			glVertex2f( m_nDrawRight, m_nDrawTop );
			glVertex2f( m_nDrawRight, m_nDrawBottom );
		glEnd();

		glPopMatrix();

		glMatrixMode( GL_PROJECTION );
		glPopMatrix();

		glPopAttrib();

		glMatrixMode(GL_MODELVIEW);

		return true;
	}

	void SetCropProjExtents( float nLeft, float nRight, float nBottom, float nTop )
	{
		m_nCropLeft = nLeft;
		m_nCropRight = nRight;
		m_nCropBottom = nBottom;
		m_nCropTop = nTop;
		RecalculateDrawArea();
	}

	void RecalculateDrawArea()
	{
		if( m_pProjObserver ) // determines whether to use projection extents or viewport
		{
			VistaProjection::VistaProjectionProperties* pProj =
						GetAttachedViewport()->GetProjection()->GetProjectionProperties();

			double nProjLeft, nProjRight, nProjBottom, nProjTop;
			pProj->GetProjPlaneExtents( nProjLeft, nProjRight, nProjBottom, nProjTop );

			float nProjWidth = (float)( nProjRight - nProjLeft );
			float nProjHeight = (float)( nProjTop - nProjBottom );

			m_nDrawLeft = (int)( m_nWidth / nProjWidth * ( m_nCropLeft - (float)nProjLeft ) );
			m_nDrawRight = (int)( m_nWidth - m_nWidth / nProjWidth* ( (float)nProjRight - m_nCropRight ) );
			m_nDrawBottom = (int)( m_nHeight / nProjHeight * ( m_nCropBottom - (float)nProjBottom ) );
			m_nDrawTop = (int)( m_nHeight - m_nHeight / nProjHeight * ( (float)nProjTop - m_nCropTop ) );
		}
		else
		{
			m_nDrawLeft = m_nWidth * m_nCropLeft;
			m_nDrawRight = m_nWidth * m_nCropRight;
			m_nDrawBottom = m_nHeight * m_nCropBottom;
			m_nDrawTop = m_nHeight * m_nCropTop;
		}
	}

private:
	

	int m_nWidth;
	int m_nHeight;

	float m_nCropLeft;
	float m_nCropRight;
	float m_nCropTop;
	float m_nCropBottom;

	int m_nDrawLeft;
	int m_nDrawRight;
	int m_nDrawTop;
	int m_nDrawBottom;

	bool m_bEnabled;

	VistaColor m_oCropColor;

	ProjectionObserver* m_pProjObserver;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaDfnCropViewportNode::VistaDfnCropViewportNode( float nLeft, float nRight, float nBottom, float nTop, 
												bool bUseProjExtents, const VistaColor& oCropColor )
: IVdfnNode()
, m_nLeft( nLeft )
, m_nRight( nRight )
, m_nBottom( nBottom )
, m_nTop( nTop )
, m_bUseProjExtents( bUseProjExtents )
, m_oCropColor( oCropColor )
{
	RegisterInPortPrototype( "left", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "right", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "top", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "bottom", new TVdfnPortTypeCompare<TVdfnPort<float> > );
}

VistaDfnCropViewportNode::~VistaDfnCropViewportNode()
{
	for( std::vector<CropViewportOverlay*>::const_iterator itOverlay = m_vecOverlays.begin();
			itOverlay != m_vecOverlays.end(); ++itOverlay )
	{
		delete (*itOverlay);
	}
}
 
bool VistaDfnCropViewportNode::PrepareEvaluationRun()
{
	m_pLeftPort = dynamic_cast<TVdfnPort<float>*>( GetInPort( "left" ) );
	m_pRightPort = dynamic_cast<TVdfnPort<float>*>( GetInPort( "right" ) );
	m_pTopPort = dynamic_cast<TVdfnPort<float>*>( GetInPort( "top" ) );
	m_pBottomPort = dynamic_cast<TVdfnPort<float>*>( GetInPort( "bottom" ) );
	
	return GetIsValid();
}

bool VistaDfnCropViewportNode::GetIsValid() const
{
	// we are valid even if no inports are set, and if no viewports are present
	// since both are valid scenarios
	return true;
}

// #############################################################################

bool VistaDfnCropViewportNode::DoEvalNode()
{
	if( m_pLeftPort )
		m_nLeft = m_pLeftPort->GetValue();
	if( m_pRightPort )
		m_nRight = m_pRightPort->GetValue();
	if( m_pBottomPort )
		m_nBottom = m_pBottomPort->GetValue();
	if( m_pTopPort )
		m_nTop = m_pTopPort->GetValue();

	for( std::vector<CropViewportOverlay*>::const_iterator itOverlay = m_vecOverlays.begin();
			itOverlay != m_vecOverlays.end(); ++itOverlay )
	{
		(*itOverlay)->SetCropProjExtents( m_nLeft, m_nRight, m_nBottom, m_nTop );
	}

	return true;
}

void VistaDfnCropViewportNode::AddViewport( VistaViewport* pViewports )
{
	m_vecOverlays.push_back( new CropViewportOverlay( pViewports, m_bUseProjExtents, m_oCropColor ) );
	m_vecOverlays.back()->SetCropProjExtents( m_nLeft, m_nRight, m_nBottom, m_nTop );
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


