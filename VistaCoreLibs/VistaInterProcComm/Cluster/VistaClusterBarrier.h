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


#ifndef _VISTACLUSTERBARRIER_H
#define _VISTACLUSTERBARRIER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterSyncEntity.h>

#include <VistaBase/VistaBaseTypes.h>

#include <string>
#include <iostream>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaPropertyList;
class IVistaSerializable;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaNetworkSync is an abstract base class for a barrier wait among
 * several nodes in a cluster.
 * It is observable purely for 
 */
class VISTAINTERPROCCOMMAPI IVistaClusterBarrier : public IVistaClusterSyncEntity
{
public:
	virtual ~IVistaClusterBarrier();

	/**
	 * Call this routine to wait until all nodes reach the sync
	 * The network leader as well as all nodes will wait until all
	 * nodes reach the sync, so that all return at about the
	 * same time
	 * @return true if sync went on without problems
	 *         false if something went wrong (e.g. timeout was hit)
	 */
	virtual bool BarrierWait( int iTimeOut = 0 ) = 0;

	virtual std::string GetBarrierType() const = 0;

protected:
	IVistaClusterBarrier( const bool bVerbose, const bool bLeader );
	
private:
	// prevent copying
	IVistaClusterBarrier( const IVistaClusterBarrier& );
	IVistaClusterBarrier& operator= ( const IVistaClusterBarrier& );

protected:
	bool m_bVerbose;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERBARRIER_H
