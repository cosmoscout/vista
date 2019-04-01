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


#include "VistaExceptionBase.h"

#include <VistaBase/VistaStreamUtils.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <cstdio>
#include <cmath>
#include <cstdlib>

#if defined(LINUX)
#include <execinfo.h>
#endif


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaExceptionBase::VistaExceptionBase(const char *pcExMsg,
					 const char *pcExSource, int iExLine, int iExNum) throw()
: std::exception(),
  m_sExceptionText(pcExMsg),
  m_sExceptionSource(pcExSource),
  m_iExceptionLine(iExLine),
  m_iExceptionNumber(iExNum)
{
#if defined(LINUX)
	/// @todo integrate with stack walker
	void *array[25]; // we trace 25 levels
	int nSize = backtrace(array, 25);
	char **symbols = backtrace_symbols(array, nSize);
	for(int i=0; i < nSize; ++i)
	{
		m_sBacktrace = m_sBacktrace + std::string(symbols[i]) + std::string("\n");
	}

	free(symbols);
#endif
}

// PRIVATE!
VistaExceptionBase::VistaExceptionBase()
: std::exception()
,  m_iExceptionLine( -1 )
, m_iExceptionNumber( -1 )
{
}


VistaExceptionBase::~VistaExceptionBase() throw()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
const char* VistaExceptionBase::what() const throw()
{
	return m_sExceptionText.c_str();
}

int    VistaExceptionBase::GetExceptionNumber() const
{
	return m_iExceptionNumber;
}

std::string VistaExceptionBase::GetExceptionText() const
{
	return m_sExceptionText;
}

int    VistaExceptionBase::GetExceptionLine() const
{
	return m_iExceptionLine;
}

std::string VistaExceptionBase::GetExceptionSource() const
{
	return m_sExceptionSource;
}

void VistaExceptionBase::PrintException() const
{
	PrintException( vstr::err(), true );
}

void VistaExceptionBase::PrintException( std::ostream& oStream, bool bAutoIndent ) const
{
	if( bAutoIndent == false )
	{
		oStream << "VistaExceptionBase() -- Exception [" << GetExceptionNumber() << "]\n"
				<< "===============================================\n"
				<< m_sExceptionText << "\n"
				<< "===============================================\n"
				<< "Location: " << m_sExceptionSource << "\n"
				<< "Line:     " << m_iExceptionLine << "\n"
				<< "===============================================\n";
	}
	else
	{
		oStream << vstr::indent << "VistaExceptionBase() -- Exception [" << GetExceptionNumber() << "]\n"
				<< vstr::indent << "===============================================\n"
				<< vstr::indent << m_sExceptionText << "\n"
				<< vstr::indent << "===============================================\n"
				<< vstr::indent << "Location: " << m_sExceptionSource << "\n"
				<< vstr::indent << "Line:     " << m_iExceptionLine << "\n"
				<< vstr::indent << "===============================================\n";
	}
	oStream.flush();
}

std::string VistaExceptionBase::GetPrintStatement() const
{
	std::stringstream oText;
	PrintException( oText );
	return oText.str();
}

std::string VistaExceptionBase::GetBacktraceString() const
{
	return m_sBacktrace;
}

void   VistaExceptionBase::PrintBacktrace() const
{
	vstr::err() << m_sBacktrace << std::endl;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
