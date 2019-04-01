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


#ifndef _VISTAIOREQUEST_H
#define _VISTAIOREQUEST_H

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <vector>

#ifdef WIN32
#include <Windows.h>
#endif

#include <VistaBase/VistaBaseTypes.h>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/Concurrency/VistaThreadPool.h>
#include <VistaInterProcComm/AsyncIO/VistaIOMultiplexer.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
//class VistaConnection;
class VistaIOScheduler;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IVistaIORequest : public IVistaThreadPoolWorkInstance
{
public:
	// request direction
	enum eRequestDir
	{
		RD_NONE=0,
		RD_IN,
		RD_OUT,
		RD_ERR,
		RD_INOUT
	};

	// request state
   enum eRequestState
	{
		REQ_NONE=0,
		REQ_PENDING,
		REQ_FINISHED,
		REQ_PROCESSING,
		REQ_ABORTED
	};


	virtual ~IVistaIORequest();

	//###################################################
	// STATS
	//###################################################
	eRequestDir GetRequestDir() const;
	eRequestState GetRequestState() const;

 //   virtual HANDLE GetMultiplexHandle() = 0;
	virtual bool RegisterAtMultiplexer (VistaIOMultiplexer* multiplexer, int iTicket, VistaIOMultiplexer::eIODir eDir) = 0;

protected:
	IVistaIORequest();
	/**
	* signals done-condition
	 */
	virtual void PostWork();
	virtual void PreWork();

	/**
	 * Reads things off the connection
	 */
	virtual void DefinedThreadWork() = 0;

	void SetRequestDir(eRequestDir eDir);
	void SetRequestState(eRequestState eState);

private:

	 eRequestState m_eState;
	 eRequestDir   m_eReqDir;
//     VistaIOScheduler*   m_pCallingScheduler;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAIOREQUEST_H
