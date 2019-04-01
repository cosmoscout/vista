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


#include "VistaObjectRegistry.h"
#include "VistaNameable.h"

#include <string>
#include <algorithm>
#include <functional>
using std::string;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace
{
	class _count : public std::unary_function<VistaObjectRegistry::value_type&,bool>
	{
	public:
		_count( const std::string &strName )
		: m_strName(strName)
		, m_cnt(0)
		{}

		std::string m_strName;
		VistaObjectRegistry::size_type m_cnt;
		bool operator()( const VistaObjectRegistry::value_type &v )
		{
			if( v->GetNameForNameable() == m_strName )
			{
				++m_cnt;
				return true;
			}
			return false;
		}
	};
}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaObjectRegistry::VistaObjectRegistry()
{
}

VistaObjectRegistry::~VistaObjectRegistry()
{
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
unsigned int VistaObjectRegistry::RegisterNameable(IVistaNameable* pNameable)
{
	m_vecObjectRegistry.push_back(pNameable);
	return (unsigned int)m_vecObjectRegistry.size()-1;
}

IVistaNameable *VistaObjectRegistry::UnregisterNameable(int iID)
{
	if(iID < 0 || iID >= (int)m_vecObjectRegistry.size())
		return NULL;
	IVistaNameable *pNam(m_vecObjectRegistry[iID]);
	m_vecObjectRegistry[iID] = NULL;
	return pNam;
}


IVistaNameable *VistaObjectRegistry::UnregisterNameable(const string& strName)
{
	int iID(this->FindNameable(strName));
	if(iID < 0)
		return NULL;
	return this->UnregisterNameable(iID);
}


bool VistaObjectRegistry::UnregisterNameable(IVistaNameable* pNameable)
{
	for(unsigned int i =0; i < m_vecObjectRegistry.size(); ++i)
	{
		if(m_vecObjectRegistry[i]== pNameable)
		{
			m_vecObjectRegistry[i] = 0;
			return true;
		}
	}
	return false;
}


IVistaNameable *VistaObjectRegistry::RetrieveNameable(int iID) const
{
	if(iID < 0 || iID >= (int)m_vecObjectRegistry.size())
		return NULL;
	return m_vecObjectRegistry[iID];
}


IVistaNameable *VistaObjectRegistry::RetrieveNameable(const string& strName) const
{
	int iID(this->FindNameable(strName));
	if(iID < 0)
		return NULL;
	return m_vecObjectRegistry[iID];
}

bool VistaObjectRegistry::HasNameable( const std::string &strName ) const
{
	return (FindNameable(strName) >= 0);
}

void VistaObjectRegistry::ClearRegistry()
{
	m_vecObjectRegistry.clear();
}

int VistaObjectRegistry::FindNameable(const std::string& strName) const
{
	for(unsigned int i=0; i<m_vecObjectRegistry.size(); ++i)
	{
		if(m_vecObjectRegistry[i] // can be NULL
		&& m_vecObjectRegistry[i]->GetNameForNameable() == strName)
			return i;

	}
	return -1;
}

VistaObjectRegistry::const_iterator VistaObjectRegistry::begin() const
{
	return m_vecObjectRegistry.begin();
}

VistaObjectRegistry::const_iterator VistaObjectRegistry::end() const
{
	return m_vecObjectRegistry.end();
}


VistaObjectRegistry::iterator VistaObjectRegistry::begin()
{
	return m_vecObjectRegistry.begin();
}

VistaObjectRegistry::iterator VistaObjectRegistry::end()
{
	return m_vecObjectRegistry.end();
}


VistaObjectRegistry::reverse_iterator VistaObjectRegistry::rbegin()
{
	return m_vecObjectRegistry.rbegin();
}

VistaObjectRegistry::reverse_iterator VistaObjectRegistry::rend()
{
	return m_vecObjectRegistry.rend();
}

VistaObjectRegistry::size_type VistaObjectRegistry::count( const std::string &sname ) const
{
	_count c = std::for_each( m_vecObjectRegistry.begin(), m_vecObjectRegistry.end(), _count( sname ) );
	return c.m_cnt;
}




/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

