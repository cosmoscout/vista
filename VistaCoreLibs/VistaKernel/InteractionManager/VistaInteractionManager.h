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


#ifndef _VISTAINTERACTIONMANAGER_H
#define _VISTAINTERACTIONMANAGER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaBase/VistaBaseTypes.h>


#include <map>
#include <string>
#include <vector>
#include <set>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;
class VistaInteractionContext;
class VistaDriverMap;
class VistaConnectionUpdater;
class VistaInteractionUpdateObserver;
class VistaInteractionEvent;
class VistaWeightedAverageTimer;
class IVddReadstateSource;
class VistaSensorReadState;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaInteractionManager : public VistaEventHandler
{
public:
	VistaInteractionManager( VistaEventManager *pEventManager );
	virtual ~VistaInteractionManager();

	bool Init( VistaDriverMap* pDriverMap,
				bool bIsLeaderInCluster );

	/**
	 *  The update prio can be used to delay update in the
	 *  sequence of the <b>in-frame</b> update devices.
	 *  It is usually ignored for devices that run in their
	 *  own context (threaded) as no information about the
	 *  update frequency can be provided.
	 *  As a rule of thumb, the smaller the number, the later
	 *  the update. For head tracking devices, this should
	 *  be the smallest value possible in the prio in order to
	 *  fetch the latest sensor reading before the next image
	 *  update.
	 */
	enum
	{
		PRIO_LAST     = -1,   /** is updated latest */
		PRIO_DONTCARE =  0,   /** is updated in the order of addition */
		PRIO_MID      = 4096, /** is updated in the middle */
		PRIO_HIGH     = 8192  /** is updated first (and above that value even more first ;)*/
	};

	// ##########################################################
	// CONTEXT API
	// ##########################################################
	/**
	 * Add to the list of interaction contexts, with the given
	 * priority. The smaller the value of the priority, the later the
	 * context is processed. The whole domain of an int can be used for
	 * priorities, for C++ coders some good estimates are defined as enums
	 * in this class (PRIO_LAST, PRIO_DONTCARE, PRIO_MID, PRIO_HIGH).
	 * There is currently no way to change the priority only, if desired,
	 * remove the context and add it again with a new priority.
	 * The DelayedUpdate Flag determines if the context is evaluated in
	 * the normal UPDATE_INTERACTION event (between PRE and POST_APP events)
	 * or in the delayed updade (between POST_APP and PRE_GRAPHICS)
	 * @return true if the context was added without problems
	 * @see RemInteractionContext() for removal
	 * @see DelInteractionContext() for removal and memory disposal
	 */
	bool AddInteractionContext( VistaInteractionContext* pContext,
								int nPriority,
								bool bDelayedUpdate );
	/**
	 * Removes the interaction context from the list of logical
	 * devices.  If events are currently being processed, this might
	 * be delayed until after the next update loop.
	 */
	bool RemInteractionContext( VistaInteractionContext* pContext );
	/**
	 * Deletes the interaction context. Behaves like
	 * RemInteractionContext, but the VistaInteractionContext object
	 * is also deleted. This may also be deferred until after the next
	 * update loop.
	 */
	bool DelInteractionContext( VistaInteractionContext* pContext );

	int GetInteractionContextPriority( VistaInteractionContext* pContext ) const;
	bool SetInteractionContextPriority( VistaInteractionContext* pContext,
										int nPriority );

	int GetInteractionContextDelayedUpdate( VistaInteractionContext* pContext ) const;
	bool SetInteractionContextDelayedUpdate( VistaInteractionContext* pContext,
										bool bDelayedUpdate );

	/**
	 * The api to query for an interaction context by role id.
	 * @see RegisterRole()
	 * @see UnregisterRole()
	 * @see GetRoldId()
	 * @see GetIsRole()
	 * @see GetRoleForId()
	 */
	VistaInteractionContext *GetInteractionContextByRoleId(unsigned int nRoleId) const;
	int GetNumInteractionContexts() const;
	VistaInteractionContext* GetInteractionContext( const int iIndex );

	/**
	 * Loads Context graphs and initializes them by calling EvaluateGraph(0).
	 */
	void InitializeGraphContexts( const std::string& sNodeTag );
	bool LoadGraphForContext( VistaInteractionContext* pContext,
								const std::string& sNodeTag );
	bool ReloadGraphForContext( VistaInteractionContext* pContext,
								const std::string& sNodeTag,
								const bool bDumpGraphAsDot = false,
								const bool bWritePortsToDump = false );
    void DumpGraphsToDot( bool bWritePorts ) const;

    VistaType::microtime GetAvgUpdateTime() const;
	// ##########################################################
	// ROLE API
	// ##########################################################
	unsigned int RegisterRole( const std::string& sRole );
	bool         UnregisterRole( const std::string& sRole );
	unsigned int GetRoleId( const std::string& sRole ) const;
	bool         GetIsRole( const std::string& sRole ) const;
	std::string  GetRoleForId (unsigned int nRoleId ) const;


	// ##########################################################
	// SYSTEM CONTROL API
	// ##########################################################
	bool StartDriverThread();
	bool StopDriverThread();

	// ##########################################################
	// EVENTHANDLER API
	// ##########################################################
	virtual void HandleEvent(VistaEvent *pEvent);
	static bool  RegisterEventTypes(VistaEventManager *pEventMgr);
	bool GetSwallowUpdateEvent() const;
	void SetSwallowUpdateEvent(bool bDone);

	// ##########################################################
	// DRIVER API
	// ##########################################################

	bool                AddDeviceDriver(const std::string &sName,
										IVistaDeviceDriver *pDriver,
										int nPriority = PRIO_DONTCARE);
	bool                ActivateDeviceDriver(IVistaDeviceDriver *pDriver,
											 int nPriority = PRIO_DONTCARE);

	bool                GetIsDeviceDriver(const std::string &sName) const;
	IVistaDeviceDriver *GetDeviceDriver(const std::string &sName) const;

	bool                DelDeviceDriver(const std::string &sName);
	bool                RemDeviceDriver(const std::string &sName);
	IVistaDeviceDriver *RemAndGetDeviceDriver(const std::string &sName);

	VistaDriverMap    *GetDriverMap() const;
	void                SetDriverMap(VistaDriverMap *);

	IVddReadstateSource *GetReadStateSource() const;
	typedef std::vector<VistaSensorReadState*> READSTATEVECTOR;
	typedef std::map< IVistaDeviceDriver*, READSTATEVECTOR > READSTATEMAP;

	const READSTATEVECTOR *GetReadStatesForDriver( IVistaDeviceDriver * ) const;

	// ##########################################################
	// CREATION METHOD API
	// ##########################################################
	bool RegisterDriverCreationMethod( const std::string &sTypeName,
									IVistaDriverCreationMethod *pMethod,
									bool bForceRegistration = false );
	bool UnregisterDriverCreationMethod (const std::string &sTypeName,
									bool bDeleteDriverCreationMethod = true );
	IVistaDriverCreationMethod *GetDriverCreationMethod( const std::string& sTypeName ) const;

	// ##########################################################

	class VISTAKERNELAPI IVistaPostUpdateFunctor
	{
	public:
		virtual ~IVistaPostUpdateFunctor() {}
		virtual bool PostDriverUpdate( IVistaDeviceDriver* pDriver ) = 0;
	};

	bool SetPostUpdateFunctor(const std::string &strDriverName,
							  IVistaPostUpdateFunctor *pFunctor);

	IVistaPostUpdateFunctor *GetPostUpdateFunctor(const std::string &sDriverName) const;


	class DRIVER
	{
	public:
		DRIVER(IVistaDeviceDriver *pDriver, int nPrio )
			: m_pDriver(pDriver), m_nPrio(nPrio), m_pPostUpdate(NULL)
		{
		}

		bool operator==(const DRIVER &) const;

		IVistaDeviceDriver*			m_pDriver;
		int							m_nPrio;
		IVistaPostUpdateFunctor*	m_pPostUpdate;
	};
protected:
private:

	void UpdateWithEventHandling(double dTs, bool bDelayedUpdate);
	bool RemoveFromUpdateList(IVistaDeviceDriver*);

	struct _qCompPrio;
	struct _qCompPrioCtx;



	class CONTEXT
	{
	public:
		CONTEXT( VistaInteractionContext *pCtx, int nPrio, bool bDelayedUpdate )
			: m_pContext(pCtx)
			, m_nPrio(nPrio)
			, m_bDelayedUpdate( bDelayedUpdate )
		{
		}
		bool operator==(const CONTEXT &) const;
		bool operator<(const CONTEXT &) const;

		VistaInteractionContext*	m_pContext;
		int							m_nPrio;
		bool						m_bDelayedUpdate;
	};

	std::vector<DRIVER>					m_vecInFrameUpdates;
	std::vector<IVistaDeviceDriver*>	m_vecThreadedUpdates;
	std::vector<CONTEXT>				m_vecInteractionContexts;
	VistaDriverMap*						m_pDriverMap;
	VistaConnectionUpdater*				m_pUpdater;
	VistaEventManager*					m_pEventManager;
	VistaInteractionUpdateObserver*		m_pEventObserver;
	VistaInteractionEvent*				m_pInteractionEvent;
	VistaWeightedAverageTimer*			m_pAvgUpd;
	bool								m_bProcessingEvents;
	std::set<VistaInteractionContext*>	m_setRemoveAfterUpdate;
	std::set<VistaInteractionContext*>	m_setDeleteAfterUpdate;

	unsigned int						m_nRoleIdCount;
	bool								m_bIsActiveProducer;
	bool								m_bSwallowEvent;


	typedef std::map<unsigned int, std::string> ROLEMAP;
	ROLEMAP m_mapRoleIdMap;

	READSTATEMAP m_mapReadStates;
	IVddReadstateSource               *m_pReadStateCreator;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAINTERACTIONMANAGER_H
