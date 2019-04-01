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


#include "VdfnSamplerNode.h"

#include <VistaDataFlowNet/VdfnUtil.h>
#include <VistaAspects/VistaPropertyAwareable.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnSamplerNode::VdfnSamplerNode(const std::list<std::string> &strTriggerList)
: VdfnShallowNode(),
  m_liTriggerList(strTriggerList),
  m_eMd(MD_TRIGGER_OR),
  m_nUpdateScore(0)
{
}

VdfnSamplerNode::~VdfnSamplerNode()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VdfnSamplerNode::SetInPort(const std::string &sName, IVdfnPort *pPort )
{
    if(VdfnShallowNode::SetInPort( sName, pPort ))
    {
        // register out port
        IVdfnPort *pClonedPort = pPort->Clone();
        if(pClonedPort)
        {
            RegisterOutPort( sName, pClonedPort );
        }
        else
        {
            // should we unset the inport?
            return false;
        }
        return true;
    }
    return false;
}

bool VdfnSamplerNode::DoEvalNode()
{
    // copy in-ports to outports
    for( IVdfnNode::PortMap::const_iterator cit = this->m_mpInPorts.begin();
        cit != m_mpInPorts.end(); ++cit )
    {
        IVdfnPort *pIn = (*cit).second;
        IVdfnPort *pOut = GetOutPort( (*cit).first );
        if(pIn && pOut)
        {
            pOut->AssignFrom( pIn );
        }
    }
	return true;
}

bool VdfnSamplerNode::PrepareEvaluationRun()
{
	if(m_liTriggerList.empty())
	{
		// use all assigned ports as triggers.
		m_liTriggerList = GetInPortNames();
	}

	// prepare trigger map
	for(std::list<std::string>::const_iterator cit = m_liTriggerList.begin();
		cit != m_liTriggerList.end(); ++cit)
	{
		IVdfnPort *pPort = GetInPort( *cit );
		if(pPort)
		{
			// store initial revision for this port in the portmap
			m_mpRevisions[ pPort ] = pPort->GetUpdateCounter();
		}
	}

	return VdfnShallowNode::PrepareEvaluationRun();
}


unsigned int VdfnSamplerNode::CalcUpdateNeededScore() const
{

	if(m_mpRevisions.empty())
		return VdfnShallowNode::CalcUpdateNeededScore();
	else
	{
		if(m_eMd == MD_TRIGGER_OR)
		{
			// check revisions of trigger ports
			for(REVMAP::const_iterator cit = m_mpRevisions.begin();
				cit != m_mpRevisions.end(); ++cit)
			{
				IVdfnPort *pPort = (*cit).first;
				if( pPort->GetUpdateCounter() != (*cit).second )
				{
					// update score
					m_mpRevisions[ pPort ] = pPort->GetUpdateCounter();
					return ++m_nUpdateScore; // leave loop on first trigger!
				}
			}
		}
		else if(m_eMd == MD_TRIGGER_AND)
		{
			REVMAP::size_type n = 0;
			for(REVMAP::iterator cit = m_mpRevisions.begin();
				cit != m_mpRevisions.end(); ++cit)
			{
				IVdfnPort *pPort = (*cit).first;
				if( pPort->GetUpdateCounter() != (*cit).second )
				{
					++n;
					(*cit).second = pPort->GetUpdateCounter();
				}
			}
			if( n == m_mpRevisions.size() )
			{
				return ++m_nUpdateScore; // increase counter, indicating change
			}
		}
	}
	return m_nUpdateScore;
}

VdfnSamplerNode::eMd VdfnSamplerNode::GetTriggerMode() const
{
	return m_eMd;
}

void VdfnSamplerNode::SetTriggerMode( eMd md )
{
	m_eMd = md;
}

// ############################################################################

IVdfnNode *VdfnSamplerNodeCreate::CreateNode(const VistaPropertyList &oParams) const
{
	const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

	std::list<std::string> liTriggers;
	subs.GetValue( "triggerlist", liTriggers );
	
	VdfnSamplerNode::eMd md = VdfnSamplerNode::MD_TRIGGER_OR;

	std::string sMode;
	if( subs.GetValue( "mode", sMode ) && sMode == "and" )
		md = VdfnSamplerNode::MD_TRIGGER_AND;

	VdfnSamplerNode *pNode = new VdfnSamplerNode(liTriggers);
	pNode->SetTriggerMode(md);

	return pNode;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


