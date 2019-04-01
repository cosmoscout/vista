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
#pragma warning(disable: 4231)
#pragma warning(disable: 4312)
#pragma warning(disable: 4267)
#pragma warning(disable: 4275)
#endif

#include "VistaOpenSGWebInterface.h"

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>

#include <VistaBase/VistaStreamUtils.h>

#include <OpenSG/OSGWebInterface.h>
#include <OpenSG/OSGNode.h>

#include <queue>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

struct VistaOpenSGWebInterface::DataWrapper
{
	osg::WebInterface*		m_pWebInterface;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOpenSGWebInterface::VistaOpenSGWebInterface( VistaSystem* pVistaSystem,
												const int iPort )
: m_pVistaSystem( pVistaSystem )
, m_pDataWrapper( new DataWrapper )
, m_iPort( iPort )
{
	m_pDataWrapper->m_pWebInterface = NULL;
	SetupWebInterface();
}

VistaOpenSGWebInterface::~VistaOpenSGWebInterface()
{
	delete m_pDataWrapper->m_pWebInterface;
	delete m_pDataWrapper;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaOpenSGWebInterface::SetIsEnabled( bool bSet )
{
	if( bSet == GetIsEnabled() )
		return;	

	VistaEventHandler::SetIsEnabled( bSet );

	SetupWebInterface();
}

void VistaOpenSGWebInterface::SetWebPort( const int iPort )
{
	if( m_iPort != iPort )
	{
		m_iPort = iPort;
		SetupWebInterface();
	}
}
int VistaOpenSGWebInterface::GetWebPort() const
{
	return m_iPort;
}

void VistaOpenSGWebInterface::HandleEvent( VistaEvent *pEvent )
{
	if( m_pDataWrapper->m_pWebInterface )
	{
		m_pDataWrapper->m_pWebInterface->waitRequest(0.001);
		m_pDataWrapper->m_pWebInterface->handleRequests();
	}
}

void VistaOpenSGWebInterface::SetupWebInterface()
{
	if( GetIsEnabled() == false )
	{
		delete m_pDataWrapper->m_pWebInterface;
		m_pDataWrapper->m_pWebInterface = NULL;
		m_pVistaSystem->GetEventManager()->RemEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS );
	}
	else
	{
		// only allow web interface on primary instance
		if( m_pVistaSystem->GetClusterMode()->GetIsLeader() )
		{		
			m_pDataWrapper->m_pWebInterface = new osg::WebInterface( m_iPort );

			// get realroot node
			VistaNode* pRealRoot = m_pVistaSystem->GetGraphicsManager()->GetSceneGraph()->GetRealRoot();
			osg::NodePtr pOSGNode = dynamic_cast<VistaOpenSGNodeData*>( pRealRoot->GetData() )->GetNode();
			m_pDataWrapper->m_pWebInterface->setRoot( pOSGNode );

			m_pVistaSystem->GetEventManager()->AddEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS );
		}
	}
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

