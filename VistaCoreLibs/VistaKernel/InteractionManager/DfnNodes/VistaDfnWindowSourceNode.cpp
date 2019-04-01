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


#include "VistaDfnWindowSourceNode.h"
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaAspects/VistaObserver.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaDfnWindowSourceNode::WindowObserver : public IVistaObserver
{
public:
	WindowObserver( VistaWindow* pWindow )
	: IVistaObserver()
	, m_pWindow( pWindow )
	, m_bRatioChange( true )
	{
		m_pWindow->GetWindowProperties()->AttachObserver( this );
	}

	~WindowObserver()
	{
		m_pWindow->GetWindowProperties()->DetachObserver( this );
	}

	virtual bool ObserveableDeleteRequest( IVistaObserveable* pObserveable,
										int nTicket = IVistaObserveable::TICKET_NONE )
	{
		return true;
	}

	virtual void ObserveableDelete( IVistaObserveable* pObserveable,
										int nTicket = IVistaObserveable::TICKET_NONE )
	{
		if( pObserveable == m_pWindow->GetWindowProperties() )
			m_pWindow = NULL;
	}

	virtual void ReleaseObserveable( IVistaObserveable* pObserveable,
										int nTicket = IVistaObserveable::TICKET_NONE )
	{
		if( m_pWindow->GetWindowProperties() == pObserveable )
			m_pWindow = NULL;
	}

	virtual void ObserverUpdate( IVistaObserveable* pObserveable,
										int nMsg, int nTicket )
	{
		if( nMsg == VistaWindow::VistaWindowProperties::MSG_SIZE_CHANGE )
		{
			m_bRatioChange = true;
		}
	}

	virtual bool Observes( IVistaObserveable* pObserveable )
	{
		return ( pObserveable == m_pWindow->GetWindowProperties() );
	}

	virtual void Observe( IVistaObserveable* pObserveable, 
										int nTicket = IVistaObserveable::TICKET_NONE )
	{
		// won't happen
	}

	bool GetRatioChange()
	{
		bool bRet = m_bRatioChange;
		m_bRatioChange = false;
		return bRet;
	}

private:
	bool			m_bRatioChange;
	VistaWindow*	m_pWindow;
};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnWindowSourceNode::VistaDfnWindowSourceNode( VistaWindow *pWindow )
										  : IVdfnNode(),
											m_pX(new TVdfnPort<int>),
											m_pY(new TVdfnPort<int>),
											m_pW(new TVdfnPort<int>),
											m_pH(new TVdfnPort<int>),
											m_pWindow(pWindow),
											m_pObs(new WindowObserver( pWindow )),
											m_nUpdateCount(0)
{
	RegisterOutPort( "win_x", m_pX );
	RegisterOutPort( "win_y", m_pY );
	RegisterOutPort( "win_w", m_pW );
	RegisterOutPort( "win_h", m_pH );
}


VistaDfnWindowSourceNode::~VistaDfnWindowSourceNode()
{
	delete m_pObs;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaDfnWindowSourceNode::GetIsValid() const
{
	return (m_pWindow != NULL);
}


bool VistaDfnWindowSourceNode::DoEvalNode()
{
	int nX, nY, nW, nH;
	m_pWindow->GetWindowProperties()->GetSize(nW, nH);
	m_pWindow->GetWindowProperties()->GetPosition(nX, nY);

	m_pX->SetValue( nX, GetUpdateTimeStamp() );
	m_pY->SetValue( nY, GetUpdateTimeStamp() );
	m_pW->SetValue( nW, GetUpdateTimeStamp() );
	m_pH->SetValue( nH, GetUpdateTimeStamp() );

	return true;
}

unsigned int VistaDfnWindowSourceNode::CalcUpdateNeededScore() const
{
	if(m_pObs->GetRatioChange())
	{
		return ++m_nUpdateCount;
	}
	return m_nUpdateCount;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

