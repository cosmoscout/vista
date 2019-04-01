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


#ifndef _VISTANETWORKSYNC_H
#define _VISTANETWORKSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/Cluster/VistaClusterSyncEntity.h>

#include <VistaBase/VistaBaseTypes.h>

#include <string>
#include <vector>
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
 * VistaNetworkSync is an abstract base class for synchronizing several computers
 * in a network (e.g. a bunch of cluster nodes). Basically one of these nodes
 * takes on the role of a master whereas the others are mere "slaves".
 */
class VISTAINTERPROCCOMMAPI IVistaClusterDataSync : public IVistaClusterSyncEntity
{
public:
	virtual ~IVistaClusterDataSync();

	/**
	 * Call this routine to get a synchronized time that corresponds
	 * to the time instance on the leader node
	 */
	VistaType::systemtime GetSyncTime();

	/**
	 * Call these routines to sync data on all clients
	 * If the call returns false, Syncing failed and the data remains untouched
	 * Otherwise, the data is the same on all nodes
	 * Usually wraps the data into an internal package to ensure correct
	 * sizes, check types etc.
	 * Usually, Sync-Calls have to be matched with the exact same call (incl parameters)
	 * On all machines. An exception is SyncData with variable size buffers, where
	 * vectors and Buffers can be used interchangeably
	 */
	virtual bool SyncTime( VistaType::systemtime& nTime ) = 0;
	virtual bool SyncData( std::string& sData ) = 0;
	virtual bool SyncData( VistaPropertyList& oList ) = 0;
	virtual bool SyncData( IVistaSerializable& oSerializable ) = 0;
	virtual bool SyncData( VistaType::byte* pFixedSizeData, 
							const int iDataSize ) = 0;
	virtual bool SyncData( VistaType::byte* pVariableSizeData, 
							const int iBufferSize,
							int& iDataSize ) = 0;
	virtual bool SyncData( std::vector<VistaType::byte>& vecData ) = 0;



	/**
	 * returns the type of the DataSync as string (usually the classname)
	 */
	virtual std::string GetDataSyncType() const = 0;

protected:
	IVistaClusterDataSync( const bool bVerbose, const bool bIsLeader );
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANETWORKSYNC_H
