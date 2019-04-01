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


#include "VistaClusterSyncEntity.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaClusterSyncEntity::IVistaClusterSyncEntity( const bool bVerbose, const bool bIsLeader )
: IVistaObserveable()
, m_bVerbose( bVerbose )
, m_bIsLeader( bIsLeader )
{
}

IVistaClusterSyncEntity::IVistaClusterSyncEntity( const IVistaClusterSyncEntity& )
: IVistaObserveable()
{
}	

IVistaClusterSyncEntity::~IVistaClusterSyncEntity()
{
}

bool IVistaClusterSyncEntity::GetIsVerbose() const
{
	return m_bVerbose;
}

void IVistaClusterSyncEntity::SetIsVerbose( const bool bSet )
{
	m_bVerbose = bSet;
}


bool IVistaClusterSyncEntity::GetIsLeader() const
{
	return m_bIsLeader;
}

int IVistaClusterSyncEntity::GetNumberOfFollowers() const
{
	return 0;
}

int IVistaClusterSyncEntity::GetNumberOfActiveFollowers() const
{
	return 0;
}

int IVistaClusterSyncEntity::GetNumberOfDeadFollowers() const
{
	return 0;
}

std::string IVistaClusterSyncEntity::GetFollowerNameForId( const int nID ) const
{
	return "";
}

int IVistaClusterSyncEntity::GetFollowerIdForName( const std::string& sName ) const
{
	return -1;
}

bool IVistaClusterSyncEntity::GetFollowerIsAlive( const int nID ) const
{
	return false;
}

int IVistaClusterSyncEntity::GetLastChangedFollower()
{
	return -1;
}

bool IVistaClusterSyncEntity::DeactivateFollower( const std::string& sName )
{
	return false;
}

bool IVistaClusterSyncEntity::DeactivateFollower( const int nID )
{
	return false;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

