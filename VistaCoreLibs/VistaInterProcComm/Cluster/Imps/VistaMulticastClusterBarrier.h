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


#ifndef _VISTAMULTICASTCLUSTERBARRIER_H
#define _VISTAMULTICASTCLUSTERBARRIER_H

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
class VistaMcastSocket;
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaMulticastClusterLeaderBarrier : public VistaClusterLeaderBarrierIPBase
{
public:
	VistaMulticastClusterLeaderBarrier( const std::string& sOwnIP,
									const std::string& sMulticastIP,
									const int nMulticastPort,
									const bool bVerbose = true );
	explicit VistaMulticastClusterLeaderBarrier( VistaMcastSocket* pMulticastSocket,
									const bool bManageDeletion = true,
									const bool bVerbose = true );

	virtual ~VistaMulticastClusterLeaderBarrier();	


	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual bool GetIsValid() const;

	virtual std::string GetBarrierType() const;


private:
	VistaMcastSocket*		m_pMulticastSocket;
	bool					m_bDeleteMulticastSocket;
};

class VISTAINTERPROCCOMMAPI VistaMulticastClusterFollowerBarrier : public VistaClusterFollowerBarrierIPBase
{
public:
	VistaMulticastClusterFollowerBarrier( const std::string& sMulticastIP,
									const int nMulticastPort,
									VistaConnectionIP* pLeaderConnection,
									const bool bManageConnection = true,
									const bool bVerbose = true );
	VistaMulticastClusterFollowerBarrier( VistaMcastSocket* pMulticastSocket,
									VistaConnectionIP* pLeaderConnection,
									const bool bManageSocket = true,
									const bool bManageConnection = true,
									const bool bVerbose = true );

	virtual ~VistaMulticastClusterFollowerBarrier();

	virtual bool GetIsValid() const;

	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual std::string GetBarrierType() const;

private:
	VistaType::sint32	m_nBarrierCount;
	VistaMcastSocket*	m_pMulticastSocket;
	VistaConnectionIP*	m_pLeaderConn;
	bool				m_bManageSocket;
	bool				m_bSwap;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMULTICASTCLUSTERBARRIER_H
