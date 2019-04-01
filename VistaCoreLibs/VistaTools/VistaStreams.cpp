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


#include "VistaStreams.h"

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamManager.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaPropertyList.h>

#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <VistaInterProcComm/Concurrency/VistaSemaphore.h>
#include <VistaInterProcComm/Concurrency/VistaThread.h>

#if defined(WIN32)
	#include <windows.h>
	#include <conio.h>
#else
	#include <unistd.h>
#endif

#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <sstream>

/*============================================================================*/
/*  COLORSTREAM                                                               */
/*============================================================================*/

namespace
{

#ifdef WIN32
	// singletons for the Win console file handle, plus  areference count
	HANDLE		m_hConsole = NULL;
	int			m_ConsoleRefCount = 0;

	bool IsWinXPOrEarlier()
	{
		static int nVersion = -1;

		if( nVersion == -1 )
		{
			OSVERSIONINFO oVersionInfo;
			ZeroMemory( &oVersionInfo, sizeof(OSVERSIONINFO) );
			oVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx( &oVersionInfo );
			nVersion = oVersionInfo.dwMajorVersion;
		}
		return ( nVersion <= 5 );
	}

	void ApplyTextColor( const VistaColorOutstream::CONSOLE_COLOR iColor, WORD& oConsoleAttributes )
	{
		oConsoleAttributes &= ~( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY );

		switch( iColor )
		{		
			case VistaColorOutstream::CC_BLACK:
				break;
			case VistaColorOutstream::CC_DARK_GRAY:
				oConsoleAttributes |= FOREGROUND_INTENSITY;
				break;
			default:
			case VistaColorOutstream::CC_DEFAULT:
			case VistaColorOutstream::CC_LIGHT_GRAY:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				break;
			case VistaColorOutstream::CC_WHITE:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_RED:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_RED:
				oConsoleAttributes |= FOREGROUND_RED;
				break;
			case VistaColorOutstream::CC_GREEN:
				oConsoleAttributes |= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_GREEN:
				oConsoleAttributes |= FOREGROUND_GREEN;
				break;
			case VistaColorOutstream::CC_BLUE:	
				oConsoleAttributes |= FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_BLUE:
				oConsoleAttributes |= FOREGROUND_BLUE;
				break;
			case VistaColorOutstream::CC_YELLOW:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_YELLOW:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_GREEN;
				break;
			case VistaColorOutstream::CC_MAGENTA:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_MAGENTA:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_BLUE;
				break;
			case VistaColorOutstream::CC_CYAN:	
				oConsoleAttributes |= FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_CYAN:
				oConsoleAttributes |= FOREGROUND_GREEN | FOREGROUND_BLUE;
				break;		
		}
	}
	void ApplyBackgroundColor( const VistaColorOutstream::CONSOLE_COLOR iColor,	WORD& oConsoleAttributes )
	{
		oConsoleAttributes &= ~( BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY );

		switch( iColor )
		{
			default:
			case VistaColorOutstream::CC_DEFAULT:
			case VistaColorOutstream::CC_BLACK:
				break;
			case VistaColorOutstream::CC_DARK_GRAY:
				oConsoleAttributes |= BACKGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_LIGHT_GRAY:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
				break;
			case VistaColorOutstream::CC_WHITE:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_RED:
				oConsoleAttributes |= BACKGROUND_RED |BACKGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_RED:
				oConsoleAttributes |= BACKGROUND_RED;
				break;
			case VistaColorOutstream::CC_GREEN:
				oConsoleAttributes |= BACKGROUND_GREEN | BACKGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_GREEN:
				oConsoleAttributes |= BACKGROUND_GREEN;
				break;
			case VistaColorOutstream::CC_BLUE:	
				oConsoleAttributes |= BACKGROUND_BLUE | BACKGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_BLUE:
				oConsoleAttributes |= BACKGROUND_BLUE;
				break;
			case VistaColorOutstream::CC_YELLOW:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_YELLOW:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_GREEN;
				break;
			case VistaColorOutstream::CC_MAGENTA:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_MAGENTA:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_BLUE;
				break;
			case VistaColorOutstream::CC_CYAN:	
				oConsoleAttributes |= BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
				break;
			case VistaColorOutstream::CC_DARK_CYAN:
				oConsoleAttributes |= BACKGROUND_GREEN | BACKGROUND_BLUE;
				break;	
		}	
	}
#else
	//VistaColorOutstream::CONSOLE_COLOR iStdTextColor = VistaColorOutstream::CC_DEFAULT;
	//VistaColorOutstream::CONSOLE_COLOR iStdBackgroundColor = VistaColorOutstream::CC_DEFAULT;
	std::string sStdColorTag = "\033[0m";

	std::string aForegroundTags[] =
	{
		"\033[0;30",  //CC_BLACK
		"\033[0;1;30",  //CC_DARK_GRAY
		"\033[0;37",  //CC_LIGHT_GRAY
		"\033[0;1;37",  //CC_WHITE
		"\033[0;1;31",  //CC_RED
		"\033[0;31",  //CC_DARK_RED
		"\033[0;1;32",  //CC_GREEN,
		"\033[0;32",  //CC_DARK_GREEN
		"\033[0;1;34",  //CC_BLUE
		"\033[0;34",  //CC_DARK_BLUE
		"\033[0;1;33",  //CC_YELLOW
		"\033[0;33",  //CC_DARK_YELLOW
		"\033[0;1;35",  //CC_MAGENTA
		"\033[0;35",  //CC_DARK_MAGENTA
		"\033[0;1;36",  //CC_CYAN,
		"\033[0;36",  //CC_DARK_CYAN
		"\033[0",      //CC_DEFAULT
	};
	std::string aBackgroundTags[] =
	{
		";40m",  //CC_BLACK
		";40m",  //CC_DARK_GRAY
		";47m",  //CC_LIGHT_GRAY
		";m",	 //CC_WHITE
		";41m",  //CC_RED
		";41m",  //CC_DARK_RED
		";42m",  //CC_GREEN,
		";42m",  //CC_DARK_GREEN
		";44m",  //CC_BLUE
		";44m",  //CC_DARK_BLUE
		";43m",  //CC_YELLOW
		";43m",  //CC_DARK_YELLOW
		";45m",  //CC_MAGENTA
		";45m",  //CC_DARK_MAGENTA
		";46m",  //CC_CYAN,
		";46m",  //CC_DARK_CYAN
		"m",     //CC_DEFAULT
	};
#endif
} // empty namespace


class ConsoleLock
{
public:
	ConsoleLock()
	{
		GetMutex().Lock();
	}
	~ConsoleLock()
	{
		GetMutex().Unlock();
	}
	VistaMutex& GetMutex()
	{
		static VistaMutex m_oMutex;
		return m_oMutex;
	}	
};

const std::string S_aColorNames[] = 
{
	"BLACK",
	"DARK_GRAY",
	"LIGHT_GRAY",
	"WHITE",
	"RED",
	"DARK_RED",
	"GREEN",
	"DARK_GREEN",
	"BLUE",
	"DARK_BLUE",
	"YELLOW",
	"DARK_YELLOW",
	"MAGENTA",
	"DARK_MAGENTA",
	"CYAN",
	"DARK_CYAN",
	"DEFAULT",
};


/* VistaColorOutstream::VistaColorOutstreamBuffer                           */

class VistaColorOutstream::Buffer : public std::streambuf
{
public:
	Buffer( const size_t iBufferSize,
								const bool bUseErrorStream )
	: std::streambuf()
	, m_iTextColor( VistaColorOutstream::CC_DEFAULT )
	, m_iBackgroundColor( VistaColorOutstream::CC_DEFAULT )
	, m_vecBuffer( iBufferSize )
	, m_iReferenceCount( 1 )
	, m_pConsoleStream( NULL )
	{
		char* pBase = &m_vecBuffer.front();
		setp( pBase, pBase + m_vecBuffer.size() - 1 );

		if( bUseErrorStream )
			m_pConsoleStream = &std::cerr;
		else
			m_pConsoleStream = &std::cout;

#ifdef WIN32
		if( m_ConsoleRefCount == 0 )
		{
			m_hConsole = CreateFile( "CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE,
									NULL, OPEN_EXISTING, 0, NULL );
		}
		++m_ConsoleRefCount;
		CONSOLE_SCREEN_BUFFER_INFO oInfo;
		GetConsoleScreenBufferInfo( m_hConsole, &oInfo );
		m_oAttributes = oInfo.wAttributes;
#else
		
#endif
	}

	~Buffer()
	{
		sync();
#ifdef WIN32
		--m_ConsoleRefCount;
		if( m_ConsoleRefCount <= 0 )
			CloseHandle( m_hConsole );
#else
#endif		
	}

	void RefUp()
	{
		++m_iReferenceCount;
	}

	void RefDown()
	{
		--m_iReferenceCount;
		if( m_iReferenceCount < 0 )
			delete this;
	}

	void SetTextColor( const CONSOLE_COLOR iColor )
	{
		Flush();
		m_iTextColor = iColor;
#ifdef WIN32
		ApplyTextColor( m_iTextColor, m_oAttributes );
#else
		m_sTag = aForegroundTags[m_iTextColor] + aBackgroundTags[m_iBackgroundColor];
#endif
	}
	CONSOLE_COLOR GetTextColor() const
	{
		return m_iTextColor;
	}
	void SetBackgroundColor( const CONSOLE_COLOR iColor )
	{
		Flush();
		m_iBackgroundColor = iColor;
#ifdef WIN32
		ApplyBackgroundColor( m_iBackgroundColor, m_oAttributes );
#else
		m_sTag = aForegroundTags[m_iTextColor] + aBackgroundTags[m_iBackgroundColor];
#endif
	}
	CONSOLE_COLOR GetBackgroundColor() const
	{
		return m_iBackgroundColor;
	}

protected:
	virtual int_type overflow( int_type cChar )
	{
		if( cChar != traits_type::eof() )
		{
			*pptr() = cChar;
			pbump(1);
			if( Flush() )
				return cChar;
		}

		return traits_type::eof();

	}
	virtual int sync()
	{
		if( Flush() )
			return 0;
		else
			return -1;
	}

#ifdef WIN32
	bool Flush()
	{
		std::ptrdiff_t n = pptr() - pbase();
		if( n == 0 )
			return true;

		std::cout.flush();

		// we lock globally to verify that only one thread
		// writes to the console at any time
		ConsoleLock oLock;
		
		CONSOLE_SCREEN_BUFFER_INFO oInfo;
		GetConsoleScreenBufferInfo( m_hConsole, &oInfo );
		SetConsoleTextAttribute( m_hConsole, m_oAttributes );		

		LPDWORD oReturnChars = 0;

		// WindowsXP has problems with directly writing to the console, so we just use the stream instead
		if( IsWinXPOrEarlier() )
		{
			m_pConsoleStream->write( &m_vecBuffer[0], n );			
		}
		else
		{
			WriteConsole( m_hConsole, &m_vecBuffer[0], (DWORD)n, oReturnChars, NULL );
		}
		
		
		SetConsoleTextAttribute( m_hConsole, oInfo.wAttributes );

		pbump( (DWORD)-n );
		return true;
	}
#else
	bool Flush()
	{
		std::ptrdiff_t n = pptr() - pbase();
		if( n == 0 )
			return true;

		//ConsoleLock oLock;
		
		m_pConsoleStream->write( m_sTag.c_str(), m_sTag.size() );
		m_pConsoleStream->write( &m_vecBuffer[0], n );
		m_pConsoleStream->write( sStdColorTag.c_str(), sStdColorTag.size() );
		m_pConsoleStream->flush();
		
		pbump( -n );
		return true;
	}
#endif

private:

#ifdef WIN32
	WORD					m_oAttributes;
#else
	std::string				m_sTag;
#endif
	CONSOLE_COLOR			m_iTextColor;
	CONSOLE_COLOR			m_iBackgroundColor;
	std::vector<char>		m_vecBuffer;
	int						m_iReferenceCount;
	std::ostream*			m_pConsoleStream;
};	

/* VistaColorOutstream                                                       */
VistaColorOutstream::VistaColorOutstream( const CONSOLE_COLOR iTextColor,
											const CONSOLE_COLOR iBackgroundColor,
											const bool bUseErrorStream,
											const size_t iBufferSize)
: std::ostream( (std::streambuf*)( new Buffer( iBufferSize, bUseErrorStream ) ) )
{
	m_pBuffer = static_cast<Buffer*>( rdbuf() );
	m_pBuffer->SetTextColor( iTextColor );
	m_pBuffer->SetBackgroundColor( iBackgroundColor );
}
VistaColorOutstream::VistaColorOutstream( const VistaColorOutstream& oCopy )
: std::ostream( (std::streambuf*)( oCopy.m_pBuffer ) )
, m_pBuffer( oCopy.m_pBuffer )
{
	m_pBuffer->RefUp();
}
VistaColorOutstream::~VistaColorOutstream()
{
	m_pBuffer->RefDown();
}

void VistaColorOutstream::SetTextColor( const CONSOLE_COLOR iColor )
{
	m_pBuffer->SetTextColor( iColor );
}
VistaColorOutstream::CONSOLE_COLOR VistaColorOutstream::GetTextColor() const
{
	return m_pBuffer->GetTextColor();
}
void VistaColorOutstream::SetBackgroundColor( const CONSOLE_COLOR iColor )
{
	m_pBuffer->SetBackgroundColor( iColor );
}
VistaColorOutstream::CONSOLE_COLOR VistaColorOutstream::GetBackgroundColor() const
{
	return m_pBuffer->GetBackgroundColor();
}

std::string VistaColorOutstream::GetConsoleColorName( const CONSOLE_COLOR oColor )
{
	return S_aColorNames[oColor];
}

VistaColorOutstream::CONSOLE_COLOR VistaColorOutstream::GetConsoleColorFromString( const std::string& sName )
{
	std::string sCleanedName = VistaConversion::StringToUpper( sName );
	for( int nColorIndex = 0; nColorIndex < CC_NUM_COLORS; ++nColorIndex )
	{
		if( sCleanedName == S_aColorNames[nColorIndex] )
			return CONSOLE_COLOR( nColorIndex );
	}
	return CONSOLE_COLOR( ~0 );
}


/*============================================================================*/
/*  SPLITOUTSTREAM                                                            */
/*============================================================================*/

/* VistaSplitOutstream::VistaSplitOutstreamBuffer                           */
class VistaSplitOutstream::Buffer : public std::streambuf
{
public:
	Buffer()
	: std::streambuf()
	{		
	}

	~Buffer()
	{
	}

	bool AddStream( std::ostream* oStream )
	{
		m_vecStreams.push_back( oStream );
		return true;
	}
	bool GetHasStream( std::ostream* oStream ) const
	{
		std::vector<std::ostream*>::const_iterator itStream
			= std::find( m_vecStreams.begin(), m_vecStreams.end(), oStream );
		return ( itStream != m_vecStreams.end() );
	}
	bool RemoveStream( std::ostream* oStream )
	{
		std::vector<std::ostream*>::iterator itStream
			= std::find( m_vecStreams.begin(), m_vecStreams.end(), oStream );
		if( itStream == m_vecStreams.end() )
			return false;

		m_vecStreams.erase( itStream );
		return true;
	}
	const std::vector<std::ostream*>& GetStreams() const
	{
		return m_vecStreams;
	}
	void SetStreams( const std::vector<std::ostream*>& vecStreams )
	{
		m_vecStreams = vecStreams;
	}

protected:
	virtual int_type overflow( int_type cChar )
	{
		for( std::vector<std::ostream*>::const_iterator itStream = m_vecStreams.begin();
				itStream != m_vecStreams.end(); ++itStream )
		{
			(*itStream)->put( cChar );
		}
		return cChar;
	}
	virtual int sync()
	{
		for( std::vector<std::ostream*>::const_iterator itStream = m_vecStreams.begin();
			itStream != m_vecStreams.end(); ++itStream )
		{
			(*itStream)->flush();			
		}
		return 0;
	}


private:
	std::vector<std::ostream*>	m_vecStreams;
};	

/* VistaSplitOutstream                                                       */

VistaSplitOutstream::VistaSplitOutstream()
: std::ostream( (std::streambuf*)( new Buffer() ) )

{	
	m_pBuffer = static_cast<Buffer*>( rdbuf() );
}
VistaSplitOutstream::VistaSplitOutstream( const VistaSplitOutstream& oCopy )
: std::ostream( (std::streambuf*)( new Buffer() ) )
{
	m_pBuffer = static_cast<Buffer*>( rdbuf() );
	m_pBuffer->SetStreams( static_cast<Buffer*>( oCopy.rdbuf() )->GetStreams() );
}
VistaSplitOutstream::~VistaSplitOutstream()
{
}

bool VistaSplitOutstream::AddStream( std::ostream* oStream )
{
	return m_pBuffer->AddStream( oStream );
}
bool VistaSplitOutstream::GetHasStream( std::ostream* oStream ) const
{
	return m_pBuffer->GetHasStream( oStream );
}
bool VistaSplitOutstream::RemoveStream( std::ostream* oStream )
{
	return m_pBuffer->RemoveStream( oStream );
}
const std::vector<std::ostream*>& VistaSplitOutstream::GetStreams() const
{
	return m_pBuffer->GetStreams();
}
void VistaSplitOutstream::SetStreams( const std::vector<std::ostream*>& vecStreams )
{
	return m_pBuffer->SetStreams( vecStreams );
}
/*============================================================================*/
/*  PREFIXOUTSTREAM                                                            */
/*============================================================================*/

/* VistaPrefixOutstream::VistaPrefixOutstreamBuffer                           */
class VistaPrefixOutstream::Buffer : public std::streambuf
{
public:
	Buffer( std::ostream* pStream )
	: std::streambuf()
	, m_pStream( pStream )
	, m_bPrefixIndent( false )
	, m_bNewLinePending( true )
	{		
	}

	~Buffer()
	{
	}

	std::ostream* GetOriginalStream() const
	{
		return m_pStream;
	}
	void SetOriginalStream( std::ostream* pStream )
	{
		m_pStream->flush();
		m_pStream = pStream;
	}

	bool GetPrefixIndent() const { return m_bPrefixIndent; }
	void SetPrefixIndent( const bool oValue ) { m_bPrefixIndent = oValue; }
	std::string GetPrefix() const { return m_sPrefix; }
	void SetPrefix( const std::string& oValue ) { m_sPrefix = oValue; }

protected:
	virtual int_type overflow( int_type cChar )
	{
		if( m_bNewLinePending )
		{				
			if( m_bPrefixIndent )
				(*m_pStream) << vstr::indent;
			m_pStream->write( m_sPrefix.c_str(), m_sPrefix.size() );
			m_bNewLinePending = false;
		}	
		m_pStream->put( cChar );
		if( cChar == '\n' )
		{				
			m_bNewLinePending = true;
		}		
		return cChar;
	}
	virtual int sync()
	{
		m_pStream->flush();
		return 0;
	}


protected:
	std::ostream*	m_pStream;
	bool m_bPrefixIndent;
	std::string m_sPrefix;
	bool m_bNewLinePending;
};	

/* VistaPrefixOutstream                                                       */

VistaPrefixOutstream::VistaPrefixOutstream( std::ostream* pStream )
: std::ostream( new Buffer( pStream ) )

{	
	m_pBuffer = static_cast<Buffer*>( rdbuf() );
}


VistaPrefixOutstream::VistaPrefixOutstream( Buffer* pBuffer )
: std::ostream( pBuffer )
{
	m_pBuffer = static_cast<Buffer*>( rdbuf() );
}

VistaPrefixOutstream::VistaPrefixOutstream( const VistaPrefixOutstream& oCopy )
: std::ostream( new Buffer( static_cast<Buffer*>( oCopy.rdbuf() )->GetOriginalStream() ) )
{
	const Buffer* pOrigBuffer = static_cast<Buffer*>( 
												oCopy.rdbuf() );
	m_pBuffer = static_cast<Buffer*>( rdbuf() );
	m_pBuffer->SetPrefix( pOrigBuffer->GetPrefix() );
	m_pBuffer->SetPrefixIndent( m_pBuffer->GetPrefixIndent() );
}
VistaPrefixOutstream::~VistaPrefixOutstream()
{
}

std::ostream* VistaPrefixOutstream::GetOriginalStream() const
{
	return m_pBuffer->GetOriginalStream();
}


void VistaPrefixOutstream::SetOriginalStream( std::ostream* pStream )
{
	m_pBuffer->SetOriginalStream( pStream );
}



bool VistaPrefixOutstream::GetPrefixWithIndent() const
{
	return m_pBuffer->GetPrefixIndent();
}

void VistaPrefixOutstream::SetPrefixWithIndent( const bool bSet )
{
	m_pBuffer->SetPrefixIndent( bSet );
}

std::string VistaPrefixOutstream::GetPrefixString() const
{
	return m_pBuffer->GetPrefix();
}

void VistaPrefixOutstream::SetPrefixString( const std::string& sPrefix )
{
	m_pBuffer->SetPrefix( sPrefix );
}

/*============================================================================*/
/*  CALLBACKPREFIXOUTSTREAM                                                   */
/*============================================================================*/

class VistaCallbackPrefixOutstream::Buffer : public VistaPrefixOutstream::Buffer
{
public:
	Buffer( std::ostream* pStream, ICallback* pCallback, bool bManageCallbackDeletion )
	: VistaPrefixOutstream::Buffer( pStream )
	, m_pCallback( pCallback )
	, m_bManageCallbackDeletion( bManageCallbackDeletion )
	{		
	}

	~Buffer()
	{
		if( m_bManageCallbackDeletion )
			delete m_pCallback;
	}

	ICallback* GetCallback() { return m_pCallback; }
	void SetCallback( ICallback* pCallback, bool bManageCallbackDeletion )
	{
		if( m_bManageCallbackDeletion )
			delete m_pCallback;
		m_pCallback = pCallback;
		m_bManageCallbackDeletion = bManageCallbackDeletion;
	}

protected:
	virtual int_type overflow( int_type cChar )
	{
		if( m_bNewLinePending )
		{				
			if( m_bPrefixIndent )
				(*m_pStream) << vstr::indent;
			std::string sCallbackPrefix = m_pCallback->GetPrefix();
			m_pStream->write( sCallbackPrefix.c_str(), sCallbackPrefix.size() );
			m_pStream->write( m_sPrefix.c_str(), m_sPrefix.size() );			
			m_bNewLinePending = false;
		}	
		m_pStream->put( cChar );
		if( cChar == '\n' )
		{				
			m_bNewLinePending = true;
		}		
		return cChar;
	}
	virtual int sync()
	{
		m_pStream->flush();
		return 0;
	}

private:
	ICallback* m_pCallback;
	bool m_bManageCallbackDeletion;
};	




VistaCallbackPrefixOutstream::VistaCallbackPrefixOutstream( std::ostream* pOriginalStream, 
														   ICallback* pCallback,
														   bool bManageCallbackDeletion )
: VistaPrefixOutstream( new Buffer( pOriginalStream, pCallback, bManageCallbackDeletion ) )
{
	m_pCallbackBuffer = static_cast<Buffer*>( rdbuf() );
}

VistaCallbackPrefixOutstream::~VistaCallbackPrefixOutstream()
{
}

VistaCallbackPrefixOutstream::ICallback* VistaCallbackPrefixOutstream::GetCallback() const
{
	return m_pCallbackBuffer->GetCallback();
}

void VistaCallbackPrefixOutstream::SetCallback( ICallback* pfCallback, bool bManageCallbackDeletion )
{
	return m_pCallbackBuffer->SetCallback( pfCallback, bManageCallbackDeletion );
}


/*============================================================================*/
/*  NULLOUTSTREAM                                                             */
/*============================================================================*/

/* VistaNullOutstream::VistaNullOutstreamBuffer                               */
class VistaNullOutstream::Buffer : public std::streambuf
{
public:
	Buffer()
	: std::streambuf()
	{		
	}

	~Buffer()
	{
	}
	
protected:
	virtual int_type overflow( int_type cChar )
	{
		return cChar;
	}	
};	

/* VistaNullOutstream                                                         */

VistaNullOutstream::VistaNullOutstream()
: std::ostream( (std::streambuf*)( new Buffer() ) )
{	
}
VistaNullOutstream::~VistaNullOutstream()
{
}


/*============================================================================*/
/* THREADSAFESTREAM                                                           */
/*============================================================================*/

class ThreadSafeStreamBuffer : public std::streambuf
{
public:
	ThreadSafeStreamBuffer( std::streambuf* pOrigBuffer,
							const bool bBufferInternally )
	: std::streambuf()
	, m_pOriginalStreamBuffer( pOrigBuffer )
	, m_bBufferInternally( bBufferInternally )
	{
	}
	~ThreadSafeStreamBuffer()
	{
		for( std::map<long, std::ostringstream*>::iterator itBuff = m_mapThreadBuffers.begin();
				itBuff != m_mapThreadBuffers.end(); ++itBuff )
				delete (*itBuff).second;
		delete m_pOriginalStreamBuffer;
	}

protected:
	// output functions


	// sync means we write our output to the actual stream
	virtual int sync()
	{
		if( m_bBufferInternally )
		{
			VistaMutexLock oLock( m_oOriginalBufferMutex );

			std::ostringstream& oBuffer = GetBuffer();
			std::string sString = oBuffer.str();
			m_pOriginalStreamBuffer->sputn( sString.c_str(), sString.size() );
			m_pOriginalStreamBuffer->pubsync();
			oBuffer.str( "" );
		}
		else
		{
			VistaMutexLock oLock( m_oOriginalBufferMutex );
			(*m_pOriginalStreamBuffer).pubsync();
		}
		return 0;
	}

	// buffer input in stringstream
	virtual int_type overflow( int_type nChar )
	{
		if( nChar == traits_type::eof() )
			return traits_type::eof();
			
		if( m_bBufferInternally )
		{
			GetBuffer().put( (char_type)nChar );
			return (char_type)nChar;
		}
		else
		{
			VistaMutexLock oLock( m_oOriginalBufferMutex );
			return (*m_pOriginalStreamBuffer).sputc( nChar );
		}		
	}  
	virtual std::streamsize xsputn( const char_type* aChars, std::streamsize nCount )
	{
		if( m_bBufferInternally )
		{
			GetBuffer().write( aChars, nCount );
			return nCount;
		}
		else
		{
			VistaMutexLock oLock( m_oOriginalBufferMutex );
			return (*m_pOriginalStreamBuffer).sputn( aChars, nCount );
		}		
	}

	virtual std::streambuf* setbuf( char_type*, std::streamsize )
	{
		VISTA_THROW( "ThreadSafeStreamBuffer does not allow changing the target buffer after creation", -1 );
	}
	
	

	// we are only a back-inserting output buffer, so no
	// setting of pos pointers or reading is allowed
	virtual int_type pbackfail( int_type )
	{
		VISTA_THROW( "ThreadSafeStreamBuffer does not support positioning or reading", -1 );
	}
	virtual std::streamsize showmanyc()
	{
		VISTA_THROW( "ThreadSafeStreamBuffer does not support positioning or reading", -1 );
	}
	virtual int_type underflow()
	{
		VISTA_THROW( "ThreadSafeStreamBuffer does not support positioning or reading", -1 );
	}
	virtual int_type uflow()
	{
		VISTA_THROW( "ThreadSafeStreamBuffer does not support positioning or reading", -1 );
	}

	// positioning, not allowed - we're a log
	virtual pos_type seekoff( off_type, std::ios_base::seekdir, std::ios_base::openmode )
	{
		VISTA_THROW( "ThreadSafeStreamBuffer does not support positioning or reading", -1 );
	}
	virtual pos_type seekpos( pos_type, std::ios_base::openmode )
	{
		VISTA_THROW( "ThreadSafeStreamBuffer does not support positioning or reading", -1 );
	}
	

	std::ostringstream& GetBuffer()
	{
		int nID = VistaThread::GetCallingThreadIdentity();
		std::map<long, std::ostringstream*>::iterator itEntry = m_mapThreadBuffers.find( nID );
		if( itEntry != m_mapThreadBuffers.end() )
			return (*(*itEntry).second);
		std::ostringstream* pStream = new std::ostringstream;
		m_mapThreadBuffers[nID] = pStream;
		return (*pStream);
	}
private:
	bool			m_bBufferInternally;
	std::streambuf*	m_pOriginalStreamBuffer;
	VistaMutex		m_oOriginalBufferMutex;
	std::map<long, std::ostringstream*> m_mapThreadBuffers;
};

bool VistaStreams::MakeStreamThreadSafe( std::ostream& oStream,
										const bool bBufferInternally )
{
	if( dynamic_cast<ThreadSafeStreamBuffer*>( oStream.rdbuf() ) != NULL )
		return true; // alread is threadsafe
	oStream.rdbuf( new ThreadSafeStreamBuffer( oStream.rdbuf(), bBufferInternally ) );
	return true;
}

bool VistaStreams::GetStreamWasMadeThreadSafe( std::ostream& oStream )
{
	return ( dynamic_cast<ThreadSafeStreamBuffer*>( oStream.rdbuf() ) != NULL );
}

bool VistaStreams::GetStreamWasMadeThreadSafe( std::ostream* pStream )
{
	return ( dynamic_cast<ThreadSafeStreamBuffer*>( pStream->rdbuf() ) != NULL );
}


VISTATOOLSAPI bool VistaStreams::MakeStreamThreadSafe( std::ostream* pStream,
										const bool bBufferInternally )
{
	return MakeStreamThreadSafe( *pStream, bBufferInternally );
}


// nResult: -1 = fail, 0 = delay (recursive), 1 = success
std::ostream* CreateStreamFromDescription( const std::string& sDefinition,
								const VistaPropertyList& oConfig,
								const bool bThreadSafeStream,
								const std::list<std::string>& liStreamsToCreate,
								const std::string& sNodename,
								int& nResult,
								const bool bAllowColorStreams )
{
	std::string sCommand;
	std::vector<std::string> vecArguments;

	nResult = -1;
	
	// first, check if it is more complex, i.e. if it has braces
	std::size_t nBraceStart = sDefinition.find( '(' );
	if( nBraceStart != std::string::npos )
	{
		std::size_t nBraceEnd =  sDefinition.rfind( ')' );
		if( nBraceEnd == std::string::npos )
		{
			vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- Could not parse definition \""
						<< sDefinition << "\" - missing \")\"" << std::endl;
			return NULL;
		}
		sCommand = sDefinition.substr( 0, nBraceStart );

		// make sure that we do not split sub-commands
		vecArguments.clear();
		int nInBraceDepth = 0;
		std::size_t nCommandStart = nBraceStart + 1;
		std::size_t nCommandEnd = 0;
		bool bBeforeFirstSign = true;
		for( std::size_t nPos = nBraceStart + 1; nPos != nBraceEnd; ++nPos )
		{
			char cChar = sDefinition[nPos];
			if( cChar == '(' )
				++nInBraceDepth;
			else if( cChar == ')' )
			{
				--nInBraceDepth;
				if( bBeforeFirstSign )
				{
					nCommandStart = nPos;
					bBeforeFirstSign = false;
				}
				else
					nCommandEnd = nPos;
			}			
			else if( nInBraceDepth == 0 && cChar == ',' )
			{
				// we have a split
				if( bBeforeFirstSign == false )
				{
					vecArguments.push_back( sDefinition.substr( nCommandStart, nCommandEnd - nCommandStart + 1 ) );
				}
				bBeforeFirstSign = true;
			}
			else if( cChar != ' ' && cChar != '\t'  )
			{
				//skip whitespaces
				if( bBeforeFirstSign )
				{
					nCommandStart = nPos;
					bBeforeFirstSign = false;
				}
				else
					nCommandEnd = nPos;
					
			}
		}

		if( bBeforeFirstSign == false )
		{
			vecArguments.push_back( sDefinition.substr( nCommandStart, nCommandEnd - nCommandStart + 1 ) );
		}
		
	}
	else
		sCommand = sDefinition;

	std::ostream* pStream = NULL;
	if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "COUT" ) )
	{
		pStream = &std::cout;
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "CLOG" ) )
	{
		pStream = &std::clog;
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "CERR" ) )
	{
		pStream = &std::cerr;
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "NULL" ) )
	{
		pStream = &vstr::GetNullStream();
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "FILE" ) )
	{
		if( vecArguments.empty() )
		{
			vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
						<< "Call to \"FILE\" missing arguments - specify at least filename" << std::endl;
			return NULL;
		}
		std::string sFilename = vecArguments.front();
		std::string sExtension = "log";
		std::size_t nDotPos = sFilename.rfind( '.' );
		if( nDotPos != std::string::npos )
		{
			sExtension = sExtension.substr( nDotPos + 1 );
			sFilename = sFilename.substr( 0, nDotPos );
		}

		if( !sFilename.empty() && *sFilename.begin() == '\"' )
			sFilename.erase( sFilename.begin() );
		if( !sFilename.empty() && *(sFilename.end() - 1 ) == '\"' )
			sFilename.erase( sFilename.end() - 1 );

		bool bAppend = false;
		for( std::vector<std::string>::const_iterator itParam = vecArguments.begin() + 1;
				itParam != vecArguments.end(); ++itParam )
		{
			if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( (*itParam), "ADD_TIME" ) )
			{
				sFilename += "_" + VistaTimeUtils::ConvertToLexicographicDateString( VistaTimeUtils::GetStandardTimer().GetSystemTime() );
			}
			else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( (*itParam), "ADD_NODENAME" ) )
			{
				if( sNodename.empty() == false )
					sFilename += "_" + sNodename;
			}
			else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( (*itParam), "APPEND_TO_FILE" ) )
				bAppend = true;
			else
			{
				vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
						<< "unknown parameter \"" << (*itParam) << "\" to stream \"FILE\"" << std::endl;
			}
		}
		pStream = vstr::GetStreamManager()->CreateNewLogFileStream( sFilename, sExtension, true, false, false, bAppend );
		if( pStream == NULL )
			return NULL;
		if( bThreadSafeStream )
			VistaStreams::MakeStreamThreadSafe( pStream );

	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "PREFIX" ) )
	{
		if( vecArguments.size() < 2 )
		{
			vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
						<< "Call to \"PREFIX\" with invalid arguments - specify at least stream and prefix" << std::endl;
			return NULL;
		}

		std::ostream* pSubStream = CreateStreamFromDescription( vecArguments[0], oConfig, false, liStreamsToCreate, sNodename, nResult, bAllowColorStreams );
		if( pSubStream == NULL )
		{
			if( nResult == 0 )
			{
				return NULL; // delay until the stream is ready
			}
			else
			{
				vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
							<< "\"PREFIX\"-command's substream \"" << vecArguments[0]
							<< "\" could not be created" << std::endl;
				nResult = -1;
				return NULL;
			}
		}
		
		VistaPrefixOutstream* pPrefixStream = new VistaPrefixOutstream( pSubStream );

		std::string sPrefix = vecArguments[1];
		if( !sPrefix.empty() && *sPrefix.begin() == '\"' )
			sPrefix.erase( sPrefix.begin() );
		if( !sPrefix.empty() && *(sPrefix.end() - 1 ) == '\"' )
			sPrefix.erase( sPrefix.end() - 1 );

		for( std::size_t i = 2; i < vecArguments.size(); ++i )
		{
			if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( vecArguments[i], "ADD_NODENAME" ) )
			{
				sPrefix = "[" + sNodename + "]: " + sPrefix;
			}
			else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( vecArguments[i], "ADD_NODENAME" ) )
			{
				pPrefixStream->SetPrefixWithIndent( true );
			}
			else
			{
				vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
						<< "Call to \"PREFIX\" with invalid argument [" << vecArguments[i] << "]" << std::endl;
			}
		}
	
		pPrefixStream->SetPrefixString( sPrefix );

		pStream = pPrefixStream;
		if( bThreadSafeStream )
			VistaStreams::MakeStreamThreadSafe( pStream );

	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "COLOR" ) )
	{	
		if( bAllowColorStreams )
		{
			VistaColorOutstream::CONSOLE_COLOR oTextColor = VistaColorOutstream::CC_DEFAULT;
			VistaColorOutstream::CONSOLE_COLOR oBackgroundColor = VistaColorOutstream::CC_DEFAULT;
			bool bUseErrorStream = false;
			if( vecArguments.size() > 0 )
			{
				oTextColor = VistaColorOutstream::GetConsoleColorFromString( vecArguments[0] );
				if( oTextColor == VistaColorOutstream::CONSOLE_COLOR( ~0 ) )
				{
					vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
							<< "unknown color \"" << vecArguments[0] << "\" to stream \"COLOR\"" << std::endl;
					return NULL;
				}
			}
			if( vecArguments.size() > 1 )
			{
				if( vecArguments.size() == 2 
					&&  VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( vecArguments[1], "USE_ERROR_STREAM" ) )
				{
					bUseErrorStream = true;
				}
				else
				{
					oBackgroundColor = VistaColorOutstream::GetConsoleColorFromString( vecArguments[1] );
					if( oBackgroundColor == VistaColorOutstream::CONSOLE_COLOR( ~0 ) )
					{
						vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
								<< "unknown color \"" << vecArguments[1] << "\" to stream \"COLOR\"" << std::endl;
						return NULL;
					}
					if( vecArguments.size() > 2
						&&  VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( vecArguments[2], "USE_ERROR_STREAM" ) )
					{
						bUseErrorStream = true;
					}
				}
			}
			pStream = new VistaColorOutstream( oTextColor, oBackgroundColor, bUseErrorStream );
		}
		else
		{
			if( vecArguments.size() >= 2 
				&&  VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( vecArguments[1], "USE_ERROR_STREAM" ) )
			{
				pStream = &std::cerr;
			}
			else
			{
				pStream = &std::cout;
			}
				
		}

		if( bThreadSafeStream )
			VistaStreams::MakeStreamThreadSafe( pStream );
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "SPLIT" ) )
	{
		if( vecArguments.empty() )
		{
			vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
						<< "Call to \"SPLIT\" missing arguments - specify at least one stream" << std::endl;
			return NULL;
		}
		// to prevent memory leaks/multiple creations of substreams, we have to first wait till all our strings can be
		// created
		for( std::vector<std::string>::const_iterator itArg = vecArguments.begin();
				itArg != vecArguments.end(); ++itArg )
		{			
			std::list<std::string>::const_iterator itWait = std::find(
							liStreamsToCreate.begin(), liStreamsToCreate.end(), (*itArg) );
			if( itWait != liStreamsToCreate.end() )
			{
				nResult = 0;
				return NULL;	
			}
		}

		VistaSplitOutstream* pSplitStream = new VistaSplitOutstream;
		if( bThreadSafeStream )
			VistaStreams::MakeStreamThreadSafe( pSplitStream );
		for( std::vector<std::string>::const_iterator itArg = vecArguments.begin();
				itArg != vecArguments.end(); ++itArg )
		{		
			std::ostream* pSubStream = CreateStreamFromDescription( (*itArg), oConfig, false, liStreamsToCreate, sNodename, nResult, bAllowColorStreams );
			if( pSubStream == NULL )
			{
				vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
							<< "\"SPLIT\"-command's substream \"" << (*itArg)
							<< "\" could not be created" << std::endl;
			}
			else
			{
				pSplitStream->AddStream( pSubStream );
			}			
		}
		pStream = pSplitStream;
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "BUILDTYPE" ) )
	{
		if( vecArguments.size() != 2 )
		{
			vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
						<< "Call to \"BUILDTYPE\" requires exactly two parameters" << std::endl;
			return NULL;
		}
#ifdef DEBUG
		std::string sBuildTypeName = vecArguments[1];
#else
		std::string sBuildTypeName = vecArguments[0];
#endif
		std::list<std::string>::const_iterator itWait = std::find(
							liStreamsToCreate.begin(), liStreamsToCreate.end(),sBuildTypeName );
		if( itWait != liStreamsToCreate.end() )
		{
			nResult = 0;
			return NULL;
		}
		pStream = CreateStreamFromDescription( sBuildTypeName, oConfig, true, liStreamsToCreate, sNodename, nResult, bAllowColorStreams );
		if( nResult != 1 )
			return NULL;
		
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "OUT" ) )
	{
		std::list<std::string>::const_iterator itWait = std::find(
					liStreamsToCreate.begin(), liStreamsToCreate.end(), "out" );
		if( itWait != liStreamsToCreate.end() )
		{
			nResult = 0;
			return NULL;
		}
		pStream = &vstr::out();
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "WARN" ) )
	{
		std::list<std::string>::const_iterator itWait = std::find(
					liStreamsToCreate.begin(), liStreamsToCreate.end(), "warn" );
		if( itWait != liStreamsToCreate.end() )
		{
			nResult = 0;
			return NULL;
		}
		pStream = &vstr::warn();
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "ERR" ) )
	{
		std::list<std::string>::const_iterator itWait = std::find(
					liStreamsToCreate.begin(), liStreamsToCreate.end(), "err" );
		if( itWait != liStreamsToCreate.end() )
		{
			nResult = 0;
			return NULL;
		}
		pStream = &vstr::err();
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sCommand, "DEBUG" ) )
	{
		std::list<std::string>::const_iterator itWait = std::find(
					liStreamsToCreate.begin(), liStreamsToCreate.end(), "debug" );
		if( itWait != liStreamsToCreate.end() )
		{
			nResult = 0;
			return NULL;
		}
		pStream = &vstr::debug();
	}
	else
	{
		std::list<std::string>::const_iterator itWait = std::find(
					liStreamsToCreate.begin(), liStreamsToCreate.end(), sCommand );
		if( itWait != liStreamsToCreate.end() )
		{
			nResult = 0;
			return NULL;
		}

		if( vstr::GetStreamManager()->GetHasStream( sCommand ) == false )
		{
			vstr::warnp() << "VistaStreams::CreateStreamsFromProplist -- "
						<< "Cannot interprete definition \"" << sDefinition << "\" - "
						<< "unknown command or non-existent stream" << std::endl;
			return NULL;
		}
		pStream = &vstr::Stream( sCommand );		
	}

	nResult = 1;
	return pStream;
}

bool VistaStreams::CreateStreamsFromProplist( const VistaPropertyList& oConfig,
													const bool bAllowColorStreams )
{
	return CreateStreamsFromProplist( oConfig, vstr::GetStreamManager()->GetNodeName() );
}
bool VistaStreams::CreateStreamsFromProplist( const VistaPropertyList& oConfig,
											 const std::string& sNodename,
													const bool bAllowColorStreams )
{
	std::list<std::string> liStreamsToCreate;
	VistaAspectsComparisonStuff::StringCompareObject oCompare( false );

	bool bThreadSafe = oConfig.GetValueOrDefault<bool>( "THREADSAFE", true );


	for( VistaPropertyList::const_iterator itEntry = oConfig.begin();
			itEntry != oConfig.end(); ++itEntry )
	{
		if( oCompare( (*itEntry).first, "THREADSAFE" ) )
			continue;
		if( oCompare( (*itEntry).first, "OUT" )
			|| oCompare( (*itEntry).first, "WARN" )
			|| oCompare( (*itEntry).first, "ERR" )
			|| oCompare( (*itEntry).first, "DEBUG" ) )
		{
			liStreamsToCreate.push_back( VistaConversion::StringToLower( (*itEntry).first ) );
		}
		else
		{
			liStreamsToCreate.push_back( (*itEntry).first );
		}
	}

	int nMaxIterations = (int)oConfig.size() * (int)oConfig.size();
	int nIterations = 0;

	while( liStreamsToCreate.empty() == false )
	{
		if( ++nIterations > nMaxIterations )
		{
			VISTA_THROW( "Recursive Stream Definition!", -1 );
		}

		std::string sStreamName = liStreamsToCreate.front();
		liStreamsToCreate.pop_front();

		int nRes;
		std::ostream* pStream = CreateStreamFromDescription( oConfig.GetValue<std::string>( sStreamName ),
															oConfig, bThreadSafe, liStreamsToCreate, sNodename, nRes, bAllowColorStreams );
		if( nRes == 0 )
		{
			liStreamsToCreate.push_back( sStreamName );
		}
		else if( nRes == 1 )
		{
			vstr::GetStreamManager()->AddStream( sStreamName, *pStream, false, true );
			if( oCompare( sStreamName, "OUT" ) )
				vstr::SetOutStream( pStream );
			else if( oCompare( sStreamName, "WARN" ) )
				vstr::SetWarnStream( pStream );
			else if( oCompare( sStreamName, "ERR" ) )
				vstr::SetErrStream( pStream );
			else if( oCompare( sStreamName, "DEBUG" ) )
				vstr::SetDebugStream( pStream );
		}

	}

	return true;
}

