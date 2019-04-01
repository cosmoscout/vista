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


#include "VistaBasicProfiler.h"

#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaAspects/VistaDeSerializer.h>
#include <VistaAspects/VistaSerializer.h>

#include <map>
#include <set>
#include <list>
#include <iomanip>
#include <algorithm>
#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaBasicProfiler* pSingleton = NULL;


VistaBasicProfiler::ProfileScopeObject::ProfileScopeObject( const std::string& sName )
: m_sName( sName )
, m_pProfiler( VistaBasicProfiler::GetSingleton() )
{
	m_pProfiler->StartSection( sName );
}

VistaBasicProfiler::ProfileScopeObject::ProfileScopeObject( const std::string& sName,
														   VistaBasicProfiler* pProf )
: m_sName( sName )
, m_pProfiler( pProf )
{
	m_pProfiler->StartSection( sName );
}

VistaBasicProfiler::ProfileScopeObject::~ProfileScopeObject()
{
	m_pProfiler->StopSection( m_sName );
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaBasicProfiler::VistaBasicProfiler()
: m_pProfileRoot( new ProfileTreeNode( "ViSTA", NULL ) )
, m_iMaxNameLength( 5 )
{
	m_pProfileCurrent = m_pProfileRoot;
}

VistaBasicProfiler::VistaBasicProfiler( const std::string& sRootName )
: m_pProfileRoot( new ProfileTreeNode( sRootName, NULL ) )
, m_iMaxNameLength( (int)sRootName.size() )
{
	m_pProfileCurrent = m_pProfileRoot;
}

VistaBasicProfiler::~VistaBasicProfiler()
{
	delete m_pProfileRoot;
}

void VistaBasicProfiler::StartSection( const std::string& sName )
{
	if( m_pProfileRoot == NULL )
		return;
	m_pProfileCurrent = m_pProfileCurrent->Sub( sName );
	m_iMaxNameLength = std::max<int>( m_iMaxNameLength, (int)sName.size() );	
}

void VistaBasicProfiler::StopSection()
{
	if( m_pProfileRoot == NULL )
		return;
	m_pProfileCurrent = m_pProfileCurrent->Leave();
}

bool VistaBasicProfiler::StopSection( const std::string& sName )
{
	if( m_pProfileRoot == NULL ) // has just been reset
		return true;
	if( m_pProfileCurrent->m_sName == sName )
	{
		m_pProfileCurrent = m_pProfileCurrent->Leave();
		return true;
	}
	else
	{
		std::cerr << "[VistaBasicProfiler]: trying to end section ["
				<< sName << "], but section [" 
				<< m_pProfileCurrent->m_sName
				<< "] is active!" << std::endl;
		return false;
	}
}

VistaBasicProfiler* VistaBasicProfiler::GetSingleton()
{
	return pSingleton;
}

void VistaBasicProfiler::SetSingleton( VistaBasicProfiler* pProfiler )
{
	pSingleton = pProfiler;
}

void VistaBasicProfiler::PrintProfile( std::ostream& oStream, int iMaxDepth )
{	
	if( m_pProfileRoot == NULL )
		return;
	std::ios_base::fmtflags oFlags = oStream.flags();
	std::streamsize nCurrentPrecision = oStream.precision( 3 );		
	oStream.setf( std::ios_base::fixed );
	
	oStream << std::string( iMaxDepth + m_iMaxNameLength - 8, ' ' )
			<< "SectionName"
			<< " || last frame| fast avg  | slow avg  | max \n";
	oStream << std::string( iMaxDepth+ m_iMaxNameLength + 52, '-' ) << "\n";
	m_pProfileRoot->Print( oStream, 0, iMaxDepth, m_iMaxNameLength );

	oStream.precision( nCurrentPrecision );	
	oStream.flags( oFlags );
	oStream.flush();
}

bool VistaBasicProfiler::PrintMultipleProfiles( std::ostream& oStream, const std::vector< std::string >& vecNames,
												const std::vector< VistaBasicProfiler* >& vecProfilers, int iMaxDepth )
{
	if( vecNames.size() != vecProfilers.size() )
		return false;

	std::ios_base::fmtflags oFlags = oStream.flags();
	std::streamsize nCurrentPrecision = oStream.precision( 3 );		
	oStream.setf( std::ios_base::fixed );

	std::vector< ProfileTreeNode* > vecNodes( vecProfilers.size() );
	int nMaxNodeNameLength = 0;
	int nMaxSectionLenth = 0;
	for( std::size_t i = 0; i < vecProfilers.size(); ++i )
	{
		vecNodes[i] = vecProfilers[i]->GetRoot();
		nMaxSectionLenth = std::max( nMaxSectionLenth, vecProfilers[i]->m_iMaxNameLength );
		nMaxNodeNameLength = std::max( nMaxNodeNameLength, (int)vecNames[i].size() );
	}
	int nSectionNameColWidth = nMaxSectionLenth + iMaxDepth;
	int nMeasureColWidth = std::min( nMaxNodeNameLength, 10 );
	int nTypeColWidth = 11;
	int nRowWidth = nSectionNameColWidth + 2 + nTypeColWidth + 3 + ( nMeasureColWidth + 2 ) * (int)vecNodes.size();
	
	oStream << std::string( nRowWidth, '-' ) << "\n";
	oStream << std::setw( nSectionNameColWidth ) << "" << " |"
			<< std::setw( nTypeColWidth ) << "" << " ||";
	for( std::size_t i = 0; i < vecNames.size(); ++i )
		oStream << std::setw( nMeasureColWidth ) << vecNames[i] << " |";
	oStream << "\n";
	oStream << std::string( nRowWidth, '-' ) << "\n";
	DoPrintMultipleProfiles( oStream, vecNodes, 0, iMaxDepth, nSectionNameColWidth, nMeasureColWidth, nTypeColWidth );
	oStream << std::string( nRowWidth, '-' ) << "\n";
		
	oStream.precision( nCurrentPrecision );	
	oStream.flags( oFlags );
	oStream.flush();
	return true;
}

void VistaBasicProfiler::DoPrintMultipleProfiles( std::ostream& oStream,
												const std::vector< ProfileTreeNode* >& vecProfilers,
												int nDepth, int iMaxDepth, int nNameColWidth, int nEntryColWidth, int nTypeColWidth )
{
	int nRowWidth = nNameColWidth + 2 + nTypeColWidth + 3 + ( nEntryColWidth + 2 ) * (int)vecProfilers.size();
	oStream << std::string( nRowWidth, '-' ) << "\n";

	std::string sName;
	for( std::size_t i = 0; i < vecProfilers.size(); ++i )
	{
		if( vecProfilers[i] != NULL )
		{
			sName = vecProfilers[i]->m_sName;
			break;
		}
	}

	oStream << std::string( nDepth, '=' ) << sName << std::string( std::max( nNameColWidth - nDepth - (int)sName.size(), 0 ), ' ' )  << " |"
			<< std::setw( nTypeColWidth ) << "last frame" << " ||";
	for( std::size_t i = 0; i < vecProfilers.size(); ++i )
	{
		if( vecProfilers[i] == NULL )
			oStream << std::setw( nEntryColWidth ) << "-----"  << " |";
		else
			oStream << std::setw( nEntryColWidth ) << 1e3 * vecProfilers[i]->m_dLastFrame << " |";
	}
	oStream << "\n";

	oStream << std::setw( nNameColWidth ) << "" << " |"
			<< std::setw( nTypeColWidth ) << "fast avg." << " ||";
	for( std::size_t i = 0; i < vecProfilers.size(); ++i )
	{
		if( vecProfilers[i] == NULL )
			oStream << std::setw( nEntryColWidth ) << "-----"  << " |";
		else
			oStream << std::setw( nEntryColWidth ) << 1e3 * vecProfilers[i]->m_dFastAvg << " |";
	}
	oStream << "\n";

	oStream << std::setw( nNameColWidth ) << "" << " |"
			<< std::setw( nTypeColWidth ) << "slow avg." << " ||";
	for( std::size_t i = 0; i < vecProfilers.size(); ++i )
	{
		if( vecProfilers[i] == NULL )
			oStream << std::setw( nEntryColWidth ) << "-----"  << " |";
		else
			oStream << std::setw( nEntryColWidth ) << 1e3 * vecProfilers[i]->m_dSlowAvg << " |";
	}
	oStream << "\n";

	oStream << std::setw( nNameColWidth ) << "" << " |"
			<< std::setw( nTypeColWidth ) << "max" << " ||";
	for( std::size_t i = 0; i < vecProfilers.size(); ++i )
	{
		if( vecProfilers[i] == NULL )
			oStream << std::setw( nEntryColWidth ) << "-----"  << " |";
		else
			oStream << std::setw( nEntryColWidth ) << 1e3 * vecProfilers[i]->m_dMax << " |";
	}
	oStream << "\n";

	if( nDepth + 1 == iMaxDepth )
		return;

	// sort entries - they may appear multiple times...
	std::map< std::string, int > mapSectionOccurences;
	for( std::size_t i = 0; i < vecProfilers.size(); ++i )
	{
		if( vecProfilers[i] == NULL )
			continue;
		for( std::list< ProfileTreeNode* >::const_iterator itChild = vecProfilers[i]->m_liChildren.begin();
				itChild != vecProfilers[i]->m_liChildren.end(); ++itChild )
		{
			++mapSectionOccurences[ (*itChild)->m_sName ];
		}
	}
	std::vector< std::list< ProfileTreeNode* >::const_iterator > vecCurrentChildren( vecProfilers.size() );
	for( std::size_t i = 0; i < vecProfilers.size(); ++i )
	{
		if( vecProfilers[i] != NULL )
			vecCurrentChildren[i] = vecProfilers[i]->m_liChildren.begin();
	}
	std::set< std::string > setHandledSections;
	for( ;; )
	{
		bool bHasNoMoreEntries = true;
		std::string sChosenSection;
		for( std::size_t i = 0; i < vecProfilers.size(); ++i )
		{
			if( vecProfilers[i] == NULL || vecCurrentChildren[i] == vecProfilers[i]->m_liChildren.end() )
				continue;
			bHasNoMoreEntries = false;
			std::string sSection = (*vecCurrentChildren[i])->m_sName;
			if( setHandledSections.find( sSection ) != setHandledSections.end() )
			{
				++vecCurrentChildren[i];
				continue;
			}
			if( sChosenSection == sSection )
				continue;
			if( sChosenSection.empty() == false && mapSectionOccurences[ sChosenSection ] <= mapSectionOccurences[ sSection ] )
				continue;
			sChosenSection = sSection;
		}

		if( bHasNoMoreEntries )
			break;

		if( sChosenSection.empty() )
			continue;

		std::vector< ProfileTreeNode* > vecChildren( vecProfilers.size(), NULL );
		for( std::size_t i = 0; i < vecProfilers.size(); ++i )
		{
			if( vecProfilers[i] == NULL || vecCurrentChildren[i] == vecProfilers[i]->m_liChildren.end() )
				continue;
			if( (*vecCurrentChildren[i])->m_sName == sChosenSection )
			{
				vecChildren[i] = (*vecCurrentChildren[i]);
				++vecCurrentChildren[i];
			}
			else
			{
				vecChildren[i] = NULL;
			}
		}
		DoPrintMultipleProfiles( oStream, vecChildren, nDepth + 1, iMaxDepth, nNameColWidth, nEntryColWidth, nTypeColWidth );
	}
}

void VistaBasicProfiler::NewFrame()
{
	if( m_pProfileRoot == NULL )
	{		
		m_pProfileRoot = new ProfileTreeNode( "ViSTA", NULL );
		m_pProfileCurrent = m_pProfileRoot;
		return;
	}
	if( m_pProfileRoot->m_dEntryTime != 0.0 ) // not the first frame
	{
		m_pProfileRoot->Leave();
		m_pProfileRoot->NewFrame();
	}
	m_pProfileRoot->Enter();
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::GetRoot() const
{
	return m_pProfileRoot;
}

int VistaBasicProfiler::Serialize( IVistaSerializer& oSerializer ) const
{
	VistaType::sint32 nMaxNameLength = (VistaType::sint32)m_iMaxNameLength;
	int nWriteCount = oSerializer.WriteInt32( nMaxNameLength );
	nWriteCount += m_pProfileRoot->SerializeNode( oSerializer );
	return nWriteCount;
}

int VistaBasicProfiler::DeSerialize( IVistaDeSerializer& oDeSerializer )
{	
	delete m_pProfileRoot;
	m_pProfileRoot = new ProfileTreeNode( "", NULL );
	VistaType::sint32 nMaxNameLength = 0;
	int nReadCount = oDeSerializer.ReadInt32( nMaxNameLength );
	m_iMaxNameLength = (int)nMaxNameLength;
	nReadCount += m_pProfileRoot->DeSerializeNode( oDeSerializer );
	return nReadCount;
}

std::string VistaBasicProfiler::GetSignature() const
{
	return "VistaBasicProfiler";
}


void VistaBasicProfiler::Reset()
{
	delete m_pProfileRoot;
	m_pProfileRoot = NULL;
}

/*============================================================================*/
/* PROFILETREENODE                                                            */
/*============================================================================*/


VistaBasicProfiler::ProfileTreeNode::ProfileTreeNode( const std::string& sName, ProfileTreeNode* pParent ) : m_pParent( pParent )
, m_sName( sName )
, m_dCurrentFrame( 0 )
, m_dLastFrame( 0 )
, m_dSlowAvg( 0 )
, m_dFastAvg( 0 )
, m_dEntryTime( 0 )
, m_dMax( 0 )
{

}

VistaBasicProfiler::ProfileTreeNode::~ProfileTreeNode()
{
	for( std::map<std::string, ProfileTreeNode*>::iterator itChild = m_mapChildren.begin(); itChild != m_mapChildren.end(); ++itChild )
		delete (*itChild).second;
}

void VistaBasicProfiler::ProfileTreeNode::Enter()
{
	m_dEntryTime = VistaTimeUtils::GetStandardTimer().GetMicroTime();
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::ProfileTreeNode::Sub( const std::string& sName )
{
	ProfileTreeNode* pNode = NULL;
	std::map<std::string, ProfileTreeNode*>::iterator it = m_mapChildren.find( sName );
	if( it == m_mapChildren.end() )
	{
		pNode = new ProfileTreeNode( sName, this );
		m_mapChildren[sName] = pNode;
		m_liChildren.push_back( pNode );
	}
	else
		pNode = (*it).second;

	pNode->Enter();
	return pNode;
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::ProfileTreeNode::Leave()
{
	m_dCurrentFrame += VistaTimeUtils::GetStandardTimer().GetMicroTime() - m_dEntryTime;
	return m_pParent;
}

void VistaBasicProfiler::ProfileTreeNode::Print( std::ostream& oStream, int iDepth, int iMaxDepth, int iMaxNameLength )
{
	if( m_mapChildren.empty() )
	{
		oStream << std::string( iDepth, ' ' ) << "---"
			<< std::left << std::setw( iMaxNameLength + iMaxDepth - iDepth ) 
			<< m_sName << std::right
			<< " || " << std::setw(7) << 1000.0*m_dLastFrame << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dFastAvg << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dSlowAvg << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dMax << "ms\n";
	}
	else
	{
		oStream << std::string( iDepth, ' ' ) << "|--"
			<< std::left << std::setw( iMaxNameLength + iMaxDepth - iDepth ) 
			<< m_sName << std::right
			<< " || " << std::setw(7) << 1000.0*m_dLastFrame << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dFastAvg << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dSlowAvg << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dMax << "ms\n";

		if( iDepth > iMaxDepth )
			return;

		++iDepth;
		for( std::list<ProfileTreeNode*>::iterator itChild = m_liChildren.begin();
			itChild != m_liChildren.end(); ++itChild )
			(*itChild)->Print( oStream, iDepth, iMaxDepth, iMaxNameLength );
	}
}

void VistaBasicProfiler::ProfileTreeNode::NewFrame()
{
	m_dLastFrame = m_dCurrentFrame;
	m_dMax = std::max<VistaType::microtime>( m_dMax, m_dLastFrame );
	//@todo: inaccurate at the beginning
	if( m_dSlowAvg > 0.0 )
	{
		m_dSlowAvg = 0.005 * m_dLastFrame + 0.995 * m_dSlowAvg;
		m_dFastAvg = 0.025 * m_dLastFrame + 0.975 * m_dFastAvg;
	}
	else
	{
		m_dSlowAvg = m_dLastFrame;
		m_dFastAvg = m_dLastFrame;
	}
	for( std::map<std::string, ProfileTreeNode*>::iterator itChild = m_mapChildren.begin();
		itChild != m_mapChildren.end(); ++itChild )
		(*itChild).second->NewFrame();

	m_dCurrentFrame = 0;
}

int VistaBasicProfiler::ProfileTreeNode::SerializeNode( IVistaSerializer& oSerializer )
{
	int nWriteSize = 0;
	nWriteSize += oSerializer.WriteDouble( m_dEntryTime );
	nWriteSize += oSerializer.WriteEncodedString( m_sName );
	nWriteSize += oSerializer.WriteDouble( m_dLastFrame );
	nWriteSize += oSerializer.WriteDouble( m_dFastAvg );
	nWriteSize += oSerializer.WriteDouble( m_dSlowAvg );
	nWriteSize += oSerializer.WriteDouble( m_dMax );
	nWriteSize += oSerializer.WriteDouble( m_dCurrentFrame );

	VistaType::sint32 nNumChildren = (VistaType::sint32)m_liChildren.size();
	nWriteSize += oSerializer.WriteInt32( nNumChildren );
	for( std::list< ProfileTreeNode* >::iterator itNode = m_liChildren.begin(); itNode != m_liChildren.end(); ++itNode )
	{
		nWriteSize += (*itNode)->SerializeNode( oSerializer );
	}

	return nWriteSize;
}

int VistaBasicProfiler::ProfileTreeNode::DeSerializeNode( IVistaDeSerializer& oDeSerializer )
{
	assert( m_liChildren.empty() );
	assert( m_mapChildren.empty() );
	assert( m_sName.empty() );

	int nReadSize = 0;
	nReadSize += oDeSerializer.ReadDouble( m_dEntryTime );
	nReadSize += oDeSerializer.ReadEncodedString( m_sName );
	nReadSize += oDeSerializer.ReadDouble( m_dLastFrame );
	nReadSize += oDeSerializer.ReadDouble( m_dFastAvg );
	nReadSize += oDeSerializer.ReadDouble( m_dSlowAvg );
	nReadSize += oDeSerializer.ReadDouble( m_dMax );
	nReadSize += oDeSerializer.ReadDouble( m_dCurrentFrame );

	VistaType::sint32 nNumChildren = 0;
	oDeSerializer.ReadInt32( nNumChildren );
	for( int i = 0; i < nNumChildren; ++i )
	{
		ProfileTreeNode* pNode = new ProfileTreeNode( "", this );
		nReadSize += pNode->DeSerializeNode( oDeSerializer );
		m_liChildren.push_back( pNode );
		m_mapChildren[ pNode->m_sName ] = pNode;
	}

	return nReadSize;

}

int VistaBasicProfiler::ProfileTreeNode::GetNumChildren() const
{
	return (int)m_liChildren.size();
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::ProfileTreeNode::GetChild( int iIndex ) const
{
	int iCurrentIndex = 0;
	for( std::list<ProfileTreeNode*>::const_iterator itChild = m_liChildren.begin();
				itChild != m_liChildren.end(); ++itChild, ++iCurrentIndex )
	{
		if( iCurrentIndex == iIndex ) 
			return (*itChild);
	}
	return NULL;
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::ProfileTreeNode::GetChild( 
													const std::string& sName,
													bool bSearchWholeSubtree ) const
{
	std::map<std::string, ProfileTreeNode*>::const_iterator itChild = m_mapChildren.find( sName );
	if( itChild != m_mapChildren.end() )
		return (*itChild).second;

	if( bSearchWholeSubtree )
	{	
		for( std::list<ProfileTreeNode*>::const_iterator itTreeChild = m_liChildren.begin();
				itTreeChild != m_liChildren.end(); ++itTreeChild )
		{
			ProfileTreeNode* pSubtreeResult = (*itTreeChild)->GetChild( sName, true );
			if( pSubtreeResult != NULL ) 
				return pSubtreeResult;
		}
	}

	return NULL;
}

VistaType::microtime VistaBasicProfiler::ProfileTreeNode::GetLastFrameTime() const
{
	return m_dLastFrame;
}

VistaType::microtime VistaBasicProfiler::ProfileTreeNode::GetFastAverage() const
{
	return m_dFastAvg;
}

VistaType::microtime VistaBasicProfiler::ProfileTreeNode::GetSlowAverage() const
{
	return m_dSlowAvg;
}

bool VistaBasicProfiler::ProfileTreeNode::operator<( const VistaBasicProfiler::ProfileTreeNode& oCompare )
{
	return m_dEntryTime < oCompare.m_dEntryTime;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

