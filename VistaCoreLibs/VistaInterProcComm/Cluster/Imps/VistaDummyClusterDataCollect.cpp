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


#include "VistaDummyClusterDataCollect.h"

#include <VistaBase/VistaTimeUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/


VistaDummyClusterDataCollect::VistaDummyClusterDataCollect()
: IVistaClusterDataCollect( false, true )
{
}

VistaDummyClusterDataCollect::~VistaDummyClusterDataCollect()
{
}

bool VistaDummyClusterDataCollect::GetIsValid() const
{
	return true;
}

std::string VistaDummyClusterDataCollect::GetDataCollectType() const
{
	return "DummyClusterDataCollet";
}

bool VistaDummyClusterDataCollect::CollectTime( const VistaType::systemtime nOwnTime,
							std::vector<VistaType::systemtime>& vecCollected )
{
	vecCollected.push_back( nOwnTime );
	return true;
}
bool VistaDummyClusterDataCollect::CollectData( const VistaPropertyList& oList,
						std::vector<VistaPropertyList>& vecCollected )
{
	vecCollected.push_back( oList );
	return true;
}
bool VistaDummyClusterDataCollect::CollectData( const VistaType::byte* pDataBuffer, 
						const int iBufferSize,
						std::vector<std::vector<VistaType::byte> >& vecCollected )
{
	vecCollected.push_back( std::vector<VistaType::byte>() );
	vecCollected[0].resize( iBufferSize );
	memcpy( &vecCollected[0][0], pDataBuffer, iBufferSize );
	return true;
}

int VistaDummyClusterDataCollect::GetSendBlockingThreshold() const
{
	return -1;
}

bool VistaDummyClusterDataCollect::SetSendBlockingThreshold( const int nNumBytes )
{
	return false;
}
