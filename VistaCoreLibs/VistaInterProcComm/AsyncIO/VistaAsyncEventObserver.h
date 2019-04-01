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


#ifndef _VISTAASYNCEVENTOBSERVER_H
#define _VISTAASYNCEVENTOBSERVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaAspects/VistaObserver.h>
#include <VistaInterProcComm/Concurrency/VistaThreadTask.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <set>
#include <VistaBase/VistaBaseTypes.h> // needed for HANDLE


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaThreadEvent;
class VistaThreadTask;
class VistaAEObserverTask;
class IVistaSerializer;
class VistaTCPSocket;
class VistaTCPServerSocket;
class VistaConnectionIP;
class VistaIOHandleBasedIOMultiplexer;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaAsyncEventObserver : public IVistaObserver
{

public:
	VistaAsyncEventObserver(const std::string &sHostName, int iHostPort);
	virtual ~VistaAsyncEventObserver();

	virtual bool ObserveableDeleteRequest(IVistaObserveable *pObserveable,
										  int nTicket) ;
	virtual void ObserveableDelete(IVistaObserveable *pObserveable, int nTicket) ;
	virtual void ReleaseObserveable(IVistaObserveable *pObserveable, int nTicket) ;
	virtual void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket) ;
	virtual bool Observes(IVistaObserveable *pObserveable) ;
	virtual void Observe(IVistaObserveable *pObservable, int eTicket=IVistaObserveable::TICKET_NONE) ;


	bool StartObserver();
	bool StopObserver();

	// update propagation
	// please subclass and register
	class VISTAINTERPROCCOMMAPI IUpdateTranslator
	{
	public:
		virtual ~IUpdateTranslator();
		virtual bool operator()(IVistaObserveable *pObj,
								int iMsg,
								int iTicket,
								bool &bPrependSize,
								IVistaSerializer &) = 0;
	protected:
		IUpdateTranslator();
	};

	// there can only be one right now
	bool RegisterUpdateTranslator(IUpdateTranslator *);
	IUpdateTranslator *GetUpdateTranslator() const;
protected:
private:

	VistaAEObserverTask *m_pActivatorTask;
	VistaThreadTask *m_pThread;

	std::set<IVistaObserveable*> m_setProConts;
};

class VISTAINTERPROCCOMMAPI VistaAEObserverTask : public IVistaThreadedTask
{
	friend class VistaAsyncEventObserver;
public:
	enum eChannel
	{
		E_FAILURE = -1,
		E_METAEVENT=0,
		E_NOTIFY=2,
		E_MODEL,
		E_SERVER,
		E_CLIENT
	};

	VistaAEObserverTask(const std::string &sHost, int iPort);
	virtual ~VistaAEObserverTask();


	void IndicateModelEvent(IVistaObserveable *pObj, int iMsg, int iTicket);
	void IndicateNotificationEvent();
	void StopProcessing();

	bool RegisterUpdateTranslator(VistaAsyncEventObserver::IUpdateTranslator *pTranslator);
	VistaAsyncEventObserver::IUpdateTranslator *GetUpdateTranslator() const;

	protected:
	 virtual void HandleModelChange();


	public:
	// ############################
	// REDEFINITION FROM THREADTASK
	void PreWork();
	void PostWork();

	protected:
	 void DefinedThreadWork();

	// ############################
	private:

	bool SetupServerSocket();
	bool ShutdownServerSocket();


	void EnableEventSelect(HANDLE iSocketDesc, VistaThreadEvent *pEvent, long iMask);
	void DisableEventSelect(HANDLE iSocketDesc, VistaThreadEvent *pEvent);

	enum eServerEvType
	{
		ETS_UNKNOWN = 0,
		ETS_CLOSE,
		ETS_READPOSSIBLE,
		ETS_ACCEPT
	};

	enum eClientEvType
	{
		ETC_UNKNOWN=0,
		ETC_CLOSE,
		ETC_MESSAGE
	};

	eClientEvType DetermineClientEvent();
	eServerEvType DetermineServerEvent();

	bool SetupClient(VistaTCPSocket *pSock);

	void ReadClient();
	bool DetachClient();

	VistaConnectionIP       *m_pEventChannel;
	struct _objHlp
	{
		_objHlp(IVistaObserveable *pObj,
			int iMsg, int iTicket)
			: m_pObj(pObj),
			m_iMsg(iMsg),
			m_iTicket(iTicket)
		{
		}

		~_objHlp() {}

		IVistaObserveable *m_pObj;
		int m_iMsg,
			m_iTicket;

		bool operator==(const _objHlp &other) const
		{
			return (
				(other.m_pObj == this->m_pObj)
				&&(other.m_iMsg == this->m_iMsg)
				&&(other.m_iTicket == this->m_iTicket) );
		}

		bool operator<(const _objHlp &other) const
		{
			return m_pObj < other.m_pObj;
		}
	};

	VistaMutex m_ChgSetLock;
	std::set<_objHlp> m_ChgSet;

	VistaThreadEvent     *m_pNotificationEvent,
		*m_pModelEvent;

	VistaTCPServerSocket *m_pServer;
	VistaTCPSocket       *m_pClient;
	VistaConnectionIP    *m_pClientCon;

	VistaIOHandleBasedIOMultiplexer *m_pMp;

	VistaAsyncEventObserver::IUpdateTranslator *m_pTranslator;

	std::string m_sHostName;
	int         m_iHostPort;
	bool        m_bConnected;

	VistaThreadEvent *m_pServerEvent, *m_pClientEvent;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAASYNCEVENTOBSERVER_H

