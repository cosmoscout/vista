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


#include "VistaStreamUtils.h"

#include "VistaStreamManager.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* StreamManager Singleton                                                    */
/*============================================================================*/

namespace
{
	VistaStreamManager* S_pManager = NULL;
}

VistaStreamManager* vstr::GetStreamManager()
{
	if( S_pManager == NULL )
		S_pManager = new VistaStreamManager;
	return S_pManager;
}
void vstr::SetStreamManager( VistaStreamManager* pStreamManager,
										const bool bDeleteOldManager )
{
	if( S_pManager != NULL && bDeleteOldManager )
		delete S_pManager;
	S_pManager = pStreamManager;
}
void vstr::DestroyStreamManager()
{
	delete S_pManager;
	S_pManager = NULL;
}
std::ostream& vstr::Stream( const std::string& sName )
{
	return GetStreamManager()->GetStream( sName );
}

/*============================================================================*/
/* NullStream Singleton                                                       */
/*============================================================================*/

class VistaNullOutstream : public std::ostream
{
private:
	class NullBuffer : public std::streambuf
	{
	public:
		NullBuffer() : std::streambuf()	{}
		~NullBuffer()	{}
		
	protected:
		virtual int_type overflow( int_type cChar )
		{
			return cChar;
		}	
	};	
public:
	VistaNullOutstream()
	: std::ostream( (std::streambuf*)( new NullBuffer() ) )
	{
	}
	virtual ~VistaNullOutstream() {}
};

VISTABASEAPI std::ostream& vstr::GetNullStream()
{
	static VistaNullOutstream S_oNullStream;
	return S_oNullStream;
}


/*============================================================================*/
/* General Streams                                                            */
/*============================================================================*/

namespace
{
	std::ostream* S_pOutStream = &std::cout;
	std::ostream* S_pWarnStream = &std::cerr;
	std::ostream* S_pErrStream = &std::cerr;
	#ifdef DEBUG
	std::ostream* S_pDebugStream = &std::cout;
	#else
	std::ostream* S_pDebugStream = &vstr::GetNullStream();
	#endif
}

void vstr::SetOutStream( std::ostream* oStream )
{
	S_pOutStream = oStream;
}

void vstr::SetWarnStream( std::ostream* oStream )
{
	S_pWarnStream = oStream;
}

void vstr::SetErrStream( std::ostream* oStream )
{
	S_pErrStream = oStream;
}

void vstr::SetDebugStream( std::ostream* oStream )
{
	S_pDebugStream = oStream;
}

std::ostream& vstr::out()
{
	return (*S_pOutStream);
}

std::ostream& vstr::warn()
{
	return (*S_pWarnStream);
}

std::ostream& vstr::err()
{
	return (*S_pErrStream);
}

std::ostream& vstr::debug()
{
	return (*S_pDebugStream);
}


std::ostream& vstr::outi()
{
	return (*S_pOutStream) << vstr::indent;
}

std::ostream& vstr::warni()
{
	return (*S_pWarnStream) << vstr::indent;
}

std::ostream& vstr::erri()
{
	return (*S_pErrStream) << vstr::indent;
}

std::ostream& vstr::debugi()
{
	return (*S_pDebugStream) << vstr::indent;
}

std::ostream& vstr::warnp()
{
	return (*S_pWarnStream) << vstr::indent << vstr::warnprefix;
}

std::ostream& vstr::errp()
{
	return (*S_pErrStream) << vstr::indent << vstr::errprefix;
}


/*============================================================================*/
/* StreamManipulators                                                         */
/*============================================================================*/

vstr::formattime::formattime( const double dTime, const int nPrecision )
: m_dTime( dTime )
, m_nPrecision( nPrecision )
{
}

std::ostream& vstr::formattime::operator()( std::ostream& oStream ) const
{
	std::ios_base::fmtflags oFlags = oStream.flags();
	std::streamsize nCurrentPrecision = oStream.precision( m_nPrecision );		
	oStream.setf( std::ios_base::fixed );
	//oStream.setf( std::ios_base::floatfield );
	oStream << m_dTime;
	oStream.precision( nCurrentPrecision );	
	oStream.flags( oFlags );
	return oStream;
}

std::ostream& vstr::systime( std::ostream& oStream )
{
	oStream << std::setw( 11 )
		<< vstr::formattime( vstr::GetStreamManager()->GetSystemTime() );
	return oStream;
}
std::ostream& vstr::relativetime( std::ostream& oStream )
{
	oStream << std::setw( 14 )
		<< vstr::formattime( vstr::GetStreamManager()->GetMicroTime(), 9 );
	return oStream;

	
}
std::ostream& vstr::frameclock( std::ostream& oStream )
{
	oStream << vstr::formattime( vstr::GetStreamManager()->GetFrameClock() );	
	return oStream;
}
std::ostream& vstr::date( std::ostream& oStream )
{
	oStream << std::setw(16) 
		<< vstr::GetStreamManager()->GetDateString();
	return oStream;
}
std::ostream& vstr::framecount( std::ostream& oStream )
{
	oStream << std::setw(8) 
		<< vstr::GetStreamManager()->GetFrameCount();
	return oStream;
}
std::ostream& vstr::info( std::ostream& oStream )
{
	GetStreamManager()->PrintInfo( oStream );
	return oStream;
}
std::ostream& vstr::nodename( std::ostream& oStream )
{
	oStream << vstr::GetStreamManager()->GetNodeName();
	return oStream;
}
std::ostream& vstr::framerate( std::ostream& oStream )
{
	oStream << vstr::GetStreamManager()->GetFrameRate();
	return oStream;
}	

std::ostream& vstr::indent( std::ostream& oStream )
{
	return ( oStream << GetStreamManager()->GetIndentation() );
}
std::ostream& vstr::singleindent( std::ostream& oStream )
{
	return ( oStream << GetStreamManager()->GetIndentationLevelString() );
}
std::ostream& vstr::indentpostfix( std::ostream& oStream )
{
	return ( oStream << GetStreamManager()->GetIndentationPrefixString() );
}
std::ostream& vstr::indentprefix( std::ostream& oStream )
{
	return ( oStream << GetStreamManager()->GetIndentationPostfixString() );
}
vstr::IndentObject::IndentObject()
{
	vstr::GetStreamManager()->AddToIndentationLevel( +1 );
}
vstr::IndentObject::~IndentObject()
{
	vstr::GetStreamManager()->AddToIndentationLevel( -1 );
}

std::ostream& vstr::warnprefix( std::ostream& oStream )
{
	return ( oStream << GetStreamManager()->GetWarningPrefix() );
}
std::ostream& vstr::errprefix( std::ostream& oStream )
{
	return ( oStream << GetStreamManager()->GetErrorPrefix() );
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
