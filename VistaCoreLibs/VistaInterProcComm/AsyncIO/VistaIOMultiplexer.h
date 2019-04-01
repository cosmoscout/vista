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


#ifndef _VISTAIOMULTIPLEXER_H
#define _VISTAIOMULTIPLEXER_H

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


class VistaMutex;
class VistaThreadCondition;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaIOMultiplexer
{
public:
	VistaIOMultiplexer();
	virtual ~VistaIOMultiplexer();

	// Demultiplex function, implement this
	virtual int Demultiplex(unsigned int nTimeout = ~0 ) = 0;

	enum eIODir
	{
		MP_IONONE=0,
		MP_IOIN = 1,
		MP_IOOUT = 2,
		MP_IOERR = 4,
		MP_IOALL = 7
	};


	enum eIOState
	{
		MP_NONE=0,
		MP_IDLE,
		MP_PLEXING,
		MP_DISPATCHING
	};

	eIOState GetState() const;
	int GetNextTicket();
	virtual void Shutdown();
	void WaitForShutdownComplete();

	virtual bool RemMultiplexPointByTicket(int iTicket) = 0;

protected:


	bool SetState(eIOState);
	enum eIOCom
	{
		MPC_NONE=-1,
		MPC_ADDHANDLE=0,
		MPC_REMHANDLE,
		MPC_SHUTDOWN,
		MPC_LAST
	};

	bool SetCommand(eIOCom);

	eIOState m_eState;
	eIOCom   m_eCom;


	VistaMutex *m_pStateMutex, *m_pShutdownMutex,
				*m_pTicketMutex;

	VistaThreadCondition *m_pShutdown;

	int m_iTicket;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

