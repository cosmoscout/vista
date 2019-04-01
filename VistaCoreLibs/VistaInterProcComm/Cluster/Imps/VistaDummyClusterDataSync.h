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


#ifndef _VISTADUMMYCLUSTERSYNC_H
#define _VISTADUMMYCLUSTERSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterDataSync.h>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Dummy implementation that does nothing - useful for example in Standalone
 */
class VISTAINTERPROCCOMMAPI VistaDummyClusterDataSync : public IVistaClusterDataSync
{
public:
	VistaDummyClusterDataSync();
	virtual ~VistaDummyClusterDataSync();

	virtual VistaType::systemtime GetSyncTime();

	virtual bool SyncTime( VistaType::systemtime& nTime );
	virtual bool SyncData( std::string& sData );
	virtual bool SyncData( VistaPropertyList& oList );
	virtual bool SyncData( IVistaSerializable& oSerializable );
	virtual bool SyncData( VistaType::byte* pData, 
							const int iDataSize );
	virtual bool SyncData( VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							int& iDataSize );
	virtual bool SyncData( std::vector<VistaType::byte>& vecData );

	virtual bool GetIsValid() const;

	virtual std::string GetDataSyncType() const;

	virtual bool SetSendBlockingThreshold( const int nNumBytes );
	virtual int GetSendBlockingThreshold() const;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADUMMYCLUSTERSYNC_H
