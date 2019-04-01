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


#ifndef _VISTAINTERPROCCLUSTERBARRIER_H
#define _VISTAINTERPROCCLUSTERBARRIER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterBarrier.h>

#include <vector>
#include <string>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaProcessEventSignaller;
class VistaProcessEventReceiver;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaInterProcClusterLeaderBarrier : public IVistaClusterBarrier
{
public:
	VistaInterProcClusterLeaderBarrier( IVistaClusterBarrier* pDependentBarrier,
										const bool bVerbose = true );

	virtual ~VistaInterProcClusterLeaderBarrier();

	bool AddInterProc( const std::string& sInterProcName );

	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual bool GetIsValid() const;

	virtual std::string GetBarrierType() const;

	virtual int GetSendBlockingThreshold() const;
	virtual bool SetSendBlockingThreshold( const int nNumBytes );

private:
	std::vector<std::pair<VistaProcessEventReceiver*, int> > m_vecReadyEvents;
	std::vector<VistaProcessEventSignaller*> m_vecGoEvents;
	
	IVistaClusterBarrier*		m_pDependentBarrier;
};

class VISTAINTERPROCCOMMAPI VistaInterProcClusterFollowerBarrier : public IVistaClusterBarrier
{
public:
	VistaInterProcClusterFollowerBarrier( const std::string& sInterProcName,
										const bool bVerbose = true );

	virtual ~VistaInterProcClusterFollowerBarrier();

	virtual bool GetIsValid() const;

	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual std::string GetBarrierType() const;

	virtual int GetSendBlockingThreshold() const;
	virtual bool SetSendBlockingThreshold( const int nNumBytes );

private:
	VistaProcessEventSignaller* m_pReadyEvent;
	int m_nTimeoutCount;
	VistaProcessEventReceiver* m_pGoEvent;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAINTERPROCCLUSTERBARRIER_H
