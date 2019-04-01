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



#ifndef _VISTACLUSTERSYNCEDTIMERIMP_H
#define _VISTACLUSTERSYNCEDTIMERIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaTimerImp.h>
#include <VistaBase/VistaDefaultTimerImp.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaClusterMode;
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Implementation of the TimerImp that provides mostly synchronous local clocks
 * in cluster mode. Mostly for testing reasons, pretty inefficient, and not
 * fully reliable, but okay for testing synchronity etc. in ClusterMode
 * Directly after calling sync, the clocks should be in sync with a delta of
 * less than a microsecond (if Network performance is good, at least).
 * However, due to local clock drifting, a re-syncing has to be performed perio-
 * dically to maintain precision.
 * Usage: Create instance of TmierImp, and either use directly to initialize
 *       your timers, or set as VistaTimerImp-singleton to create all timers
 *       as netsync by default (before VistaSystem::Init), then initialize
 *       the TimerImp either using custom connections or the ClusterMode.
 *       Finally, perform at least on call to Sync()
 * Note: timestamps will not be necessarily monotonous when calling Sync()
 */
class VISTAKERNELAPI VistaClusterSyncedTimerImp : public VistaDefaultTimerImp
{
public:
	VistaClusterSyncedTimerImp();	
	virtual ~VistaClusterSyncedTimerImp();

	bool Init( VistaClusterMode* pMode );
	bool InitAsLeader( VistaConnectionIP* pConn, bool bManageConnDeletion = true );
	bool InitAsFollower( std::vector<VistaConnectionIP*> vecConns, 
							bool bManageConnDeletion = true );

	bool Sync( int nIterations = 1000 );

private:
	bool m_bIsLeader;
	bool m_bOwnConnections;
	std::vector<VistaConnectionIP*> m_vecConnections;
};


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif /* _VISTACLUSTERSYNCEDTIMERIMP_H */
