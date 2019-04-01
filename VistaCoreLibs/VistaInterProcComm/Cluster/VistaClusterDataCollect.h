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


#ifndef _VISTANETWORKCOLLECT_H
#define _VISTANETWORKCOLLECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/Cluster/VistaClusterSyncEntity.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaPropertyList.h>

#include <string>
#include <vector>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaPropertyList;
class IVistaSerializable;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaClusterDataCollect is an abstract base class for gathering data from
 * several nodes in the cluster setup, and returns it in the vector.
 * However, the colelcted data is not distributed to the followers, so that only
 * the leader has a properly filled vector, followers will have no data at all.
 * If followers require the data too, use a DataSync to distribute it.
 */
class VISTAINTERPROCCOMMAPI IVistaClusterDataCollect : public IVistaClusterSyncEntity
{
public:
	virtual ~IVistaClusterDataCollect();
	
	virtual bool CollectTime( const VistaType::systemtime nOwnTime,
							std::vector<VistaType::systemtime>& vecCollected ) = 0;
	virtual bool CollectData( const VistaPropertyList& oList,
							std::vector<VistaPropertyList>& vecCollected ) = 0;
	virtual bool CollectData( const VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							std::vector<std::vector<VistaType::byte> >& vecCollected ) = 0;

	virtual std::string GetDataCollectType() const = 0;

protected:
	IVistaClusterDataCollect( const bool bVerbose, const bool bIsLeader );
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANETWORKCOLLECT_H
