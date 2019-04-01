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


#ifndef _VISTATCPIPCLUSTERBARRIER_H
#define _VISTATCPIPCLUSTERBARRIER_H

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
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaTCPIPClusterLeaderBarrier : public VistaClusterLeaderBarrierIPBase
{
public:
	VistaTCPIPClusterLeaderBarrier( const bool bVerbose = true );

	virtual ~VistaTCPIPClusterLeaderBarrier();
	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual bool GetIsValid() const;

	virtual std::string GetBarrierType() const;
};

class VISTAINTERPROCCOMMAPI VistaTCPIPClusterFollowerBarrier : public VistaClusterFollowerBarrierIPBase
{
public:
	VistaTCPIPClusterFollowerBarrier( VistaConnectionIP* pLeaderConnection,
									const bool bManageDeletion = false,
									const bool bVerbose = true );

	virtual ~VistaTCPIPClusterFollowerBarrier();

	virtual bool GetIsValid() const;

	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual std::string GetBarrierType() const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATCPIPCLUSTERBARRIER_H
