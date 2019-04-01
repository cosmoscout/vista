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


#ifndef _VISTACLUSTERSYNCENTITY_H
#define _VISTACLUSTERSYNCENTITY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaObserveable.h>

#include <string>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Common base class for all objects that allow some kind of sync'ing in
 * cluster mode. Current sub-interfaces are VistaClusterBarrier (wait until
 * all nodes are ready, then resume all at once), VistaClusterDataSync (ensure
 * that all nodes have the same information as the master), and
 * VistaClusterDataCollect( gathers data from all nodes and makes it available
 * to the ClusterLeader)
 * @see VistaClusterBarrier
 * @see VistaClusterDataSync
 * @see VistaClusterDataCollect
 */
class VISTAINTERPROCCOMMAPI IVistaClusterSyncEntity : public IVistaObserveable
{
public:
	enum
	{
		MSG_FOLLOWER_ADDED = IVistaObserveable::MSG_LAST,
		MSG_FOLLOWER_LOST,
		MSG_WAIT_FAILED,
		MSG_FATAL_ERROR,
		MSG_LAST
	};
public:
	virtual ~IVistaClusterSyncEntity();

	/**
	 * Get/Set the verbosity flag to control if warnings and errors are
	 * to be printed. Useful if one wants to perform his own output
	 * in response to observeable notifies
	 */
	bool GetIsVerbose() const;
	void SetIsVerbose( const bool bSet );

	/**
	 * returns true if the SyncEntity is a leader, which usually means
	 * that it is the instance on the cluster-leader node
	 */
	bool GetIsLeader() const;

	/**
	 * Get Number of total/active/dead followers, names for followers,
	 * or deactivates the specific follower.
	 * Followers are usually only specified on Leader instances, and
	 * even then the SyncEntity may nor be aware of/have knowledge about
	 * followers.
	 */
	virtual int GetNumberOfFollowers() const;
	virtual int GetNumberOfActiveFollowers() const;
	virtual int GetNumberOfDeadFollowers() const;
	virtual std::string GetFollowerNameForId( const int nID ) const;
	virtual int GetFollowerIdForName( const std::string& sName ) const;
	virtual bool GetFollowerIsAlive( const int nID ) const;
	virtual bool DeactivateFollower( const std::string& sName );
	virtual bool DeactivateFollower( const int nID );

	/**
	 * allows to query the follower that was added or lost last,
	 * useful when receiving a corresponding observeable notify
	 * only use during the corresponding notify.
	 */
	virtual int GetLastChangedFollower();

	virtual bool GetIsValid() const = 0;

	/**
	 * BlockingThreshold determines from which size on the sent data should be
	 * transmitted using blocking mode. This can be used to send small packages
	 * in non-blocking mode, while using blocking send for larger packages that
	 * might exceed the (TCP) stack size.
	 * Not all implementations utilize this setting, indicated by returning false
	 * 0 means always block, negative values mean never block
	 * @return true if the value was accepted, false if it is not used/supported
	 */
	virtual bool SetSendBlockingThreshold( const int nNumBytes ) = 0;
	/**
	 * returns the blocking threshold
	 * @return number of bytes before activating blocking send
	 *        where negative values indicate 'never block' and 0 'always block'
	 */
	virtual int GetSendBlockingThreshold() const = 0;

protected:
	IVistaClusterSyncEntity( const bool bVerbose, const bool bIsLeader );
	
private:
	// prevent copying
	IVistaClusterSyncEntity( const IVistaClusterSyncEntity& );
	IVistaClusterSyncEntity& operator= ( const IVistaClusterSyncEntity& );

protected:
	bool m_bVerbose;
	bool m_bIsLeader;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERSYNCENTITY_H
