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


#ifndef _VISTAIOHANDLEBASEDMULTIPLEXER_H
#define _VISTAIOHANDLEBASEDMULTIPLEXER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaBase/VistaBaseTypes.h> // needed for HANDLE
#include "VistaIOMultiplexer.h"
#include <vector>
#include <map>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaThreadEvent;
class VistaMutex;
class VistaThreadCondition;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaIOHandleBasedIOMultiplexer
	: public VistaIOMultiplexer
{
public:
	VistaIOHandleBasedIOMultiplexer();

	virtual ~VistaIOHandleBasedIOMultiplexer();

	void Shutdown();


	virtual int Demultiplex(unsigned int nTimeout = ~0 );

	bool AddMultiplexPoint(HANDLE han, int iTicket, eIODir eDir);
	bool RemMultiplexPoint(HANDLE han, eIODir eDir);
	bool RemMultiplexPointByTicket(int iTicket);
	int  GetTicketForHandle(HANDLE han) const;
	void Remedy();
protected:
	bool AddPoint(HANDLE han, int iTicket, eIODir eDir);
	bool RemPoint(HANDLE han, int iTicket, eIODir eDir);
private:


	void PrintHandlemap() const;
	std::vector<HANDLE> m_veHandles;
	typedef std::map<int, HANDLE> TICKETMAP;
	TICKETMAP m_mpTicketMap;
	typedef std::map<HANDLE, std::pair<int, eIODir> > HANDLEMAP;

	//map<int, VistaThreadEvent*> m_mpHandles;

	HANDLEMAP m_mpHandleMap;
	VistaThreadEvent *m_pMetaEvent;
	VistaMutex *m_pActionMutex,
				*m_pDoneMutex,
				*m_pHandleMapMutex;
	VistaThreadCondition *m_pActionDone,
						  *m_pWaitingForAction;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

