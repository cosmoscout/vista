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


#ifndef _VISTAIOMULTIPLEXERIP_H
#define _VISTAIOMULTIPLEXERIP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/


#ifdef WIN32
#include <Windows.h>
#endif

#include <map>
#include <vector>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

#include <VistaBase/VistaBaseTypes.h>

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/AsyncIO/VistaIOMultiplexer.h>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaThreadEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaIOMultiplexerIP : public VistaIOMultiplexer
{
public:
	VistaIOMultiplexerIP();
	virtual ~VistaIOMultiplexerIP();

	int Demultiplex( unsigned int nTimeout = ~0 );
	bool AddMultiplexPoint(HANDLE han, int iTicket, eIODir eDir);
	bool RemMultiplexPoint(HANDLE han);
	bool RemMultiplexPointByTicket(int iTicket);
	int  GetTicketForHandle(HANDLE han) const;
	HANDLE GetHandleForTicket(int iTicket) const;

	/**
	 * Factory method for HANDLE creations
	 */
	virtual HANDLE CreateHandleForDescriptor(int iDesc, eIODir);

	/**
	 * Factory method for HANDLE deletions
	 */
	virtual bool   DeleteHandleForDescriptor(int iDesc, HANDLE han, eIODir);

	eIODir GetDirForHandle(HANDLE) const;
	void Shutdown();
protected:
	bool AddPoint(HANDLE han, int iTicket, eIODir eDir);
	bool RemPoint(HANDLE han, int iTicket);
private:
	std::vector<HANDLE> m_veHandles;
	typedef std::map<int, HANDLE> TICKETMAP;
	TICKETMAP m_mpTicketMap;
	typedef std::map<HANDLE, std::pair<int, eIODir> > HANDLEMAP;

	std::map<int, VistaThreadEvent*> m_mpHandles;

	HANDLEMAP m_mpHandleMap;
	VistaThreadEvent *m_pMetaEvent;
	VistaMutex *m_pActionMutex,
				*m_pDoneMutex,
				*m_pHandleMapMutex;

	VistaThreadCondition *m_pActionDone, *m_pWaitingForAction;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
