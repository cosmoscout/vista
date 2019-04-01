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


#ifndef _VISTABROADCASTCLUSTERBARRIER_H
#define _VISTABROADCASTCLUSTERBARRIER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaClusterBarrierIPBase.h>

#include <vector>
#include <string>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaUDPSocket;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaBroadcastClusterLeaderBarrier : public VistaClusterLeaderBarrierIPBase
{
public:
	VistaBroadcastClusterLeaderBarrier( const std::string& sBroadcastIP,
									const int nBroadcastPort,
									const bool bVerbose = true );
	VistaBroadcastClusterLeaderBarrier( const std::string& sBroadcastIP,
									const std::vector<int>& nBroadcastPorts,
									const bool bVerbose = true );
	explicit VistaBroadcastClusterLeaderBarrier( VistaUDPSocket* pBroadcastSocket,
									const bool bManageDeletion = true,
									const bool bVerbose = true );
	explicit VistaBroadcastClusterLeaderBarrier( const std::vector<VistaUDPSocket*>& vecSockets,
									const bool bManageDeletion = true,
									const bool bVerbose = true );

	virtual ~VistaBroadcastClusterLeaderBarrier();

	bool AddBroadcast( const std::string& sBroadcastIP,
						const int nBroadcastPort );
	bool AddBroadcast( VistaUDPSocket* pBroadcastSocket,
						const bool bManageDeletion = true );

	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual bool GetIsValid() const;

	virtual std::string GetBarrierType() const;

	int GetNumberOfBroadcasts() const;

private:
	std::vector<VistaUDPSocket*>	m_vecBroadcastSockets;
	std::vector<bool>				m_vecDeleteBroadcastSocket;
};

class VISTAINTERPROCCOMMAPI VistaBroadcastClusterFollowerBarrier : public VistaClusterFollowerBarrierIPBase
{
public:
	VistaBroadcastClusterFollowerBarrier( const std::string& sBroadcastIP,
									const int nBroadcastPort,
									VistaConnectionIP* pLeaderConnection,
									const bool bManageConnection = true,
									const bool bVerbose = true );
	VistaBroadcastClusterFollowerBarrier( VistaUDPSocket* pBroadcastSocket,
									VistaConnectionIP* pLeaderConnection,
									const bool bManageSocket = true,
									const bool bManageConnection = true,
									const bool bVerbose = true );

	virtual ~VistaBroadcastClusterFollowerBarrier();

	virtual bool GetIsValid() const;

	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual std::string GetBarrierType() const;

private:
	VistaUDPSocket*		m_pBroadcastSocket;
	bool				m_bManageSocket;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTABROADCASTCLUSTERBARRIER_H
