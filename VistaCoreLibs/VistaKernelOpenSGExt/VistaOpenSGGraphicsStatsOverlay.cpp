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




#include "VistaOpenSGGraphicsStatsOverlay.h"

#include "VistaKernel/DisplayManager/VistaTextEntity.h"
#include "VistaKernel/DisplayManager/VistaDisplayManager.h"
#include "VistaKernel/DisplayManager/VistaSimpleTextOverlay.h"

#if defined(WIN32)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4275)
#pragma warning(disable: 4267)
#pragma warning(disable: 4251)
#pragma warning(disable: 4231)
#endif
#include "OpenSG/OSGSimpleStatisticsForeground.h"
#include "OpenSG/OSGDrawable.h"
#include "OpenSG/OSGRenderAction.h"

#include <sstream>
#include "VistaKernel/OpenSG/VistaOpenSGDisplayBridge.h"
#include "VistaKernel/EventManager/VistaSystemEvent.h"
#include "VistaKernel/EventManager/VistaEventManager.h"
#include "VistaBase/VistaTimeUtils.h"

struct VistaOpenSGGraphicsStatsOverlay::Line
{
public:
	static const int S_nEntrySize = 12;
	static const int S_nLabelSize = 18;

	Line( osg::StatElem* pStatElem, std::string sPrefix, std::string sPostfix,
			VistaDisplayManager* pMgr, int nPrecision = 2, float nFactor = 1.0f )
	: m_sPrefix( sPrefix )
	, m_sPostfix( sPostfix )
	, m_pText( pMgr->CreateTextEntity() )
	, m_nFactor( nFactor )
	, m_nPrecision( nPrecision )
	, m_pStatElem( pStatElem )
	{
		if( (int)m_sPrefix.length() < S_nLabelSize )
		{
			m_sPrefix = std::string( S_nLabelSize - m_sPrefix.length(), ' ' ) + m_sPrefix;
		}
		m_sPrefix += " :";

		m_sPostfix = " " + m_sPostfix;

		m_pText->SetFont( "MONOSPACE", 15 );
		m_pText->SetColor( VistaColor::YELLOW );
	}

	~Line()
	{
		delete m_pText;
	}

	IVistaTextEntity* GetText() const { return m_pText; }

	void UpdateText()
	{
		std::stringstream oText;
		oText.precision( m_nPrecision );		
		oText.setf( std::ios_base::fixed );
		oText << m_sPrefix << std::setw( S_nEntrySize )
				<< ( (double)m_nFactor * m_pStatElem->getValue() ) << m_sPostfix;
		m_pText->SetText( oText.str() );
	}

private:
	std::string m_sPrefix;
	std::string m_sPostfix;
	IVistaTextEntity* m_pText;
	float m_nFactor;
	int m_nPrecision;
	osg::StatElem* m_pStatElem;

};

VistaOpenSGGraphicsStatsOverlay::VistaOpenSGGraphicsStatsOverlay( VistaDisplayManager* pManager, VistaEventManager* pEventManager, const bool bRegisterAllViewports )
: m_pDisplayManager( pManager )
, m_pEventManager( pEventManager )
, m_bEnabled( false )
, m_nLastUpdate( 0 )
, m_pStatistics( new osg::StatCollector )
{
	if( bRegisterAllViewports )
	{
		const std::map< std::string, VistaViewport* >& mapViewports = pManager->GetViewportsConstRef();
		for( std::map< std::string, VistaViewport* >::const_iterator itViewport = mapViewports.begin(); itViewport != mapViewports.end(); ++itViewport )
		{
			AddViewport( (*itViewport).second );
		}
	}
}


VistaOpenSGGraphicsStatsOverlay::~VistaOpenSGGraphicsStatsOverlay()
{	
	for( std::vector< VistaSimpleTextOverlay* >::iterator itOverlay = m_vecTextOverlays.begin(); itOverlay != m_vecTextOverlays.end(); ++itOverlay )
	{
		delete (*itOverlay);
	}
	for( std::vector<Line*>::iterator itLine = m_vecLines.begin();
		itLine != m_vecLines.end(); ++itLine )
	{
		delete (*itLine);
	}
	delete m_pStatistics;
}

void VistaOpenSGGraphicsStatsOverlay::SetIsEnabled( bool bSet )
{
	VistaEventHandler::SetIsEnabled( bSet );

	if( bSet == m_bEnabled )
		return;

	if( m_vecLines.empty() )
	{

		VistaOpenSGDisplayBridge* pOSGBridge = dynamic_cast<VistaOpenSGDisplayBridge*>( m_pDisplayManager->GetDisplayBridge() );
		pOSGBridge->GetRenderAction()->setStatistics( m_pStatistics );

		int nLine = 0;

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::RenderAction::statDrawTime ),
			"DrawTime", "ms",
			m_pDisplayManager,
			1, 1000.0f ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::DrawActionBase::statTravTime ),
			"TraversalTime", "ms",
			m_pDisplayManager,
			1, 1000.0f ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		++nLine;

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::DrawActionBase::statCulledNodes ),
			"Culled Nodes", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::DrawActionBase::statCullTestedNodes ),
			"Culling Tests", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::RenderAction::statNOcclusionTests ),
			"Occlusion Tests", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::RenderAction::statNOcclusionCulled ),
			"Occlusions Culled", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		++nLine;

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::RenderAction::statNGeometries ),
			"Num Geometries", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::RenderAction::statNTransGeometries ),
			"Transformed Geoms", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::Drawable::statNTriangles ),
			"Num Triangles", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::Drawable::statNVertices ),
			"Num Vertices", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::Drawable::statNVertices ),
			"Num Primitives", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		++nLine;

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::RenderAction::statNLights ),
			"Num Lights", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::RenderAction::statNMaterials ),
			"Num Materials", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::RenderAction::statNTextures ),
			"Num Textures", "",
			m_pDisplayManager,
			0 ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		++nLine;

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::Drawable::statNGeoBytes ),
			"Mem Geometries", "Mb",
			m_pDisplayManager,
			3, 1.0f / 1024.0f / 1024.0f ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		m_vecLines.push_back( new Line( m_pStatistics->getElem( osg::RenderAction::statNTexBytes ),
			"Mem Textures", "Mb",
			m_pDisplayManager,
			3, 1.0f / 1024.0f / 1024.0f ) );
		m_vecLines.back()->GetText()->SetYPos( ++nLine );

		for( std::vector< VistaSimpleTextOverlay* >::iterator itOverlay = m_vecTextOverlays.begin(); itOverlay != m_vecTextOverlays.end(); ++itOverlay )
		{
			for( std::vector< Line* >::iterator itLine = m_vecLines.begin(); itLine != m_vecLines.end(); ++itLine )
				(*itOverlay)->AddText( (*itLine)->GetText() );
		}
	}

	for( std::vector< VistaSimpleTextOverlay* >::iterator itOverlay = m_vecTextOverlays.begin(); itOverlay != m_vecTextOverlays.end(); ++itOverlay )
	{
		(*itOverlay)->SetIsEnabled( bSet );
	}
	if( bSet )
	{
		m_pEventManager->AddEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_POSTGRAPHICS );
	}
	else
	{
		m_pEventManager->RemEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_POSTGRAPHICS );
	}

	m_nLastUpdate = 0;
	m_bEnabled = bSet;
}

void VistaOpenSGGraphicsStatsOverlay::HandleEvent( VistaEvent *pEvent )
{
	if( VistaTimeUtils::GetStandardTimer().GetSystemTime() - m_nLastUpdate > m_nUpdateCycle )
	{
		for( std::vector<Line*>::iterator itLine = m_vecLines.begin();
			itLine != m_vecLines.end(); ++itLine )
		{
			(*itLine)->UpdateText();
		}
		m_nLastUpdate = VistaTimeUtils::GetStandardTimer().GetSystemTime();
	}
}

VistaColor VistaOpenSGGraphicsStatsOverlay::GetTextColor() const
{
	return m_oColor;
}

void VistaOpenSGGraphicsStatsOverlay::SetTextColor( const VistaColor& colText )
{
	for( std::vector<Line*>::iterator itLine = m_vecLines.begin();
		itLine != m_vecLines.end(); ++itLine )
	{
		(*itLine)->GetText()->SetColor( colText );
	}
	m_oColor = colText;
}

int VistaOpenSGGraphicsStatsOverlay::GetTextSize() const
{
	return m_nTextSize;
}

void VistaOpenSGGraphicsStatsOverlay::SetTextSize( const int nTextSize )
{
	m_nTextSize = nTextSize;
	for( std::vector<Line*>::iterator itLine = m_vecLines.begin();
		itLine != m_vecLines.end(); ++itLine )
	{
		(*itLine)->GetText()->SetFont( "MONOSPACE", nTextSize );
	}
}

VistaType::microtime VistaOpenSGGraphicsStatsOverlay::GetUpdateInterval() const
{
	return m_nUpdateCycle;
}

void VistaOpenSGGraphicsStatsOverlay::SetUpdateIntercal( const VistaType::microtime nInterval )
{
	m_nUpdateCycle = nInterval;
}

void VistaOpenSGGraphicsStatsOverlay::AddViewport( VistaViewport* pViewport )
{
	VistaSimpleTextOverlay* pOverlay = new VistaSimpleTextOverlay( pViewport );
	for( std::vector< Line* >::iterator itLine = m_vecLines.begin(); itLine != m_vecLines.end(); ++itLine )
	{
		pOverlay->AddText( (*itLine)->GetText() );
	}
	m_vecTextOverlays.push_back( pOverlay );
}
