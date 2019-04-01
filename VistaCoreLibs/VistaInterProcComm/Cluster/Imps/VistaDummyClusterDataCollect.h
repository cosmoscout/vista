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


#ifndef _VISTADUMMYCLUSTERCOLLECT_H
#define _VISTADUMMYCLUSTERCOLLECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterDataCollect.h>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Dummy implementation that does nothing - useful for example in Standalone
 */
class VISTAINTERPROCCOMMAPI VistaDummyClusterDataCollect : public IVistaClusterDataCollect
{
public:
	VistaDummyClusterDataCollect();
	virtual ~VistaDummyClusterDataCollect();

	virtual bool CollectTime( const VistaType::systemtime nOwnTime,
							std::vector<VistaType::systemtime>& vecCollected );
	virtual bool CollectData( const VistaPropertyList& oList,
							std::vector<VistaPropertyList>& vecCollected );
	virtual bool CollectData( const VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							std::vector<std::vector<VistaType::byte> >& vecCollected );

	virtual bool GetIsValid() const;

	virtual std::string GetDataCollectType() const;

	virtual int GetSendBlockingThreshold() const;
	virtual bool SetSendBlockingThreshold( const int nNumBytes );
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADUMMYCLUSTERCOLLECT_H
