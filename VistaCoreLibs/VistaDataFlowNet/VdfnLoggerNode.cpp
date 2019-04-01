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


#include "VdfnLoggerNode.h"
#include "VdfnUtil.h"

#include <VistaAspects/VistaConversion.h>

#include <VistaBase/VistaStreamUtils.h>
#include "VistaTools/VistaFileSystemFile.h"

#include <set>
#include <algorithm>
#include <iostream>
#include <iterator>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnLoggerNode::VdfnLoggerNode(const std::string &strPrefix,
	             bool bWriteHeader,
				 bool bWriteTimeStamp,
				 bool bWriteTimeDiff,
				 bool bLogToConsole,
				 const std::list<std::string> &liPorts,
				 const std::list<std::string> &liTrigger,
				 const std::string& sSep)
	: VdfnShallowNode(),
	m_pPrototype( new TVdfnPortTypeCompare<TVdfnPort<std::string> > ),
	m_bWriteHeader(bWriteHeader),
	m_bWriteTimeStamp(bWriteTimeStamp),
	m_bWriteTimeDiff(bWriteTimeDiff),
	m_ofstream( NULL ),
	m_nLastUpdate(0),
	m_liInPorts(liPorts),
	m_liTrigger(liTrigger),
	m_pNotSet(new TVdfnPort<std::string> ),
	m_pFileName(NULL),
	m_strPrefix(strPrefix),
	m_bLogToConsole(bLogToConsole),
	m_nFileNameCnt(0),
	m_sSep(sSep),
	m_nUpdateScore(0)
{
	m_pNotSet->SetValue( "<not-set>", 0 );

}

VdfnLoggerNode::~VdfnLoggerNode()
{
	delete m_pPrototype;
	delete m_pNotSet;
	if(m_ofstream)
	{
		(*m_ofstream).flush();
		(*m_ofstream).close();
	}

	delete m_ofstream;
}

 bool VdfnLoggerNode::SetInPort(const std::string &sName,
					   IVdfnPort *pPort )
{
	if( m_pPrototype->IsTypeOf( pPort ) )
	{
		// ok, this is a string node
		return VdfnShallowNode::SetInPort(sName, pPort);
	}
	return false;
}


void VdfnLoggerNode::PrepareFile()
{
	if(m_ofstream)
	{
		delete m_ofstream;
		m_ofstream = NULL;
	}

	m_strFileName = m_pFileName->GetValue();

	if(m_strFileName.empty())
		return;

	VistaFileSystemFile oFile( m_strPrefix + m_strFileName );
	if( oFile.Exists() == false && oFile.CreateWithParentDirectories() == false )
	{
		vstr::warnp() << "[VdfnLoggerNode]: Couldnot create file ["
			<< m_strPrefix + m_strFileName << "]" << std::endl;
		return;
	}


	m_ofstream = new std::ofstream((m_strPrefix + m_strFileName).c_str());
}

bool VdfnLoggerNode::GetIsValid() const
{
	return m_ofstream || m_pFileName || m_bLogToConsole;
}

bool VdfnLoggerNode::PrepareEvaluationRun()
{
	if(!GetIsEnabled())
		return true; // ok for disabled nodes

	if(m_pFileName)
		return true;

	m_pFileName = dynamic_cast<TVdfnPort<std::string>*>( GetInPort("__filename") );
	if(!m_pFileName && !m_bLogToConsole)
		return false;


	if(m_liInPorts.empty())
		m_liInPorts = GetInPortNames(); // cache port names (sorted list)

	// this is our private port ;)
	m_liInPorts.remove( "__filename" );

	// prepare trigger map in m_pHlp
	for(std::list<std::string>::const_iterator cit = m_liTrigger.begin();
		cit != m_liTrigger.end(); ++cit)
	{
		IVdfnPort *pPort = GetInPort( *cit );
		if(pPort)
		{
			// store initial revision for this port in the portmap
			m_mpRevisions[ pPort ] = pPort->GetUpdateCounter();
		}
	}


	std::list<std::string> liPortNames;
	std::map<IVdfnPort*, std::string> lostNames;

	std::set< std::pair<std::string, IVdfnPort*> > Ports, SetPorts, DiffPorts;
	for(PortMap::const_iterator it = m_mpInPorts.begin();
		it != m_mpInPorts.end(); ++it )
	{
		if( (*it).second == m_pFileName )
			continue; // skip diz...
		 Ports.insert( std::pair<std::string, IVdfnPort*>( (*it).first, (*it).second) );
	}

	for(std::list<std::string>::const_iterator cit = m_liInPorts.begin();
		cit != m_liInPorts.end(); ++cit)
	{

		TVdfnPort<std::string> *pPort
			= VdfnUtil::GetInPortTyped<TVdfnPort<std::string>*>( *cit, this );
		if(pPort)
		{
			m_vecPorts.push_back( pPort );
			SetPorts.insert( std::pair<std::string,IVdfnPort*> ( *cit, pPort ) );
		}
		else
		{
			m_vecPorts.push_back( m_pNotSet );
			IVdfnPort *pPrt = GetInPort( *cit );
			if(pPrt)
				lostNames[ pPrt ] = *cit;
		}

		liPortNames.push_back( *cit );
	}

	// calc diff in sets
	std::set_difference( Ports.begin(), Ports.end(),
		                 SetPorts.begin(), SetPorts.end(),
						 std::inserter( DiffPorts, DiffPorts.end() ) );

	for( std::set< std::pair<std::string, IVdfnPort*> >::const_iterator dit
			= DiffPorts.begin();
	   dit != DiffPorts.end(); ++dit )
	{
		TVdfnPort<std::string> *pPort
			= dynamic_cast<TVdfnPort<std::string>*>( (*dit).second );
		if(pPort)
			m_vecPorts.push_back( pPort );
		else
			m_vecPorts.push_back( m_pNotSet );

		if( GetHasInPort( (*dit).first ) )
			liPortNames.push_back( (*dit).first );
		else
		{
			std::map<IVdfnPort*, std::string>::const_iterator lnit = lostNames.find( pPort );
			if(pPort)
			{
				liPortNames.push_back( (*lnit).second );
			}
			else
				liPortNames.push_back("<not-set>");
		}
	}

	m_liInPorts = liPortNames;

	return true;
}

unsigned int VdfnLoggerNode::CalcUpdateNeededScore() const
{
	if(m_mpRevisions.empty())
		return VdfnShallowNode::CalcUpdateNeededScore();
	else
	{
		// check revisions of trigger ports
		for(REVMAP::const_iterator cit = m_mpRevisions.begin();
			cit != m_mpRevisions.end(); ++cit)
		{
			IVdfnPort *pPort = (*cit).first;
			if( pPort->GetUpdateCounter() != (*cit).second )
			{
				// update score
				m_mpRevisions[ pPort ] = pPort->GetUpdateCounter();
				return ++m_nUpdateScore; // leave loop on first trigger!
			}
		}
	}
	return m_nUpdateScore;
}


bool VdfnLoggerNode::DoEvalNode()
{
	if( m_pFileName != NULL &&
		m_pFileName->GetUpdateCounter() != m_nFileNameCnt )
	{
		PrepareFile();
		if(m_bWriteHeader)
			WriteHeader();

		m_nFileNameCnt = m_pFileName->GetUpdateCounter();

	}

	if(m_ofstream == NULL && m_bLogToConsole == false)
		return false;

	bool bPrepend = false;
	if(m_bWriteTimeStamp)
	{
		const double nLastUpdate = GetUpdateTimeStamp();

		if( m_ofstream )
		{
			(*m_ofstream) << VistaConversion::ToString(nLastUpdate);
		}
		if( m_bLogToConsole )
		{
			vstr::outi() << VistaConversion::ToString(nLastUpdate);
		}
			
		if(m_bWriteTimeDiff)
		{
			if(m_nLastUpdate)
			{
				if( m_ofstream )
				{
					(*m_ofstream) << m_sSep
								  << VistaConversion::ToString(nLastUpdate - m_nLastUpdate);
				}
				if( m_bLogToConsole )
				{
					vstr::outi() << m_sSep
								 << VistaConversion::ToString(nLastUpdate - m_nLastUpdate);
				}
			}
			else
			{
				if( m_ofstream )
					(*m_ofstream) << m_sSep << "0";
				if( m_bLogToConsole )
					vstr::outi() << m_sSep << "0";
			}
			m_nLastUpdate = nLastUpdate;
		}
		bPrepend = true;
	}

	for( std::vector<TVdfnPort<std::string>*>::const_iterator cit = m_vecPorts.begin();
		cit != m_vecPorts.end(); ++cit )
	{
		TVdfnPort<std::string> *pSPort = *cit;
		if(bPrepend)
		{
			if( m_ofstream )
				(*m_ofstream) << m_sSep;
			if( m_bLogToConsole )
				vstr::out() << m_sSep;
		}
		
		if( m_ofstream )
			(*m_ofstream) << pSPort->GetValueConstRef();

		if( m_bLogToConsole )
			vstr::out() << pSPort->GetValueConstRef();
	  
		bPrepend = true;
	}
	if( m_ofstream )
	{
		(*m_ofstream) << "\n";
		(*m_ofstream).flush();
	}
	if( m_bLogToConsole )
		vstr::out() << std::endl;

	return true;
}

void VdfnLoggerNode::WriteHeader()
{
	if(m_ofstream == NULL)
		return;

	bool bPrepend = false;
	if(m_bWriteTimeStamp)
	{
		(*m_ofstream) << "TS";
		if(m_bWriteTimeDiff)
			(*m_ofstream) << m_sSep << "DT";
		bPrepend=true;
	}
	for( std::list<std::string>::const_iterator cit = m_liInPorts.begin();
		cit != m_liInPorts.end(); ++cit )
	{
		if(bPrepend)
			(*m_ofstream) << m_sSep;

		(*m_ofstream) << *cit;
		bPrepend = true;
	}
	(*m_ofstream) << "\n";
	(*m_ofstream).flush();
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


