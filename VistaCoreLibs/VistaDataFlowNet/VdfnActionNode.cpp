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


#include "VdfnActionNode.h"

#include "VdfnObjectRegistry.h"

#include <VistaAspects/VistaObserver.h>
#include <set>
#include <algorithm>

class VdfnActionNode::_cActObserver : public IVistaObserver
{
public:
	_cActObserver()
		: IVistaObserver(),
		  m_bDirty(true),
		  m_pObj(NULL)
	{}

	~_cActObserver ()
	{
		if(m_pObj)
			m_pObj->DetachObserver(this);
	}

	virtual bool ObserveableDeleteRequest(IVistaObserveable *pObserveable, int nTicket = IVistaObserveable::TICKET_NONE)
	{
		return true;
	}
	virtual void ObserveableDelete(IVistaObserveable *pObserveable, int nTicket = IVistaObserveable::TICKET_NONE)
	{
		ReleaseObserveable(pObserveable, nTicket);
	}

	virtual void ReleaseObserveable(IVistaObserveable *pObserveable, int nTicket = IVistaObserveable::TICKET_NONE)
	{
		if(Observes(pObserveable))
		{
			pObserveable->DetachObserver(this);
			m_pObj = NULL;
		}
	}

	virtual void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket)
	{
		m_bDirty = true;
		m_setDirtyFlags.insert(msg);
	}

	virtual bool Observes(IVistaObserveable *pObserveable)
	{
		return (dynamic_cast<IVistaObserveable*>(m_pObj) == pObserveable);
	}

	virtual void Observe(IVistaObserveable *pObservable, int eTicket=IVistaObserveable::TICKET_NONE)
	{
		if(m_pObj)
			ReleaseObserveable(m_pObj);

		m_pObj = dynamic_cast<IVdfnActionObject*>(pObservable);

		if(m_pObj)
		{
			m_pObj->AttachObserver(this, eTicket);
			m_bDirty = true;
		}
	}

	IVdfnActionObject *m_pObj;
	bool m_bDirty;
	std::set<int> m_setDirtyFlags;
};

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnActionNode::VdfnActionNode( VdfnObjectRegistry *pReg,
								const std::string &strKey,
								IVdfnActionObject *pActionObject)
	: m_pReg(pReg),
	  m_pActionObject( pActionObject ),
	  m_strKey(strKey),
	  m_pObs( new _cActObserver ),
	  m_nCount(0),
	  m_nInCnt(0)
{
	if( pActionObject )
	{
		m_pObs->Observe( m_pActionObject );
		m_sReflectionableType = m_pActionObject->GetReflectionableType();
		UpdatePorts();
	}
}

VdfnActionNode::~VdfnActionNode()
{
	delete m_pObs;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnActionNode::GetIsValid() const
{
	return m_pActionObject != NULL;
}

unsigned int VdfnActionNode::CalcUpdateNeededScore() const
{
	unsigned int nCnt = VdfnShallowNode::CalcUpdateNeededScore();
	if(m_pActionObject->Pull())
		++m_nCount;

	if( nCnt == m_nInCnt)
	{
		// no change on inport
		if(m_pObs->m_bDirty) // check probable outport change
		{
			++m_nCount;
		}
	}
	else
		m_nInCnt = nCnt;

	// change on: either a normal node change happened, or
	// something related to the observable interface triggered
	// a state update.
	return (m_nInCnt + m_nCount);
}

bool VdfnActionNode::PreparePorts()
{
	if( m_pReg != NULL )
	{
		IVdfnActionObject* pNewObject = dynamic_cast<IVdfnActionObject*>(m_pReg->GetObject( m_strKey ));
		if( pNewObject == NULL )
		{
			if(m_pActionObject != NULL)
			{
				m_pObs->ReleaseObserveable( m_pActionObject );
				m_pActionObject = NULL;
				m_sReflectionableType = "";
			}
		}
		else if( pNewObject == m_pActionObject )
		{
			return true;
		}
		else
		{
			if(m_pActionObject != NULL)
				m_pObs->ReleaseObserveable( m_pActionObject );				
			m_pActionObject = pNewObject;
			m_sReflectionableType = m_pActionObject->GetReflectionableType();
			m_pObs->Observe(m_pActionObject);
			return UpdatePorts();
		}
	}
	else
	{
		if(m_pActionObject != NULL)
		{
			m_pObs->ReleaseObserveable( m_pActionObject );
			m_pActionObject = NULL;
			m_sReflectionableType = "";
		}
	}

	return false;
}

bool VdfnActionNode::ReloadActionObject()
{
	if( m_pReg == NULL )
		return false;
	
	IVdfnActionObject* pNewObject = dynamic_cast<IVdfnActionObject*>(m_pReg->GetObject( m_strKey ));
	if( pNewObject == NULL )
	{
		return false;
	}
	else if( pNewObject == m_pActionObject )
	{
		return true;
	}
	
	// ensure that the ActionObjects are of the same type! Otherwise, the getter/setter wont work
	if( pNewObject->GetReflectionableType() != m_sReflectionableType )
	{
		vstr::warnp() << "[VdfnActionNode::ReloadActionObject]: Reloading failed - new object's type ["
					<< pNewObject->GetReflectionableType() << "] differs from old ["
					<< m_sReflectionableType << "]" << std::endl;
		return false;
	}

	if( m_pActionObject != NULL )
		m_pObs->ReleaseObserveable( m_pActionObject );				
	m_pActionObject = pNewObject;
	m_pObs->Observe( m_pActionObject);

	/** @todo ensure correct actionobject setter/getter exist*/

	return true;
}

void print_node_name( const std::string &strName )
{
	vstr::outi() << "\t[" << strName << "]" << std::endl;
}

bool VdfnActionNode::PrepareEvaluationRun()
{
	if( GetIsValid() == false )
	{
		vstr::warnp() << "[VdfnActionNode]: No valid action object found for name [" 
				<< m_strKey << "]" << std::endl;
		return false;
	}

	//for(std::map<std::string, _sHlp>::const_iterator cit = m_mpBuildMap.begin();
	//	cit != m_mpBuildMap.end(); ++cit )
	//{
	//	IVdfnPort *pPort = GetInPort( (*cit).first );
	//	if(pPort)
	//	{
	//		m_liPortSets.push_back( _sHlp( pPort, (*cit).second.m_pSet, (*cit).second.m_pFunctor ) );
	//	}
	//	// this is for debugging, in case you have trouble
	//	//else
	//	//{
	//	//	std::cerr << "[VdfnActionNode]: could not find port named [" << (*cit).first << "]" << std::endl;
	//	//	std::list<std::string> liNames = GetInPortNames();
	//	//	std::for_each( liNames.begin(), liNames.end(), print_node_name );

	//	//	std::cerr << "\nConnected ports:" << std::endl;
	//	//	liNames = GetConnectedInPortNames();
	//	//	std::for_each( liNames.begin(), liNames.end(), print_node_name );
	//	//}
	//}
	//return true; // unconditional validity

	for( std::map<std::string, IVdfnPort*>::iterator itInPort = m_mpInPorts.begin();
			itInPort != m_mpInPorts.end(); ++itInPort )
	{
		std::map<std::string, _sHlp>::const_iterator itRequest = m_mpBuildMap.find( (*itInPort).first );
		if( itRequest != m_mpBuildMap.end() )
		{
			m_liPortSets.push_back( _sHlp( (*itInPort).second, (*itRequest).second.m_pSet, (*itRequest).second.m_pFunctor ) );
		}
		else
		{
			vstr::warnp() << "[VdfnActionNode]: Failed to set inport [" << (*itInPort).first
					<< "] on ActionObject [" << m_pActionObject->GetNameForNameable()
					<< "] - no matching Setter exists!" << std::endl;
		}
	}
	return true; //even if one (or all) ports could not be set... we might use the outports
}

bool VdfnActionNode::DoEvalNode()
{
	// first: check the change of the setter ports
	bool bInportsChanged = false;
	for(std::list<_sHlp>::iterator cit = m_liPortSets.begin();
		cit != m_liPortSets.end(); ++cit)
	{
		// save a few cycles if an inport has not changed, there is no need for a new
		// set, as no new value is propagated to the network
		if( (*cit).m_pPort->GetUpdateCounter() != (*cit).m_nRevision )
		{
			(*cit).m_pSet->Set( m_pActionObject, (*cit).m_pFunctor, (*cit).m_pPort, GetUpdateTimeStamp(), ~0u );

			// copy the revision in the internal records for a later change-check
			(*cit).m_nRevision = (*cit).m_pPort->GetUpdateCounter();
			bInportsChanged = true;
		}
	}

	m_pActionObject->SetUpdateTimeStamp( GetUpdateTimeStamp(), bInportsChanged );

	// now, go for the outports
	if(m_pObs->m_bDirty) // save some cycles if the GET is still valid
	{
		for(std::list<_sOutHlp>::iterator lit = m_mpOutMap.begin();
			lit != m_mpOutMap.end(); ++lit)
		{
			IVdfnPort *pPort = GetOutPort( (*lit).m_strPortName );
			if(pPort)
			{
				(*lit).m_pSet->Get( m_pActionObject, (*lit).m_pGet, pPort, GetUpdateTimeStamp(), ~0u );
			}
		}
		// sideeffect: reset-the dirty flag of the observer
		m_pObs->m_bDirty = false;
		// forget about the flags...
		m_pObs->m_setDirtyFlags.clear();
	}

	return true;
}

bool VdfnActionNode::UpdatePorts()
{
	VdfnPortFactory *pFac = VdfnPortFactory::GetSingleton();

	std::list<std::string> liSyms;
	m_pActionObject->GetPropertySymbolList(liSyms);
	for(std::list<std::string>::const_iterator cit = liSyms.begin();
		cit != liSyms.end(); ++cit )
	{
		IVdfnActionObject::IActionSet *pSet = m_pActionObject->GetActionSet( *cit );

		if(pSet)
		{
			VdfnPortFactory::CFunctorAccess *pFunc = pFac->GetFunctorAccess( pSet->GetSetterType().name() );
			if(pFunc)
			{
				// register as in-port-set-prototype
				IVdfnPortTypeCompare *pComp = pFunc->m_pCreationMethod->CreatePortTypeCompare();
				RegisterInPortPrototype( *cit, pComp );
				m_mpBuildMap[*cit] = _sHlp(NULL, pFunc->m_pGetFunctor, pSet); // for later use in prepare evaluation run
//				std::cout << "[VdfnActionNode::UpdatePorts()] -- adding set for ["
//				          << *cit << "]" << std::endl;
			}
			else
			{
				vstr::warnp() << "[VdfnActionNode::UpdatePorts()] -- not adding ["
				          <<  *cit << "] with type [" << pSet->GetSetterType().name()
						  << "] -- no functor access defined" << std::endl;
			}
		}
//#ifdef DEBUG
//		else
//		{
//			std::cerr << "[VdfnActionNode::UpdatePorts()] -- no action set for ["
//			          << *cit << "]" << std::endl;
//		}
//#endif

		IVdfnActionObject::IActionGet *pGet = m_pActionObject->GetActionGet( *cit );
		if(pGet)
		{
			VdfnPortFactory::CFunctorAccess *pFunc = pFac->GetFunctorAccess( pGet->GetGetterType().name() );
			if(pFunc)
			{
				IVdfnPort *pPort = pFunc->m_pPortCreationMethod->CreatePort();
				if(pPort)
				{
					RegisterOutPort( *cit, pPort );
					m_mpOutMap.push_back(_sOutHlp( *cit, pFunc->m_pGetFunctor, pGet ) );
				}
				else
				{
					vstr::warnp() << "[VdfnActionNode::UpdatePorts()] -- could not create port for ["
						<< *cit << "]" << std::endl;
				}
			}
			else
			{
				vstr::warnp() << "[VdfnActionNode::UpdatePorts()] -- not creating out port for ["
				          << *cit << "] -- no functor access found for type ["
						  << pGet->GetGetterType().name() << "]" << std::endl;
			}
		}

	}

	return true;
}

// #############################################################################

VdfnActionNodeCreate::VdfnActionNodeCreate( VdfnObjectRegistry *pReg )
	: IVdfnNodeCreator(),
	  m_pReg(pReg)
{
}

IVdfnNode *VdfnActionNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
	std::string strObject;
	if( subs.GetValue( "object", strObject ) == false )
	{
		vstr::warnp() << "VdfnActionNodeCreate::CreateNode() -- "
			<< "no parameter for [object] has been specified!" << std::endl;
		return NULL;
	}
	IVdfnActionObject *pObj = dynamic_cast<IVdfnActionObject*>(m_pReg->GetObject( strObject ));
	if(!pObj)
		return new VdfnActionNode( m_pReg, strObject );
	else
		return new VdfnActionNode( m_pReg, strObject, pObj );
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


