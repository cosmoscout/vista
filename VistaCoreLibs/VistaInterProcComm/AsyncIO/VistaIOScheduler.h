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


#ifndef _VISTAIOSCHEDULER_H
#define _VISTAIOSCHEDULER_H

#include <map>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaIORequest;
class VistaThreadPool;
class VistaMultiplexerJob;
class VistaIOMultiplexer;
class VistaMutex;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaIOScheduler
{
public:
	VistaIOScheduler(int iRequestThreads, VistaIOMultiplexer *pMultiplexer);
	virtual ~VistaIOScheduler();

	int AddRequest(IVistaIORequest *pRequest);
	void AbortRequest(int ticket);
	bool GetIsRequest(int ticket) const;
	void RemRequest(int ticket);
	void StartMultiplexing();
	void StopMultiplexing();

	void HandleRequest(int iTicket);
	void Cleanup ();
protected:
	void AddRequestToMultiplexer (int ticket, IVistaIORequest *pRequest);
private:
   VistaMutex*         m_mtxPool;
	VistaThreadPool *m_pPool;
	VistaMultiplexerJob *m_pPlexer;
	VistaIOMultiplexer *m_pMultiplexer;

	int m_iPlexerId;

	typedef std::map<int, IVistaIORequest*> REQUESTMAP;
	REQUESTMAP m_mpRequestMap;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAIOSCHEDULER_H
