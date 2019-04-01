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


#include "VistaUserPlatform.h" 
#include "VistaVirtualPlatformAdapter.h"
#include <VistaAspects/VistaObserver.h>

#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
class VistaUserPlatform::UserPlatformObserver : public IVistaObserver
{
	public:
	UserPlatformObserver(VistaUserPlatform *pPlatform)
	: IVistaObserver(),
	  m_pPlatform(NULL),
	  m_pUserPlatform(pPlatform)
	{
	}
	
	~UserPlatformObserver()
	{
		ReleaseObserveable(m_pPlatform);
	}
	
	virtual bool ObserveableDeleteRequest(IVistaObserveable *pObserveable, 
										  int nTicket = IVistaObserveable::TICKET_NONE)
	{
		return true;
	}
	
	virtual void ObserveableDelete(IVistaObserveable *pObserveable, 
								   int nTicket = IVistaObserveable::TICKET_NONE)
	{
		ReleaseObserveable(pObserveable, nTicket);
	}
   
	virtual void ReleaseObserveable(IVistaObserveable *pObserveable, 
									int nTicket = IVistaObserveable::TICKET_NONE)
	{
		VistaVirtualPlatform *pPlat = dynamic_cast<VistaVirtualPlatform*>(pObserveable);
		if(pPlat)
		{
			if( m_pPlatform )
			{
				m_pPlatform->DetachObserver(this);
			}
			m_pPlatform = NULL;
		}    	
	}
	
	virtual void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket)
	{
		//VistaVirtualPlatform *pPlat = dynamic_cast<VistaVirtualPlatform*>(pObserveable);
		UpdateNodes();				
	}
	
	virtual bool Observes(IVistaObserveable *pObserveable)
	{
		return (dynamic_cast<VistaVirtualPlatform*>(pObserveable) == m_pPlatform);
	}
	
	virtual void Observe(IVistaObserveable *pObservable, int eTicket=IVistaObserveable::TICKET_NONE) 
	{
			ReleaseObserveable(m_pPlatform);
		VistaVirtualPlatform *pPlat = dynamic_cast<VistaVirtualPlatform*>(pObservable);    		
			m_pPlatform = pPlat;
			if(m_pPlatform)
				m_pPlatform->AttachObserver(this);
	}

	void UpdateNodes()
	{
		if(m_pPlatform)
		{
			VistaTransformNode *pNode = m_pUserPlatform->GetPlatformNode();
			VistaTransformMatrix m;
			
			m_pPlatform->GetMatrix(m);
			pNode->SetTransform(m); // simply apply to node
		}
	}
	
	private:
	VistaVirtualPlatform *m_pPlatform;
	VistaUserPlatform    *m_pUserPlatform;

};


// #############################################################################

class VistaUserPlatform::UserObserver : public IVistaObserver
{
public:
	UserObserver(VistaUserPlatform *pPlatform)
	: IVistaObserver(),
	  m_pProps(NULL),
	  m_pUserPlatform(pPlatform)
	  
	{
	}
	
	~UserObserver()
	{
		if(m_pProps)
			ReleaseObserveable(m_pProps);
	}
	
	virtual bool ObserveableDeleteRequest(IVistaObserveable *pObserveable, 
										  int nTicket = IVistaObserveable::TICKET_NONE)
	{
		return true;
	}
	
	virtual void ObserveableDelete(IVistaObserveable *pObserveable, 
								   int nTicket = IVistaObserveable::TICKET_NONE)
	{
		ReleaseObserveable(pObserveable, nTicket);
	}
   
	virtual void ReleaseObserveable(IVistaObserveable *pObserveable, 
									int nTicket = IVistaObserveable::TICKET_NONE)
	{
		VistaDisplaySystem::VistaDisplaySystemProperties *pProps 
			= dynamic_cast<VistaDisplaySystem::VistaDisplaySystemProperties *>(pObserveable);
		if(pProps)
		{
			if( m_pProps )
			{
				m_pProps->DetachObserver(this);
			}
			m_pProps = NULL;
		}    	
	}
	
	virtual void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket)
	{
		if( msg == VistaDisplaySystem::VistaDisplaySystemProperties::MSG_VIEWER_POSITION_CHANGE
			|| msg == VistaDisplaySystem::VistaDisplaySystemProperties::MSG_VIEWER_ORIENTATION_CHANGE )
		{
			UpdateNodes();
		}
	}
	
	virtual bool Observes(IVistaObserveable *pObserveable)
	{
		return (dynamic_cast<VistaDisplaySystem::VistaDisplaySystemProperties *>(pObserveable) == m_pProps);
	}
	
	virtual void Observe(IVistaObserveable *pObservable, int eTicket=IVistaObserveable::TICKET_NONE) 
	{
		ReleaseObserveable(m_pProps);
		VistaDisplaySystem::VistaDisplaySystemProperties *pProps 
			= dynamic_cast<VistaDisplaySystem::VistaDisplaySystemProperties *>(pObservable);    		
		m_pProps = pProps;
		if(m_pProps)
			m_pProps->AttachObserver(this);
	}

	void UpdateNodes()
	{
		if(m_pProps)
		{
			VistaVirtualPlatform *ref = m_pUserPlatform->GetPlatform();
			VistaVector3D wcPos;
			VistaQuaternion qWcOri;

			m_pProps->GetViewerPosition(wcPos[0], wcPos[1], wcPos[2]);
			m_pProps->GetViewerOrientation( qWcOri[0], qWcOri[1], qWcOri[2], qWcOri[3] );

			if(!m_pProps->GetLocalViewer())
			{
				ref->TransformToFrame(wcPos, qWcOri);
			}

			VistaTransformNode *pUser = m_pUserPlatform->GetPlatformUserNode();
			pUser->SetTranslation(wcPos);
			pUser->SetRotation(qWcOri);
		}	
	}
	
	private:
	VistaDisplaySystem::VistaDisplaySystemProperties *m_pProps;
	VistaUserPlatform    *m_pUserPlatform;
};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
//VistaUserPlatform::VistaUserPlatform(VistaSceneGraph *pSG)
//: m_pPlatTrans(new VistaVirtualPlatformAdapter),
//  m_pPlatformNode(NULL),
//  m_pUserNode(NULL),
//  m_pSG(pSG),
//  m_pPlatObs(NULL)
//{
//	m_pPlatformNode = m_pSG->NewTransformNode(m_pSG->GetRoot());
//	m_pPlatformNode->SetName("Virtualplatform-Node");
//
//	m_pPlatObs = new CUserPlatformObserver(this);
//	m_pUserNode = m_pSG->NewTransformNode(m_pPlatformNode);
//	m_pUserNode->SetName("Platform-User-Node");
//}

VistaUserPlatform::VistaUserPlatform(VistaSceneGraph *pSG,
									  VistaDisplaySystem *pDispSys)
: m_pPlatTrans(new VistaVirtualPlatformAdapter),
  m_pPlatformNode(NULL),
  m_pUserNode(NULL),
  m_pSG(pSG),
  m_pPlatObs(NULL),
  m_pDispSys(pDispSys)
{
	m_pPlatTrans->SetVirtualPlatform(m_pDispSys->GetReferenceFrame());
	m_pPlatformNode = m_pSG->NewTransformNode(m_pSG->GetRoot());
	m_pPlatformNode->SetName( "Virtualplatform-Node" );
	
	m_pPlatObs = new UserPlatformObserver(this);
	m_pPlatObs->Observe(m_pDispSys->GetReferenceFrame());

	m_pUserNode = m_pSG->NewTransformNode(m_pPlatformNode);
	m_pUserNode->SetName("Platform-User-Node");

	m_pUserObs = new UserObserver(this);
	m_pUserObs->Observe( m_pDispSys->GetProperties() );

	// obtain once to set initial transform
	m_pPlatObs->UpdateNodes();
	m_pUserObs->UpdateNodes();
}


VistaUserPlatform::~VistaUserPlatform()
{
	delete m_pUserObs;
	delete m_pPlatObs;
	delete m_pPlatTrans;

	if(m_pUserNode)
	{
		while( m_pUserNode->GetNumChildren() > 0 )
		{
			// disconnect to prevent deletion as these might be user nodes
			m_pUserNode->DisconnectChild( (unsigned int)0 );
		}
		delete m_pUserNode;
	}

	if(m_pPlatformNode)
	{
		while( m_pPlatformNode->GetNumChildren() > 0 )
		{
			// disconnect to prevent deletion as these might be user nodes
			m_pPlatformNode->DisconnectChild( (unsigned int)0 ); 
		}
		delete m_pPlatformNode;
	}
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaVirtualPlatformAdapter *VistaUserPlatform::GetPlatformTrans() const
{
	return m_pPlatTrans;
}

VistaTransformNode *VistaUserPlatform::GetPlatformNode() const
{
	return m_pPlatformNode;
}

VistaTransformNode *VistaUserPlatform::GetPlatformUserNode() const
{
	return m_pUserNode;
}

VistaVirtualPlatform *VistaUserPlatform::GetPlatform() const
{
	return m_pPlatTrans->GetVirtualPlatform();
}

VistaVector3D VistaUserPlatform::GetUserViewPosition() const
{
	if(m_pDispSys->GetDisplaySystemProperties()->GetLocalViewer())
	{
		VistaVector3D v3LcPos = m_pDispSys->GetDisplaySystemProperties()->GetViewerPosition();
		return m_pDispSys->GetReferenceFrame()->TransformPositionFromFrame(v3LcPos);
	}
	else
		return m_pDispSys->GetDisplaySystemProperties()->GetViewerPosition();
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


