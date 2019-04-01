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


#ifndef _VISTATICKER_H
#define _VISTATICKER_H

#ifdef WIN32
#include <Windows.h>
#endif

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaBase/VistaBaseTypes.h>

#include <VistaBase/VistaAtomicCounter.h>

#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaThreadEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaTicker
{
public:
	VistaTicker(unsigned int nGranularity = 10);
	virtual ~VistaTicker();


	bool StartTicker();
	bool StopTicker();

	unsigned int GetGranularity() const;

	class VISTAINTERPROCCOMMAPI TriggerContext
	{
		friend class VistaTicker;
	public:
		TriggerContext(int nTimeout, bool bPulsing);
		TriggerContext(int nTimeout, bool bPulsing, VistaThreadEvent *);

		virtual ~TriggerContext();

		HANDLE GetEventWaitHandle() const;

		void Reset();

	private:
		unsigned int m_nTimeout;
		unsigned int m_nCurrent;
		bool         m_bPulsing;
		bool         m_bActive;
		VistaThreadEvent *m_pTrigger;
		bool m_bOwnEvent;
	};

	bool AddTrigger(TriggerContext *);
	bool RemTrigger(TriggerContext *, bool bDelete = false);

	class AfterPulseFunctor
	{
	public:
		virtual ~AfterPulseFunctor() {}
		virtual bool operator()() = 0;
	};

	AfterPulseFunctor *GetAfterPulseFunctor() const;
	void                SetAfterPulseFunctor( AfterPulseFunctor * );

	void UpdateTriggerContexts();

private:
	unsigned int m_nGranularity;
	bool         m_bRunning;
	AfterPulseFunctor *m_pAfterPulseFunctor;

	typedef std::vector<TriggerContext*> TRIGVEC;
	TriggerContext **m_pFirst;
	TRIGVEC::size_type m_nSizeCache;
	TRIGVEC m_vecTriggers;



#if defined(WIN32)

	static void CALLBACK TickerCallback(UINT uTimerID,
							   UINT uMsg,
							   DWORD_PTR dwUser,
							   DWORD_PTR dw1,
							   DWORD_PTR dw2);

	MMRESULT m_Ticker;
	static VistaSigned32Atomic m_nNumTimers;
#else
	class TickerThread;

	TickerThread *m_ticker_thread;

#endif

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATICKER_H

