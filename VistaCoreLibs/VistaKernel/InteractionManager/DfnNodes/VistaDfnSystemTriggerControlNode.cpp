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


#include "VistaDfnSystemTriggerControlNode.h"

#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnSystemTriggerControlNode::VistaDfnSystemTriggerControlNode( VistaKeyboardSystemControl *pCtrl )
	: IVdfnNode(), m_pCtrl(pCtrl),
	m_pTriggers(NULL),
	m_pModders(NULL),
	m_pTrigger(NULL),
	m_pModder(NULL),
	m_pNonSwallowTrigger(new TVdfnPort<int>),
	m_pNonSwallowMod( new TVdfnPort<int> ),
	m_pNonSwallowTriggers( new TVdfnPort< std::vector<int> >),
	m_pNonSwallowMods( new TVdfnPort< std::vector<int> >)
{
	// history processing: get *list* of ints for processing
	RegisterInPortPrototype("triggerlist", new TVdfnPortTypeCompare< TVdfnPort<std::vector<int> > > );
	RegisterInPortPrototype("modlist", new TVdfnPortTypeCompare< TVdfnPort<std::vector<int> > > );

	// shortcut: do a sample latest!
	// use only one int for processing
	RegisterInPortPrototype("trigger", new TVdfnPortTypeCompare<TVdfnPort<int> >);
	RegisterInPortPrototype("modder", new TVdfnPortTypeCompare<TVdfnPort<int> >);

	// mirror trigger to output, for further processing
	RegisterOutPort( "trigger", m_pNonSwallowTrigger );
	RegisterOutPort( "modder", m_pNonSwallowMod );

	RegisterOutPort( "triggerlist", m_pNonSwallowTriggers );
	RegisterOutPort( "modlist", m_pNonSwallowMods );
}

VistaDfnSystemTriggerControlNode::~VistaDfnSystemTriggerControlNode()
{
}

bool VistaDfnSystemTriggerControlNode::GetIsValid() const
{
	return ( m_pCtrl &&
		( ( m_pTriggers != NULL && m_pTrigger == NULL )
		|| ( m_pTriggers == NULL && m_pTrigger != NULL ) ) );
}

bool VistaDfnSystemTriggerControlNode::PrepareEvaluationRun()
{
	m_pTriggers = dynamic_cast<TVdfnPort<std::vector<int> > *>(GetInPort("triggerlist"));
	m_pModders  = dynamic_cast<TVdfnPort<std::vector<int> > *>(GetInPort("modlist"));
	m_pTrigger  = dynamic_cast<TVdfnPort<int>*>(GetInPort("trigger"));
	m_pModder  = dynamic_cast<TVdfnPort<int>*>(GetInPort("modder"));

	return GetIsValid();
}

bool VistaDfnSystemTriggerControlNode::DoEvalNode()
{
	if(m_pTriggers) // sanity check, should not happen
	{
		std::vector<int> vUnswallowedTrigger,
		                 vUnswallowedMods;
		if(m_pModders)
		{
			m_pCtrl->InjectKeys(m_pTriggers->GetValueConstRef(),
					            m_pModders->GetValueConstRef(),
					            vUnswallowedTrigger,
					            vUnswallowedMods);
		}
		else
		{
			std::vector<int> vTmp(m_pTriggers->GetValueConstRef().size(), 0);
			m_pCtrl->InjectKeys(m_pTriggers->GetValueConstRef(),
					            vTmp,
					            vUnswallowedTrigger,
					            vUnswallowedMods);
		}

		// ok, history processing, there is no real useful return value for checking on
		// forwaring the incoming, InjectKeys always returns true
		// so in that case, the incoming will be reflected on triggers and modders

		// for comfort: mirror last key to non-swallow trigger and mod
		if(!vUnswallowedTrigger.empty())
		{
				m_pNonSwallowTriggers->SetValue( vUnswallowedTrigger, GetUpdateTimeStamp() );
				m_pNonSwallowTrigger->SetValue( vUnswallowedTrigger[0], GetUpdateTimeStamp() );
		}

		if(m_pModders && !vUnswallowedMods.empty())
		{
			m_pNonSwallowMods->SetValue( vUnswallowedMods, GetUpdateTimeStamp() );
			m_pNonSwallowMod->SetValue( vUnswallowedMods[0], GetUpdateTimeStamp());
		}

	}
	else if(m_pTrigger) // same here
	{
		if(m_pModder)
		{
			if(m_pCtrl->InjectKey( m_pTrigger->GetValue(), m_pModder->GetValue() ) == false)
			{
				// not swallowed!
				m_pNonSwallowMod->SetValue( m_pModder->GetValue(), GetUpdateTimeStamp() );
				m_pNonSwallowTrigger->SetValue( m_pTrigger->GetValue(), GetUpdateTimeStamp() );
			}
		}
		else
		{
			if(m_pCtrl->InjectKey( m_pTrigger->GetValue(), 0 ) == false)
			{
				// not swallowed!
				m_pNonSwallowTrigger->SetValue( m_pTrigger->GetValue(), GetUpdateTimeStamp() );
			}
		}
	}
	return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


