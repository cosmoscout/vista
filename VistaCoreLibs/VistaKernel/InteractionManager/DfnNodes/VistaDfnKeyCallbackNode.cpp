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


#include "VistaDfnKeyCallbackNode.h" 

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaDfnKeyCallbackNode::VistaDfnKeyCallbackNode()
: IVdfnNode()
, m_pCallbackCountPort( new TVdfnPort<int> )
, m_pKeyboard( NULL )
, m_nCounter(0)
{
	m_pCallbackKeyDown = new CounterCallback(this);
	RegisterOutPort( "value", m_pCallbackCountPort );
}

VistaDfnKeyCallbackNode::~VistaDfnKeyCallbackNode()
{
	if( m_pCallbackKeyDown )
		m_pKeyboard->UnbindAction( m_pCallbackKeyDown );
	delete m_pCallbackKeyDown;
}
 
bool VistaDfnKeyCallbackNode::PrepareEvaluationRun()
{
	return GetIsValid();
}

bool VistaDfnKeyCallbackNode::GetIsValid() const
{
	return ( m_pCallbackKeyDown != NULL );
}

bool VistaDfnKeyCallbackNode::DoEvalNode()
{
	if (m_pCallbackCountPort->GetValue () != (int)m_nCounter)
		m_pCallbackCountPort->SetValue( (int)m_nCounter, GetUpdateTimeStamp() );
	return true;
}

unsigned int VistaDfnKeyCallbackNode::CalcUpdateNeededScore() const
{
	// we return counter value plus 1, because else with 0 no-count gets lost
	return m_nCounter + 1;
}

void VistaDfnKeyCallbackNode::SetupKeyboardCallback(  
	  VistaKeyboardSystemControl* pKeyboard
	, const int nKeyCode
	, const int nModCode
	, const std::string& sDescription
	, const bool bForce )
{
	if (m_pKeyboard == NULL)
	{
		m_pKeyboard = pKeyboard;
		if( m_pKeyboard->BindAction( nKeyCode, nModCode, m_pCallbackKeyDown, sDescription, false, true, bForce ) == false )
		{
			delete m_pCallbackKeyDown;
			m_pCallbackKeyDown = NULL;
		}
	}
}

void VistaDfnKeyCallbackNode::Callback( const CounterCallback* pCallback )
{
	if (pCallback == m_pCallbackKeyDown)
		++m_nCounter;
}

VistaDfnKeyCallbackNode::CounterCallback::CounterCallback(VistaDfnKeyCallbackNode* pOwner)
	: m_pOwner(pOwner)
{
}

bool VistaDfnKeyCallbackNode::CounterCallback::Do()
{
	m_pOwner->Callback (this);
	return true;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


