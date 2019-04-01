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


#include "VistaInteractionManager.h"

#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/InteractionManager/VistaInteractionContext.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaConnectionUpdater.h>
#include <VistaDeviceDriversBase/VistaSensorReadState.h>
#include <VistaDeviceDriversBase/VistaSensorReader.h> 
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverLoggingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverForceFeedbackAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <VistaDataFlowNet/VdfnPersistence.h>
#include <VistaDataFlowNet/VdfnGraph.h>

#include <VistaDataFlowNet/VdfnPersistence.h>
#include <VistaDataFlowNet/VdfnGraph.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnObjectRegistry.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaAspects/VistaObserver.h>
#include <VistaAspects/VistaConversion.h>

#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cstdio>
#include <algorithm>
#include <cassert>

namespace
{
	class FindByDriver
	{

	public:
		FindByDriver( IVistaDeviceDriver *pDriver )
			: m_pDriver( pDriver )
		{}

		bool operator()( const VistaInteractionManager::DRIVER &driver ) const
		{
			return ( driver.m_pDriver == m_pDriver );
		}

		IVistaDeviceDriver *m_pDriver;
	};


	class VistaInteractionManagerReadstateMapSource : public IVddReadstateSource
	{
	public:
		VistaInteractionManagerReadstateMapSource( VistaInteractionManager *parent )
			: m_parent( parent )
		{}

		virtual std::map<VistaDeviceSensor*, VistaSensorReadState*> GetReadStatesForDriver( IVistaDeviceDriver *pDriver )
		{
			const VistaInteractionManager::READSTATEVECTOR *v = m_parent->GetReadStatesForDriver( pDriver );
			std::map<VistaDeviceSensor*, VistaSensorReadState*> ret;
			if( v == NULL )
			{
				return ret; // empty map
			}

			assert( (*v).size() == pDriver->GetNumberOfSensors() );

			for( unsigned int n = 0; n < pDriver->GetNumberOfSensors(); ++n )
			{
				ret[ pDriver->GetSensorByIndex(n) ] = (*v)[n]; 
			}
			return ret; // copy of v
		}

		virtual VistaSensorReadState *      GetReadState( VistaDeviceSensor &sensor )
		{
			if( sensor.GetParent() ) 
			{
				std::map<VistaDeviceSensor*, VistaSensorReadState*> m = GetReadStatesForDriver( sensor.GetParent() );
				std::map<VistaDeviceSensor*, VistaSensorReadState*>::iterator it = m.find( &sensor );
				if( it == m.end() ) 
				{
					// error?
					vstr::errp() << "Could not retrieve read state for sensor [" 
						         << sensor.GetSensorName() 
								 << "]";
					return 0;
				}
				return (*it).second;
			}
			return 0;
		}

		VistaInteractionManager *m_parent;
	};

	class VistaReadStateSourceWrapper : public IVistaReadStateSource
	{
	public:
		VistaReadStateSourceWrapper( IVddReadstateSource *real_source )
			: IVistaReadStateSource()
			, _real_source( real_source ) {}

		
		virtual VistaSensorReadState *      GetReadState( VistaDeviceSensor &sensor )
		{
			return _real_source->GetReadState( sensor );
		}


		IVddReadstateSource *_real_source;
	};

} // anon namespace


class VistaInteractionUpdateObserver : public IVistaObserver
{
public:

	class MapObj2Msg
	{
	public:
		MapObj2Msg(VistaInteractionContext *pCtx, int nMsg)
			: m_pCtx(pCtx), m_nMsg(nMsg)
			{

			}

		bool operator==(const MapObj2Msg &oOther) const
			{
				if(oOther.m_nMsg >= 0)
					return ((m_pCtx == oOther.m_pCtx)
							&& m_nMsg == oOther.m_nMsg);
				else
					return (m_pCtx == oOther.m_pCtx);
			}

		bool operator<(const MapObj2Msg &oOther) const
			{
				return (m_nMsg < oOther.m_nMsg);
			}

		VistaInteractionContext *m_pCtx;
		int                       m_nMsg;
	};


	VistaInteractionUpdateObserver()
	: IVistaObserver(),
		m_mpUpdates(VistaInteractionEvent::VEID_LAST)
	{

	}

	virtual ~VistaInteractionUpdateObserver()
	{
		for(std::vector<VistaInteractionContext*>::size_type z = 0;
			z < m_vecObsVec.size(); ++z)
		{
			m_vecObsVec[z]->DetachObserver(this);
		}
	}

	virtual bool ObserveableDeleteRequest(IVistaObserveable *pObserveable, int)
	{
		return true;
	}

	virtual void ObserveableDelete(IVistaObserveable *pObserveable, int nTicket)
	{
		ReleaseObserveable(pObserveable, nTicket);
	}

	virtual void ReleaseObserveable(IVistaObserveable *pObserveable, int)
	{
		// d-cast to correct pointer type (hopefully)
		VistaInteractionContext *pObj = dynamic_cast<VistaInteractionContext*>(pObserveable);

		// find observeable in all event type lists
		for(std::vector< MapObj2Msg >::size_type n = 0;
			n < m_mpUpdates.size(); ++n)
		{
			std::vector<MapObj2Msg> &v = m_mpUpdates[n];

			std::vector<MapObj2Msg>::iterator it =
				std::remove(v.begin(), v.end(), MapObj2Msg(pObj,-1));

			if(it != v.end())
				v.erase(it, v.end());
		}

		// remove from observer list
		std::vector<VistaInteractionContext *>::iterator vit = std::remove(m_vecObsVec.begin(),
																			m_vecObsVec.end(), pObj);

		if(vit != m_vecObsVec.end())
			m_vecObsVec.erase(vit, m_vecObsVec.end());


		// finally, detach from the object itself
		pObserveable->DetachObserver(this);
	}

	virtual void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket)
	{
	}

	virtual bool Observes(IVistaObserveable *pObserveable)
	{
		VistaInteractionContext *pObj = dynamic_cast<VistaInteractionContext*>(pObserveable);
		if(!pObj)
			return false;

		const std::vector<VistaInteractionContext *>::iterator vit = std::remove(m_vecObsVec.begin(),
																					m_vecObsVec.end(), pObj);

		return (vit != m_vecObsVec.end());
	}

	virtual void Observe(IVistaObserveable *pObservable, int eTicket=IVistaObserveable::TICKET_NONE)
	{
		VistaInteractionContext *pCtx = dynamic_cast<VistaInteractionContext*>(pObservable);
		if(pCtx)
		{
			m_vecObsVec.push_back(pCtx);
			pCtx->AttachObserver(this);
		}
	}


	void ResetUpdateWatch()
	{
		for(std::vector< std::vector<VistaInteractionContext*> >::size_type n=0;
			n < m_mpUpdates.size(); ++n)
		{
			std::vector<MapObj2Msg> &v = m_mpUpdates[n];
			v.clear();
		}
	}

	std::vector< std::vector< MapObj2Msg > > m_mpUpdates;
	std::vector< VistaInteractionContext * > m_vecObsVec;
protected:
private:

};

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


static bool SetupNodeWithTag( const std::string& sNodeName, IVdfnNode *pNode )
{
	std::string sNodeTag;
	pNode->GetUserTag( sNodeTag );
	// create a list of strings
	std::list<std::string> liTokens;
	VistaConversion::FromString( sNodeTag, liTokens, ';' );

	for(std::list<std::string>::const_iterator cit = liTokens.begin();
		   cit != liTokens.end(); ++cit)
	{
		std::list<std::string> cmd;
		VistaConversion::FromString( *cit, cmd, ':' );
		if(cmd.size() != 2)
			continue;

		std::vector<std::string> ve( cmd.begin(), cmd.end() );

		if( ve[0] == "enable" )
		{
			// get node list
			std::list<std::string> liNames;
			VistaConversion::FromString( ve[1], liNames, ',' );
			vstr::outi() << "node name=" << sNodeName << std::endl;
			if( liNames.size() == 1 )
			{
				if( liNames.front() == "-" )
				{
					pNode->SetIsEnabled(false);
					continue;
				}
			}

			for(std::list<std::string>::const_iterator k = liNames.begin();
						 k != liNames.end(); ++k )
			{

				if(VistaAspectsComparisonStuff::StringEquals( sNodeName, *k, false ) )
				{
					vstr::outi() << "ENABLE node [" << pNode->GetNameForNameable() << "]" << std::endl;
					pNode->SetIsEnabled(true);
				}
				else
				{
					pNode->SetIsEnabled(false);
					vstr::outi() << "DISABLE node [" << pNode->GetNameForNameable() << "]" << std::endl;
				}
			}
		}
	}

	return true;
}




class DriverUpdateCallback : public IVistaExplicitCallbackInterface
{
public:
	DriverUpdateCallback(IVistaDeviceDriver *pDriver)
		: m_pDriver(pDriver),
		  m_pPostUpdate(NULL)
		{
		}


	~DriverUpdateCallback()
		{
			delete m_pPostUpdate;
		}

	virtual bool Do()
		{
			m_pDriver->PreUpdate();
			bool bPostUpdate = m_pDriver->Update();
			m_pDriver->PostUpdate();

			if(bPostUpdate && m_pPostUpdate)
				m_pPostUpdate->PostDriverUpdate(m_pDriver);
			return true;
		}

	IVistaDeviceDriver *m_pDriver;
	VistaInteractionManager::IVistaPostUpdateFunctor *m_pPostUpdate;
};


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaInteractionManager::VistaInteractionManager( VistaEventManager *pEventManager )
: VistaEventHandler()
, m_pEventManager( pEventManager )
, m_pDriverMap( NULL )
, m_pUpdater( new VistaConnectionUpdater )
, m_pEventObserver( NULL )
, m_pInteractionEvent( NULL )
, m_bProcessingEvents( false )
, m_nRoleIdCount(0)
, m_bIsActiveProducer( false)
, m_bSwallowEvent( true )
, m_pAvgUpd( new VistaWeightedAverageTimer )
, m_pReadStateCreator( 0 )
{
	m_pReadStateCreator = new VistaInteractionManagerReadstateMapSource( this );
	m_pInteractionEvent = new VistaInteractionEvent( this );
	m_pEventManager->AddEventHandler(this, VistaSystemEvent::GetTypeId(),
							  VistaSystemEvent::VSE_UPDATE_INTERACTION,
							  VistaEventManager::PRIO_HIGH + 100);
	m_pEventManager->AddEventHandler(this, VistaSystemEvent::GetTypeId(),
							  VistaSystemEvent::VSE_UPDATE_DELAYED_INTERACTION,
							  VistaEventManager::PRIO_HIGH + 100);

	// register for events on graphs and contexts
	// will trigger a call to ''EvalContext'' on the context signature
	m_pEventManager->AddEventHandler(this,
							  VistaInteractionEvent::GetTypeId(),
							  VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE );
}

VistaInteractionManager::~VistaInteractionManager()
{
	// delete and stop updater first, otherwise
	// one of the drivers being deleted might be still in use
	delete m_pUpdater;
	delete m_pEventObserver;

	// remove all instances of me in a single call
	m_pEventManager->RemEventHandler(this, VistaEventManager::NVET_ALL,
							  VistaEventManager::NVET_ALL);

	for( std::vector<DRIVER>::iterator it = m_vecInFrameUpdates.begin();
		it != m_vecInFrameUpdates.end(); ++it )
	{
		delete (*it).m_pPostUpdate;
	}

	for( std::vector<CONTEXT>::iterator cit = m_vecInteractionContexts.begin();
		cit != m_vecInteractionContexts.end(); ++cit )
	{
		delete (*cit).m_pContext;
	}

	delete m_pInteractionEvent;
	delete m_pAvgUpd;

	for( READSTATEMAP::iterator it = m_mapReadStates.begin(); it != m_mapReadStates.end(); ++it )
	{
		for( READSTATEVECTOR::iterator tit = (*it).second.begin(); tit != (*it).second.end(); ++tit )
		{
			delete (*tit);
		}
	}

	delete m_pReadStateCreator;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaInteractionManager::Init( VistaDriverMap* pDriverMap,
									bool bIsLeaderInCluster )
{
	m_bIsActiveProducer = bIsLeaderInCluster;
	vstr::outi() << "VistaInteractionManager() -- Created as "
			  << ( m_bIsActiveProducer ? "ACTIVE" : "PASSIVE" )
			  << " InteractionManager" << std::endl;

	if( m_bIsActiveProducer )
		m_pEventObserver = new VistaInteractionUpdateObserver;

	m_pDriverMap = pDriverMap;	
	m_pDriverMap->SetReadstateSource( m_pReadStateCreator );
	return true;
}

VistaType::microtime VistaInteractionManager::GetAvgUpdateTime() const
{
	return m_pAvgUpd->GetAverageTime();
}

struct VistaInteractionManager::_qCompPrio
{
	bool operator()(const VistaInteractionManager::DRIVER &left,
					const VistaInteractionManager::DRIVER &right) const
	{
		return left.m_nPrio > right.m_nPrio;
	}
};

struct VistaInteractionManager::_qCompPrioCtx
{
	bool operator()(const VistaInteractionManager::CONTEXT &left,
					const VistaInteractionManager::CONTEXT &right) const
	{
		return left.m_nPrio > right.m_nPrio;
	}
};


bool VistaInteractionManager::DRIVER::operator==(const DRIVER &other) const
{
	return this->m_pDriver == other.m_pDriver;
}

bool VistaInteractionManager::CONTEXT::operator==(const CONTEXT &other) const
{
	return m_pContext == other.m_pContext;
}

// we want an order from largest to smallest small < large
// for contexts, so we invert the normal order here
bool VistaInteractionManager::CONTEXT::operator< ( const CONTEXT &oContext ) const
{
	if( m_bDelayedUpdate == oContext.m_bDelayedUpdate )
		return m_nPrio > oContext.m_nPrio;
	else
		return !m_bDelayedUpdate;
}


bool VistaInteractionManager::AddDeviceDriver( const std::string &sName,
												  IVistaDeviceDriver *pDriver,
												  int nPrio )
{
	if( m_pDriverMap->AddDeviceDriver(sName, pDriver) )
	{
		std::vector<VistaSensorReadState*> rdStates;
		for( unsigned int n=0; n < pDriver->GetNumberOfSensors(); ++n )
		{
#ifdef VISTA_TRANSMIT_INCREMENTAL_HISTORIES
			rdStates.push_back( new VistaSensorReadState( pDriver->GetSensorByIndex( n ), true ) );
#else
			rdStates.push_back( new VistaSensorReadState( pDriver->GetSensorByIndex( n ), false ) );
#endif
		}
		m_mapReadStates[ pDriver ] = rdStates;
		return ( m_bIsActiveProducer && ActivateDeviceDriver( pDriver, nPrio ) );
	}
	return true;
}

bool VistaInteractionManager::ActivateDeviceDriver( IVistaDeviceDriver *pDriver,
													   int nPrio )
{
	switch(pDriver->GetUpdateType())
	{
		case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
		{
			m_vecInFrameUpdates.push_back(DRIVER(pDriver, nPrio));
			// resort the list according to the priority
			// as addition can change the update order
			std::stable_sort(m_vecInFrameUpdates.begin(),
							 m_vecInFrameUpdates.end(),
							 _qCompPrio());
			return true;
		}
		case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
		{
			VistaDriverConnectionAspect *pConAsp =
				dynamic_cast<VistaDriverConnectionAspect*>(
					pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId()));

			if(!pConAsp)
				return false;


			if(pConAsp->GetConnection())
			{
				m_pUpdater->AddConnectionUpdate(pConAsp->GetConnection(pConAsp->GetUpdateConnectionIndex()),
												new DriverUpdateCallback(pDriver));

				return true;
			}
			break;

		}
		case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
		{
			// record the driver in the threadupdates vector, but do not start now
			// start later
			m_vecThreadedUpdates.push_back(pDriver);
			break;
		}
		default:
			break;
	}

	return false;
}

bool VistaInteractionManager::GetIsDeviceDriver(const std::string &sName) const
{
	return m_pDriverMap->GetIsDeviceDriver(sName);
}

IVistaDeviceDriver *VistaInteractionManager::GetDeviceDriver(const std::string &sName) const
{
	return m_pDriverMap->GetDeviceDriver(sName);
}

bool VistaInteractionManager::DelDeviceDriver(const std::string &sName)
{
	IVistaDeviceDriver *pDriver = GetDeviceDriver(sName);
	if(!pDriver)
		return false;
	switch(pDriver->GetUpdateType())
	{
		case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
		{
			RemoveFromUpdateList(pDriver);
			break;
		}
		case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
		{
			// remove from update handler!
			VistaDriverConnectionAspect *pConAsp =
				dynamic_cast<VistaDriverConnectionAspect*>(
					pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId())
					);

			if(!pConAsp)
				return false;

			IVistaExplicitCallbackInterface *pI = m_pUpdater->RemConnectionUpdate(pConAsp->GetConnection(pConAsp->GetUpdateConnectionIndex()));
			delete pI;
			break;
		}
		case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
		{
			VistaDriverThreadAspect *pThreadAspect =
				dynamic_cast<VistaDriverThreadAspect*>(
				pDriver->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
			if(!pThreadAspect)
				return false;

			if(pThreadAspect->StopProcessing())
			{
				std::vector<IVistaDeviceDriver*>::iterator it = std::remove(m_vecThreadedUpdates.begin(), m_vecThreadedUpdates.end(), pDriver);
				m_vecThreadedUpdates.erase(it, m_vecThreadedUpdates.end());
			}
			else
				return false;
			break;
		}
		default:
			break;
	}

	return m_pDriverMap->DelDeviceDriver(sName);
}

bool VistaInteractionManager::RemDeviceDriver(const std::string &sName)
{
	IVistaDeviceDriver *pDriver = GetDeviceDriver(sName);
	if(pDriver)
	{
		READSTATEMAP::iterator it = m_mapReadStates.find( pDriver );
		for( READSTATEVECTOR::iterator tit = (*it).second.begin(); tit != (*it).second.end(); ++tit )
		{
			delete (*tit);
		}

		m_mapReadStates.erase(it);
		
		// do the logical work
		return m_pDriverMap->RemDeviceDriver(sName);
	}

	return false;
}

IVistaDeviceDriver *VistaInteractionManager::RemAndGetDeviceDriver( const std::string &sName )
{
	IVistaDeviceDriver *pDriver = GetDeviceDriver(sName);
	if( pDriver )
	{
		switch( pDriver->GetUpdateType() )
		{
			case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
			{
				RemoveFromUpdateList( pDriver );
				break;
			}
			case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
			{
				// remove from update handler!
				VistaDriverConnectionAspect *pConAsp =
					dynamic_cast<VistaDriverConnectionAspect*>(
						pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId()) );

				if(!pConAsp)
					return NULL;

				// remove from update handler!
				IVistaExplicitCallbackInterface *pI = m_pUpdater->RemConnectionUpdate(pConAsp->GetConnection(pConAsp->GetUpdateConnectionIndex()));
				delete pI;
				break;
			}
			case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
			{
				VistaDriverThreadAspect *pThreadAspect =
					dynamic_cast<VistaDriverThreadAspect*>(
					pDriver->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
				if( !pThreadAspect )
					return NULL;

				if( pThreadAspect->StopProcessing() )
				{
					std::vector<IVistaDeviceDriver*>::iterator it = std::remove(m_vecThreadedUpdates.begin(), m_vecThreadedUpdates.end(), pDriver);
					m_vecThreadedUpdates.erase(it, m_vecThreadedUpdates.end());
				}
				else
					return NULL;
				break;
			}
			default:
				break;
		}
	}

	return m_pDriverMap->RemAndGetDeviceDriver( sName );
}

VistaDriverMap* VistaInteractionManager::GetDriverMap() const
{
	return m_pDriverMap;
}

void VistaInteractionManager::SetDriverMap( VistaDriverMap* pMap )
{
	if(!m_pDriverMap) 
	{
		m_pDriverMap = pMap;
		m_pDriverMap->SetReadstateSource( this->m_pReadStateCreator );
	}
}


bool VistaInteractionManager::SetPostUpdateFunctor(const std::string &sDriverName,
													   IVistaPostUpdateFunctor *pFunctor)
{
	IVistaDeviceDriver *pDriver = GetDeviceDriver(sDriverName);

	if(pDriver)
	{
		switch(pDriver->GetUpdateType())
		{
			case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
			{
				for(std::vector<DRIVER>::iterator it = m_vecInFrameUpdates.begin();
					it != m_vecInFrameUpdates.end(); ++it)
				{
					if( (*it).m_pDriver == pDriver )
					{
						(*it).m_pPostUpdate = pFunctor; // this might leak iff user is stoopid
						return true;
					}
				}
				break;
			}
			case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
			{
				// retrieve connection callback
				VistaDriverConnectionAspect *pConAsp =
					dynamic_cast<VistaDriverConnectionAspect*>(
					pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId())
					);
				// ok,
				if(pConAsp && pConAsp->GetConnection())
				{
					DriverUpdateCallback *pCb = dynamic_cast<DriverUpdateCallback*>(m_pUpdater->GetConnectionUpdate(pConAsp->GetConnection()));
					if(pCb)
					{
						pCb->m_pPostUpdate = pFunctor;
						return true;
					}
				}
				break;
			}
			case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
			{
				VistaDriverThreadAspect *pThreadAspect =
					dynamic_cast<VistaDriverThreadAspect*>(
					pDriver->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
				if(!pThreadAspect)
					return false;
				pThreadAspect->StopProcessing();

				/** @todo post update functor setting */


				pThreadAspect->StartProcessing();
				return true;
			}
			default:
				break;
		}
	}
	return false;
}

VistaInteractionManager::IVistaPostUpdateFunctor *VistaInteractionManager::GetPostUpdateFunctor(const std::string &sDriverName) const
{
	IVistaDeviceDriver *pDriver = GetDeviceDriver(sDriverName);

	if(pDriver)
	{
		switch(pDriver->GetUpdateType())
		{
			case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
			{
				for(std::vector<DRIVER>::const_iterator it = m_vecInFrameUpdates.begin();
					it != m_vecInFrameUpdates.end(); ++it)
				{
					if( (*it).m_pDriver == pDriver )
					{
						return (*it).m_pPostUpdate;
					}
				}
				break;
			}
			case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
			{
				// retrieve connection callback
				VistaDriverConnectionAspect *pConAsp =
					dynamic_cast<VistaDriverConnectionAspect*>(
					pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId())
					);
				// ok,
				if(pConAsp && pConAsp->GetConnection())
				{
					DriverUpdateCallback *pCb = dynamic_cast<DriverUpdateCallback*>(m_pUpdater->GetConnectionUpdate(pConAsp->GetConnection()));
					if(pCb)
					{
						return pCb->m_pPostUpdate;
					}
				}
				break;
			}
			case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
			{
				break;
			}
			default:
				break;
		}
	}

	return NULL;
}

bool VistaInteractionManager::GetSwallowUpdateEvent() const
{
	return m_bSwallowEvent;
}

void VistaInteractionManager::SetSwallowUpdateEvent(bool bDone)
{
	m_bSwallowEvent = bDone;
}

bool VistaInteractionManager::AddInteractionContext(VistaInteractionContext *pCtx,
													 int nPriority,
													 bool bDelayedUpdate)
{
	if(pCtx->m_bRegistered)
		return false;

	if(pCtx->GetRoleId() == ~0u)
	{
		// create dummy role
		/** @todo this should be smarter somehow */
		char buffer[256];
		sprintf(buffer, "__dummyrole_%d", m_nRoleIdCount);
		pCtx->SetRoleId(RegisterRole(buffer));
	}

	if( GetInteractionContextByRoleId(pCtx->GetRoleId()) )
	{
		vstr::warnp() << " [VistaInteractionMgr]: Context with role ["
				  << GetRoleForId(pCtx->GetRoleId())
				  << "] already registered" << std::endl;
		return false; // no way to assign a context when the role is already there
	}

	m_vecInteractionContexts.push_back( CONTEXT(pCtx, nPriority, bDelayedUpdate) );
	// sort vector
	std::stable_sort(m_vecInteractionContexts.begin(), m_vecInteractionContexts.end());

	if(m_pEventObserver) // master mode
		m_pEventObserver->Observe(pCtx);

	pCtx->m_bRegistered = true;

	return true;
}

void VistaInteractionManager::InitializeGraphContexts( const std::string& sNodeTag )
{
	for(std::vector<CONTEXT>::iterator itContext = m_vecInteractionContexts.begin();
		itContext != m_vecInteractionContexts.end(); ++itContext )
	{
		if( LoadGraphForContext( (*itContext).m_pContext, sNodeTag ) == false )
			continue;		

		(*itContext).m_pContext->Evaluate(0); // calls "init" on graph
	}
}


bool VistaInteractionManager::LoadGraphForContext( VistaInteractionContext* pContext,
												const std::string& sNodeTag )
{
	std::string sGraphFile = pContext->GetGraphSource();
	std::string sRole = GetRoleForId( pContext->GetRoleId() );
	if( sGraphFile.empty() )
	{
		vstr::warnp() 
				<< "Can't load context for role ["
				<< sRole << "] - no graph file spcified" << std::endl;
		return false;
	}

	vstr::outi() << "Loading DFN graph [" << sGraphFile 
			<< "] for Context with Role [" << sRole << "]" << std::endl;
	vstr::IndentObject oIndent;
	VdfnGraph *pGraph = VdfnPersistence::LoadGraph( sGraphFile, sRole, false, true );

	if( pGraph == NULL )
	{
		vstr::warnp() << "Failed loading graph!" << std::endl;
		return false;
	}
	else
	{
		vstr::outi() << "Successfully loaded graph context!!" << std::endl;

		// before we set the graph, we search for nodes with user tag
		VdfnGraph::Nodes liNodes = pGraph->GetNodesWithTag();
		for( VdfnGraph::Nodes::iterator itNode = liNodes.begin();
				itNode != liNodes.end(); ++itNode )
		{
			SetupNodeWithTag( sNodeTag, *itNode );
		}

		// note that the graph is not yet evaluated to 0, which means
		// it is not really usable. The Evaluate(0) is called *before* Run()
		pGraph->SetCheckMasterSim( !m_bIsActiveProducer );
	}

	pContext->SetTransformGraph( pGraph );

	return true;
}

bool VistaInteractionManager::ReloadGraphForContext( VistaInteractionContext* pContext,
												const std::string& sNodeTag,
												const bool bDumpGraphAsDot,
												const bool bWritePortsToDump )
{
	delete pContext->GetTransformGraph();
	
	if( LoadGraphForContext( pContext, sNodeTag ) == false )
	{
		pContext->SetTransformGraph( NULL );
		return false;	
	}

	pContext->Evaluate(0);

	if( bDumpGraphAsDot )
	{
		std::string sGraphName = GetRoleForId( pContext->GetRoleId() );
		std::string sFileName = sGraphName + std::string(".dot");
		VdfnPersistence::SaveAsDot( pContext->GetTransformGraph(), sFileName,
									sGraphName, bWritePortsToDump, true );
	}
	return true;
}

void VistaInteractionManager::DumpGraphsToDot(bool bWritePorts) const
{
	for(std::vector<CONTEXT>::const_iterator it = m_vecInteractionContexts.begin();
		it != m_vecInteractionContexts.end(); ++it)
	{
        VdfnGraph *pGraph = (*it).m_pContext->GetTransformGraph();
        if(pGraph)
        {
            pGraph->EvaluateGraph(0);
            std::string strGraphName = GetRoleForId( (*it).m_pContext->GetRoleId() );
            std::string strFileName = strGraphName + std::string(".dot");
            VdfnPersistence::SaveAsDot( pGraph, strFileName, strGraphName, bWritePorts, true );
        }
	}
}

void VistaInteractionManager::HandleEvent(VistaEvent *pEvent)
{
	if(pEvent->GetType() == VistaInteractionEvent::GetTypeId()
		&& pEvent->GetId() == VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE)
	{
		if( !m_pEventObserver ) // slave mode?
		{
			m_pAvgUpd->StartRecording();
			// slaves will call evaluate on the context upon receival of this event
			VistaInteractionEvent *pInEv = static_cast<VistaInteractionEvent*>(pEvent);
			VistaInteractionContext *pCtx = pInEv->GetInteractionContext();
			if(pCtx->Evaluate(pEvent->GetTime())==false)
			{
				// something went definitely wrong!

			}
			pEvent->SetHandled(m_bSwallowEvent); // swallow event			
			m_pAvgUpd->RecordTime();
		}
		return;
	}
	else if( (pEvent->GetType() == VistaSystemEvent::GetTypeId())
		&& ( (pEvent->GetId() == VistaSystemEvent::VSE_UPDATE_INTERACTION)
		|| (pEvent->GetId() == VistaSystemEvent::VSE_UPDATE_DELAYED_INTERACTION) ))
	{
		// "update interaction" event caught
		// this is *always* before POSTAPP, as POSTAPP is the last
		// event before the highway

		if(!m_pEventObserver) // slave mode?
		{
			// yes
			pEvent->SetHandled(m_bSwallowEvent); // swallow event
									  // (other events will be injected by the cluster)
			return;
		}


		m_pAvgUpd->StartRecording();
		// this is master-code below

		// update in-frame device drivers
		VistaKernelProfileStartSection( "DRIVERS_UPDATE" );
		for(std::vector<DRIVER>::iterator it = m_vecInFrameUpdates.begin();
			it != m_vecInFrameUpdates.end(); ++it)
		{
			if((*it).m_pDriver->Update() == true)
			{
				if((*it).m_pPostUpdate)
					(*it).m_pPostUpdate->PostDriverUpdate( (*it).m_pDriver );
			}
		}
		VistaKernelProfileStopSection();

		// swap measures for all the sensors
		// this is also important for in-frame devices,
		// as it will swap the current-read-head
		VistaKernelProfileStartSection( "DRIVERS_SENSORSWAP" );
		VistaDriverMap::iterator dm_it  = m_pDriverMap->begin();
		VistaDriverMap::iterator dm_end = m_pDriverMap->end();

		for(; dm_it != dm_end; ++dm_it)
		{
			READSTATEMAP::iterator tit = m_mapReadStates.find( (*dm_it).second );
			assert( tit != m_mapReadStates.end() );

			READSTATEVECTOR::iterator itReadState = (*tit).second.begin();
			READSTATEVECTOR::iterator itEnd       = (*tit).second.end();
			for( ; itReadState != itEnd; ++itReadState )
			{
				(*itReadState)->SetToLatestSample();
			}
		}

		VistaKernelProfileStopSection();

		// update logical devices aka contexts
		bool bDelayed = ( pEvent->GetId() == VistaSystemEvent::VSE_UPDATE_DELAYED_INTERACTION );
		UpdateWithEventHandling( pEvent->GetTime(), bDelayed );
		// swallow events according to flag set
		pEvent->SetHandled( m_bSwallowEvent );
	}
}

void VistaInteractionManager::UpdateWithEventHandling(double dTs, bool bDelayedUpdate)
{
	VistaKernelProfileScope( "INTERACTIONCTX_UPDATE" );
	for(std::vector<CONTEXT>::iterator ldit = m_vecInteractionContexts.begin();
		ldit != m_vecInteractionContexts.end(); ++ldit)
	{
		if( (*ldit).m_bDelayedUpdate != bDelayedUpdate )
			continue;
		VistaKernelProfileScope( "CTX_" + GetRoleForId( (*ldit).m_pContext->GetRoleId() ) );
		if((*ldit).m_pContext->Update(dTs) == true)
		{
			// this is master only-code, the contexts have a new state on the
			// master (return true of Update() assumes that, so throw an event
			// which is passed to slaves with the current state of the graph

			// this code should *never* be executed in case of slave processing
			m_pInteractionEvent->SetId(VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE);
			m_pInteractionEvent->SetTime(dTs);
			m_pInteractionEvent->SetInteractionContext((*ldit).m_pContext);
			m_pEventManager->ProcessEvent(m_pInteractionEvent);
		}
	}
}

bool VistaInteractionManager::RemoveFromUpdateList(IVistaDeviceDriver *pDev)
{
	std::vector<DRIVER>::iterator it = std::remove(m_vecInFrameUpdates.begin(),
												   m_vecInFrameUpdates.end(), DRIVER(pDev, 0));

	if(it != m_vecInFrameUpdates.end())
	{
		// erase does not change the internal order
		m_vecInFrameUpdates.erase(it, m_vecInFrameUpdates.end());
		return true;
	}
	return false;
}


bool VistaInteractionManager::RemInteractionContext(VistaInteractionContext *pCtx)
{
	std::vector<CONTEXT>::iterator it = std::remove(m_vecInteractionContexts.begin(),
													m_vecInteractionContexts.end(),
													CONTEXT(pCtx,0,false));

	if(it == m_vecInteractionContexts.end())
		return false;
	if(m_bProcessingEvents == false)
	{
		m_vecInteractionContexts.erase(it, m_vecInteractionContexts.end());
		if(m_pEventObserver) // master mode
			m_pEventObserver->ReleaseObserveable(pCtx, IVistaObserveable::TICKET_NONE);
		pCtx->m_bRegistered = false;
	}
	else
	{
		pCtx->SetIsEnabled(false); // make dumb
		m_setRemoveAfterUpdate.insert(pCtx);
	}
	return true;
}


bool VistaInteractionManager::DelInteractionContext(VistaInteractionContext *pCtx)
{
	std::vector<CONTEXT>::iterator it = std::remove(m_vecInteractionContexts.begin(),
													m_vecInteractionContexts.end(),
													CONTEXT(pCtx,0,false));

	if(it == m_vecInteractionContexts.end())
		return false;

	if(m_bProcessingEvents == false)
	{
		for(std::vector<CONTEXT>::iterator cit = it;
				cit != m_vecInteractionContexts.end(); ++cit)
		{
			delete (*cit).m_pContext;
		}

		m_vecInteractionContexts.erase(it, m_vecInteractionContexts.end());
		pCtx->m_bRegistered = false;
	}
	else
	{
		// @TODO m_setDeleteAfterUpdate is never processed
		pCtx->SetIsEnabled(false); // make dumb
		m_setDeleteAfterUpdate.insert(pCtx);
	}

	return true;
}

int VistaInteractionManager::GetInteractionContextPriority( VistaInteractionContext* pContext ) const
{
	for( std::vector<CONTEXT>::const_iterator itCtx = m_vecInteractionContexts.begin();
			itCtx != m_vecInteractionContexts.end(); ++itCtx )
	{
		if( (*itCtx).m_pContext == pContext )
		{
			return (*itCtx).m_nPrio;
		}
	}
	return -1;
}
bool VistaInteractionManager::SetInteractionContextPriority( VistaInteractionContext* pContext,
															int nPriority )
{
	for( std::vector<CONTEXT>::iterator itCtx = m_vecInteractionContexts.begin();
			itCtx != m_vecInteractionContexts.end(); ++itCtx )
	{
		if( (*itCtx).m_pContext == pContext )
		{
			(*itCtx).m_nPrio = nPriority;
			std::stable_sort(m_vecInteractionContexts.begin(), m_vecInteractionContexts.end());
			return true;
		}
	}
	return false;
}

int VistaInteractionManager::GetInteractionContextDelayedUpdate( VistaInteractionContext* pContext ) const
{
	for( std::vector<CONTEXT>::const_iterator itCtx = m_vecInteractionContexts.begin();
			itCtx != m_vecInteractionContexts.end(); ++itCtx )
	{
		if( (*itCtx).m_pContext == pContext )
		{
			return (*itCtx).m_bDelayedUpdate;
		}
	}
	return false;
}
bool VistaInteractionManager::SetInteractionContextDelayedUpdate( 
															VistaInteractionContext* pContext,
															bool bDelayedUpdate )
{
	for( std::vector<CONTEXT>::iterator itCtx = m_vecInteractionContexts.begin();
			itCtx != m_vecInteractionContexts.end(); ++itCtx )
	{
		if( (*itCtx).m_pContext == pContext )
		{
			(*itCtx).m_bDelayedUpdate = bDelayedUpdate;
			std::stable_sort(m_vecInteractionContexts.begin(), m_vecInteractionContexts.end());
			return true;
		}
	}
	return false;
}



bool VistaInteractionManager::StartDriverThread()
{
	if(!m_bIsActiveProducer)
		return true;

	if(m_pUpdater->GetIsDispatching())
		return true;
	bool bRet = m_pUpdater->StartUpdaterLoop();


	// ok, let's go over all drivers and SetEnable() to true
	for(VistaDriverMap::iterator it = (*m_pDriverMap).begin();
		it != (*m_pDriverMap).end(); ++it)
	{
		(*it).second->SetIsEnabled(true);
	}


	// start all drivers in the threaded vector
	for(std::vector<IVistaDeviceDriver*>::iterator it = m_vecThreadedUpdates.begin();
		it != m_vecThreadedUpdates.end(); ++it)
	{
		// get thread aspect
		VistaDriverThreadAspect *pThreadAspect =
			dynamic_cast<VistaDriverThreadAspect*>(
			(*it)->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
		if(!pThreadAspect)
		{
			// no thread aspect... hmm... call enable state
			(*it)->SetIsEnabled(true);
		}
		else
			pThreadAspect->StartProcessing();
	}
	return bRet;
}

bool VistaInteractionManager::StopDriverThread()
{
	if(!m_bIsActiveProducer)
		return true;

	if(m_pUpdater->ShutdownUpdaterLoop() == false)
	{
		// utter error
	}

	// ok, let's go over all drivers and SetEnable() to true
	for(VistaDriverMap::iterator it = (*m_pDriverMap).begin();
		it != (*m_pDriverMap).end(); ++it)
	{
		(*it).second->SetIsEnabled(false); // disable 'em all
	}

	for(std::vector<IVistaDeviceDriver*>::iterator it = m_vecThreadedUpdates.begin();
		it != m_vecThreadedUpdates.end(); ++it)
	{
		// get thread aspect
		VistaDriverThreadAspect *pThreadAspect =
			dynamic_cast<VistaDriverThreadAspect*>(
			(*it)->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
		if(!pThreadAspect)
		{
			// no thread aspect? ... hmm... call disable
			(*it)->SetIsEnabled(false);
		}
		else
			pThreadAspect->StopProcessing();
	}

	return true;
}

bool VistaInteractionManager::RegisterEventTypes(VistaEventManager *pEventMgr)
{
	VistaEventManager::EVENTTYPE eTp(pEventMgr->RegisterEventType("VET_INTERACTION"));
	VistaInteractionEvent::SetTypeId(eTp);

	for(int n = VistaInteractionEvent::VEID_CONTEXT_CHANGE;
		n < VistaInteractionEvent::VEID_LAST; ++n)
		pEventMgr->RegisterEventId(eTp, VistaInteractionEvent::GetIdString(n));

	return true;
}

unsigned int VistaInteractionManager::RegisterRole(const std::string &sRole)
{
	unsigned int nRet = 0;
	if((nRet=GetRoleId(sRole)) == ~0u)
	{
		m_mapRoleIdMap[m_nRoleIdCount] = sRole;
		return m_nRoleIdCount++;
	}
	return nRet;
}

bool VistaInteractionManager::UnregisterRole(const std::string &sRole)
{
	unsigned int nRole = GetRoleId(sRole);
	if(nRole == ~0u)
		return false;

	m_mapRoleIdMap.erase(m_mapRoleIdMap.find(nRole));

	return true;
}

unsigned int VistaInteractionManager::GetRoleId(const std::string &sRole) const
{
	for(ROLEMAP::const_iterator cit = m_mapRoleIdMap.begin();
		cit != m_mapRoleIdMap.end(); ++cit)
	{
		if((*cit).second == sRole)
			return (*cit).first;
	}

	return ~0;
}

bool VistaInteractionManager::GetIsRole(const std::string &sRole) const
{
	return (GetRoleId(sRole) != ~0u);
}

std::string  VistaInteractionManager::GetRoleForId(unsigned int nRoleId) const
{
	ROLEMAP::const_iterator cit = m_mapRoleIdMap.find(nRoleId);
	if(cit == m_mapRoleIdMap.end())
		return "";

	return (*cit).second;
}

VistaInteractionContext *VistaInteractionManager::GetInteractionContextByRoleId(unsigned int nRoleId) const
{
	for(std::vector<CONTEXT>::const_iterator cit = m_vecInteractionContexts.begin();
		cit != m_vecInteractionContexts.end(); ++cit)
	{
		if((*cit).m_pContext->GetRoleId() == nRoleId)
			return (*cit).m_pContext;
	}
	return NULL;
}

int VistaInteractionManager::GetNumInteractionContexts() const
{
	return (int)m_vecInteractionContexts.size();
}
VistaInteractionContext* VistaInteractionManager::GetInteractionContext( const int iIndex )
{
	return m_vecInteractionContexts[iIndex].m_pContext;
}

bool VistaInteractionManager::RegisterDriverCreationMethod(const std::string &sTypeName,
															   IVistaDriverCreationMethod *pMethod,
															   bool bForceRegistration)
{
	if(!m_pDriverMap)
		return false;

	return m_pDriverMap->RegisterDriverCreationMethod(sTypeName, pMethod, bForceRegistration);
}

bool VistaInteractionManager::UnregisterDriverCreationMethod(const std::string &sTypeName,
																 bool bDeleteDriverCreationMethod)
{
	if(!m_pDriverMap)
		return false;

	return m_pDriverMap->UnregisterDriverCreationMethod(sTypeName, bDeleteDriverCreationMethod);
}

IVistaDriverCreationMethod *VistaInteractionManager::GetDriverCreationMethod(const std::string &sTypeName) const
{
	if(!m_pDriverMap)
		return NULL;

	return m_pDriverMap->GetDriverCreationMethod(sTypeName);
}

const VistaInteractionManager::READSTATEVECTOR *VistaInteractionManager::GetReadStatesForDriver( IVistaDeviceDriver *pDriver ) const
{
	READSTATEMAP::const_iterator it = m_mapReadStates.find( pDriver );
	if( it == m_mapReadStates.end() )
		return 0;
	return &(*it).second;
}



