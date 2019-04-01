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


#ifndef _VISTABASICPROFILER_H
#define _VISTABASICPROFILER_H

/*============================================================================*/
/* MACROS AND DEFINES (part one)                                              */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"

#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaUncopyable.h>
#include <VistaAspects/VistaSerializable.h>

#include <map>
#include <list>
#include <string>
#include <iostream>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTATOOLSAPI VistaBasicProfiler : public IVistaSerializable
{
	VISTA_UNCOPYABLE( VistaBasicProfiler );
public:
	class VISTATOOLSAPI ProfileTreeNode
	{
		VISTA_UNCOPYABLE( ProfileTreeNode )
	public:
		void Print( std::ostream& oStream, int iDepth, int iMaxDepth, int iMaxNameLength );

		VistaType::microtime GetLastFrameTime() const;
		VistaType::microtime GetFastAverage() const;
		VistaType::microtime GetSlowAverage() const;

		int GetNumChildren() const;
		ProfileTreeNode* GetChild( int iIndex ) const;
		ProfileTreeNode* GetChild( const std::string& sName, bool bSearchWholeSubtree = false ) const;
		
	protected:
		friend class VistaBasicProfiler;

		ProfileTreeNode( const std::string& sName, ProfileTreeNode* pParent );
		virtual ~ProfileTreeNode();

		void Enter();
		ProfileTreeNode* Sub( const std::string& sName );
		ProfileTreeNode* Leave();		

		void NewFrame();
				
		int SerializeNode( IVistaSerializer& oSerializer );
		int DeSerializeNode( IVistaDeSerializer& oDeSerializer );

		bool operator<( const VistaBasicProfiler::ProfileTreeNode& oCompare );

	protected:
		VistaType::microtime					m_dEntryTime;
		std::string								m_sName;
		ProfileTreeNode*						m_pParent;
		std::map<std::string, ProfileTreeNode*>	m_mapChildren;	
		std::list<ProfileTreeNode*>				m_liChildren;
		VistaType::microtime					m_dCurrentFrame;
		VistaType::microtime					m_dLastFrame;
		VistaType::microtime					m_dFastAvg;
		VistaType::microtime					m_dSlowAvg;
		VistaType::microtime					m_dMax;
	};

public:
	VistaBasicProfiler();
	VistaBasicProfiler( const std::string& sRootName );
	virtual ~VistaBasicProfiler();

	void NewFrame();
	void StartSection( const std::string& sName );
	void StopSection();
	bool StopSection( const std::string& sName );

	void Reset();

	static VistaBasicProfiler* GetSingleton();
	static void SetSingleton( VistaBasicProfiler* pProfiler );

	void PrintProfile( std::ostream& oStream, int iMaxDepth = 10 );
	static bool PrintMultipleProfiles( std::ostream& oStream, 
									const std::vector< std::string >& vecNames,
									const std::vector< VistaBasicProfiler* >& vecProfilers, int iMaxDepth = 10 );

	ProfileTreeNode* GetRoot() const;

	virtual int Serialize( IVistaSerializer& oSerializer ) const;
	virtual int DeSerialize( IVistaDeSerializer& oDeSerializer );

	virtual std::string GetSignature() const;

	class VISTATOOLSAPI ProfileScopeObject
	{
	public:
		ProfileScopeObject( const std::string& sName );
		ProfileScopeObject( const std::string& sName, VistaBasicProfiler* pProf );
		~ProfileScopeObject();
	private:
		std::string m_sName;
		VistaBasicProfiler* m_pProfiler;
	};

private:
	static void DoPrintMultipleProfiles( std::ostream& oStream, const std::vector< ProfileTreeNode* >& vecProfilers, 
									   int nDepth, int iMaxDepth, int nNameColWidth, int nEntryColWdith, int nTypeColWidth );

	ProfileTreeNode*	m_pProfileRoot;
	ProfileTreeNode*	m_pProfileCurrent;
	int					m_iMaxNameLength;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTABASICPROFILER_H
