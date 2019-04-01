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


#include "VistaDfnSimpleTextNode.h"

#include <VistaKernel/DisplayManager/VistaSimpleTextOverlay.h>
#include <VistaKernel/DisplayManager/VistaTextEntity.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <VistaBase/VistaTimerImp.h>
#include <VistaTools/VistaRandomNumberGenerator.h>

#include <algorithm>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

struct VistaDfnSimpleTextNode::PortTextData
{
	std::string m_sStringPrefix;
	IVdfnPort* m_pPort;
	VdfnPortFactory::IPortStringGet* m_pStringGet;
	IVistaTextEntity* m_pText;
	IVistaTextEntity* m_pCaption;
	unsigned int m_nLastUpdateCount;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnSimpleTextNode::VistaDfnSimpleTextNode( VistaDisplayManager* pDisplayManager )
: VdfnShallowNode()
, m_pOverlay( new VistaSimpleTextOverlay( pDisplayManager ) )
, m_pDisplayManager( pDisplayManager )
, m_pEnablePort( NULL )
, m_nTextSize( 12 )
{
}

VistaDfnSimpleTextNode::~VistaDfnSimpleTextNode()
{
	
	delete m_pOverlay;
	for( std::vector<PortTextData*>::iterator itText = m_vecPortTexts.begin();
			itText != m_vecPortTexts.end(); ++itText )
	{		
		delete (*itText)->m_pText;
		delete (*itText)->m_pCaption;
		delete (*itText);
	}
}

bool VistaDfnSimpleTextNode::PrepareEvaluationRun()
{
	return GetIsValid();
}

bool VistaDfnSimpleTextNode::DoEvalNode()
{
	if( m_pEnablePort && m_pEnablePort->GetValue() == false )
	{
		m_pOverlay->SetIsEnabled( false );
		return true;
	}
	m_pOverlay->SetIsEnabled( true );
	for( std::vector<PortTextData*>::iterator itText = m_vecPortTexts.begin();
			itText != m_vecPortTexts.end(); ++itText )
	{
		if( (*itText)->m_pStringGet == NULL )
			continue;
		if( (*itText)->m_pPort->GetUpdateCounter() < (*itText)->m_nLastUpdateCount )
			continue;
		(*itText)->m_nLastUpdateCount = (*itText)->m_pPort->GetUpdateCounter();

		if( m_nCaptionColumnWidth > 0 )
		{
			(*itText)->m_pText->SetText( (*itText)->m_pStringGet->GetValueAsString( (*itText)->m_pPort ) );
		}
		else
		{
			(*itText)->m_pText->SetText( (*itText)->m_sStringPrefix 
					+ (*itText)->m_pStringGet->GetValueAsString( (*itText)->m_pPort ) );
		}
	}
	return true;
}

void VistaDfnSimpleTextNode::OnActivation( double dTs )
{
    VdfnShallowNode::OnActivation( dTs );
	if( m_pEnablePort )
		m_pOverlay->SetIsEnabled( m_pEnablePort->GetValue() );
	else
		m_pOverlay->SetIsEnabled( true );
}

void VistaDfnSimpleTextNode::OnDeactivation( double dTs )
{
    IVdfnNode::OnDeactivation(dTs);
	if( m_pEnablePort )
		m_pOverlay->SetIsEnabled( m_pEnablePort->GetValue() );
	else
		m_pOverlay->SetIsEnabled( false );
}

bool VistaDfnSimpleTextNode::SetInPort( const std::string &sName, IVdfnPort *pPort )
{
	if( sName == "TextEnabled" )
	{
		m_pEnablePort = dynamic_cast< TVdfnPort<bool>* >( pPort );
		if( m_pEnablePort )
			return VdfnShallowNode::SetInPort( sName, pPort );
	}
	PortTextData* pData = new PortTextData;
	pData->m_sStringPrefix = sName + " : ";
	pData->m_pPort = pPort;
	VdfnPortFactory::CPortAccess* pAccess = VdfnPortFactory::GetSingleton()->GetPortAccess(
											pPort->GetTypeDescriptor() );
	if( pAccess && pAccess->m_pStringGet != NULL )
	{
		pData->m_pStringGet = pAccess->m_pStringGet;
	}
	else
	{
		pData->m_pStringGet = NULL;
	}

	if( m_nCaptionColumnWidth <= 0 )
	{
		pData->m_pCaption = NULL;
		pData->m_pText = m_pDisplayManager->CreateTextEntity();
		pData->m_pText->SetColor( m_oColor );
		pData->m_pText->SetYPos( m_vecPortTexts.size() + 1 );
		if( pData->m_pStringGet )
			pData->m_pText->SetText( pData->m_sStringPrefix );
		else
			pData->m_pText->SetText( pData->m_sStringPrefix + "<no string getter>" );
		pData->m_pText->SetFont( "", m_nTextSize );
		pData->m_nLastUpdateCount = 0;
	}
	else
	{
		pData->m_pCaption = m_pDisplayManager->CreateTextEntity();
		pData->m_pCaption->SetColor( m_oColor );
		pData->m_pCaption->SetYPos( m_vecPortTexts.size() + 1 );
		pData->m_pCaption->SetText( pData->m_sStringPrefix );
		pData->m_pCaption->SetFont( "", m_nTextSize );
		m_pOverlay->AddText( pData->m_pCaption );
		pData->m_pText = m_pDisplayManager->CreateTextEntity();
		pData->m_pText->SetColor( m_oColor );
		pData->m_pText->SetYPos( m_vecPortTexts.size() + 1 );
		pData->m_pText->SetXPos( m_nCaptionColumnWidth );
		if( pData->m_pStringGet )
			pData->m_pText->SetText( " " );
		else
			pData->m_pText->SetText( "<no string getter>" );
		pData->m_pText->SetFont( "", m_nTextSize );
		pData->m_nLastUpdateCount = 0;
	}

	m_vecPortTexts.push_back( pData );
	m_pOverlay->AddText( pData->m_pText );

	return VdfnShallowNode::SetInPort( sName, pPort );
}

int VistaDfnSimpleTextNode::GetTextSize() const
{
	return m_nTextSize;
}

void VistaDfnSimpleTextNode::SetTextSize( const int& oValue )
{
	m_nTextSize = oValue;
}

VistaColor VistaDfnSimpleTextNode::GetColor() const
{
	return m_oColor;
}

void VistaDfnSimpleTextNode::SetColor( const VistaColor& oValue )
{
	m_oColor = oValue;
}

int VistaDfnSimpleTextNode::GetCaptionColumnWidth() const
{
	return m_nCaptionColumnWidth;
}

void VistaDfnSimpleTextNode::SetCaptionColumnWidth( const int& oValue )
{
	m_nCaptionColumnWidth = oValue;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


