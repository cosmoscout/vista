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


#ifndef _VISTASTREAMS_H
#define _VISTASTREAMS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaToolsConfig.h"

#include <VistaBase/VistaStreamUtils.h>

#include <string>
#include <ostream>
#include <iostream>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaPropertyList;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

namespace VistaStreams
{
	/**
	 * Makes a passed stream thread-safe by changing its streambuffer.
	 * This allows concurrent access to the stream, but ONLY for logging, i.e.
	 * one can only write to its back, but neither read nor position the stream.
	 * If bBufferInternally is true, the stream also buffers all input internally
	 * until the stream is flushed. This ensures that messages are printed as
	 * a whole (not interrupted by output from concurrent writes), but only prints
	 * if the stream is flushed - if no flush is called, the input will never
	 * be printed!
	 */
	VISTATOOLSAPI bool MakeStreamThreadSafe( std::ostream& oStream,
										const bool bBufferInternally = true );
	VISTATOOLSAPI bool MakeStreamThreadSafe( std::ostream* pStream,
										const bool bBufferInternally = true );
	VISTATOOLSAPI bool GetStreamWasMadeThreadSafe( std::ostream& oStream );
	VISTATOOLSAPI bool GetStreamWasMadeThreadSafe( std::ostream* pStream );

	/**
	 * 
	 * may get a little complex syntax
	 * Syntax: 
	 * THREADSAFE = TRUE | FALSE			determines wether or not default streams should be threadsafe (default TRUE)
	 * OUT | WARN | ERR | DEBUG | customname = 
	 *			COUT | CLOG | CERR			outputs to standard output stream
	 *			stream					    outputs to another stream (defined in this section or already registered with stream manager)
	 *			NULL						outputs nothing
	 *			FILE( filename, [ADD_TIME], [ADD_NODENAME], [APPEND_TO_FILE] )	outputs to a file
	 *			COLOR( textcolor, [backgroundcolor], [USE_ERROR_STREAM] )	outputs to a color stream
	 *			SPLIT( stream1, stream2 ... )	outputs to all streams in the list
	 *          BUILDTYPE( releasestream, debugstream )	outputs to where stream1 is used in release mode and stream2 in debug	 	 
	 *          PREFIX( stream, prefix, [INDENT], [ADD_NODENAME] )
	 *										for each newline written to PREFIX, the prefix string is written to (minus optional surrounding "")
	 *										if ADD_NODENAME is set, a nodename (formatted '[nodename]: ' ) ist prepended to the prefix
	 *										if INDENT is set, the vtsr::indent is preprended
	 * may be used recursive, e.g. BUILDTYPE( PREFIX( COLOR( GREEN ), "GreenStream - " ), NULL )
	 */
	VISTATOOLSAPI bool CreateStreamsFromProplist( const VistaPropertyList& oConfig,
													const bool bAllowColorStreams = true );
	VISTATOOLSAPI bool CreateStreamsFromProplist( const VistaPropertyList& oConfig,
													const std::string& sNodename,
													const bool bAllowColorStreams = true );
}

/**
 * class VistaColorOutstream: allows to color console streams
 */
class VISTATOOLSAPI VistaColorOutstream : public std::ostream
{
public:
	enum CONSOLE_COLOR
	{
		CC_FIRST_COLOR = 0,
		CC_BLACK = 0,
		CC_DARK_GRAY,
		CC_LIGHT_GRAY,
		CC_WHITE,
		CC_RED,
		CC_DARK_RED,
		CC_GREEN,
		CC_DARK_GREEN,
		CC_BLUE,	
		CC_DARK_BLUE,
		CC_YELLOW,
		CC_DARK_YELLOW,
		CC_MAGENTA,
		CC_DARK_MAGENTA,
		CC_CYAN,	
		CC_DARK_CYAN,
		CC_DEFAULT,
		CC_NUM_COLORS,
	};
	static std::string GetConsoleColorName( const CONSOLE_COLOR oColor );
	static CONSOLE_COLOR GetConsoleColorFromString( const std::string& sName );

public:
	VistaColorOutstream( const CONSOLE_COLOR iTextColor = CC_DEFAULT,
						const CONSOLE_COLOR iBackgroundColor = CC_DEFAULT,
						const bool bUseErrorStream = false,
						const size_t iBufferSize = 50 );
	VistaColorOutstream( const VistaColorOutstream& oCopy );
	virtual ~VistaColorOutstream();

	void SetTextColor( const CONSOLE_COLOR iColor );
	CONSOLE_COLOR GetTextColor() const;
	void SetBackgroundColor( const CONSOLE_COLOR iColor );
	CONSOLE_COLOR GetBackgroundColor() const;
private:
	class Buffer;
	Buffer* m_pBuffer;
};

/**
 * class VistaSplitStream: allows to simultaneously write to multiple streams
 * Attention: When writing to multiple streams with the same target (e.g. multiple
 * console streams), the output may be scrambled due to different buffering schemes.
 */
class VISTATOOLSAPI VistaSplitOutstream : public std::ostream
{		
public:
	VistaSplitOutstream();
	VistaSplitOutstream( const VistaSplitOutstream& oCopy );
	virtual ~VistaSplitOutstream();

	/**
	 * Adds a stream, but does NOT test if the stream is already appended
	 * Thus, it is possible to write to the same stream twice 
	 * (might be somehow useful for whatever reason...)
	 */
	bool AddStream( std::ostream* oStream );
	bool RemoveStream( std::ostream* oStream );
	bool GetHasStream( std::ostream* oStream ) const;
	const std::vector<std::ostream*>& GetStreams() const;
	void SetStreams( const std::vector<std::ostream*>& vecStreams );
private:
	class Buffer;
	Buffer* m_pBuffer;
};

/**
 * class VistaPrefixOutstream: prefixed every line of the output
 * with the specified prefix and optionally an indent
 */
class VISTATOOLSAPI VistaPrefixOutstream : public std::ostream
{		
public:
	VistaPrefixOutstream( std::ostream* oOriginalStream );
	VistaPrefixOutstream( const VistaPrefixOutstream& oCopy );
	virtual ~VistaPrefixOutstream();

	bool GetPrefixWithIndent() const;
	void SetPrefixWithIndent( const bool bSet );

	std::string GetPrefixString() const;
	void SetPrefixString( const std::string& sPrefix );

	std::ostream* GetOriginalStream() const;
	void SetOriginalStream( std::ostream* pStream );
protected:
	class Buffer;
	VistaPrefixOutstream( Buffer* pBuffer );
private:	
	Buffer* m_pBuffer;
};

/**
 * class VistaCallbackPrefixOutstream extends VistaPrefixOutstream by also adding
 * a string retireved from a callback ( added before the statically specified prefix)
 * Note that this amy make the stream potentially slow (not profiled, TODO)
 */
class VISTATOOLSAPI VistaCallbackPrefixOutstream : public VistaPrefixOutstream
{
public:
	// callbacks
	class ICallback
	{
	public:
		virtual ~ICallback() {}
		virtual std::string GetPrefix() const = 0;
	};
	class StaticFunctionCallback : public ICallback
	{
	public:
		typedef std::string (*CallbackFunction)( void );
		
		StaticFunctionCallback( CallbackFunction pfCallback ) : m_pfCallback( pfCallback ) {};
		virtual std::string GetPrefix() const
		{
			return (*m_pfCallback)();
		}
	private:
		CallbackFunction m_pfCallback;
	};
	template< typename T >
	class MemberFunctionCallback : public ICallback
	{
	public:
		typedef std::string (T::*CallbackFunction)( void ) const;
		
		MemberFunctionCallback( T* pObject, CallbackFunction pfCallback ) : m_pObject( pObject ), m_pfCallback( pfCallback ) {};
		virtual std::string GetPrefix() const
		{
			return (m_pObject->*m_pfCallback)();
		}
	private:
		T* m_pObject;
		CallbackFunction m_pfCallback;
	};

public:
	VistaCallbackPrefixOutstream( std::ostream* oOriginalStream, ICallback* pCallback, bool bManageCallbackDeletion = false );
	~VistaCallbackPrefixOutstream();

	ICallback* GetCallback() const;
	void SetCallback( ICallback* pfCallback, bool bManageCallbackDeletion = false ); 
protected:
	class Buffer;
	Buffer* m_pCallbackBuffer;
private:
	// uncopyable
	VistaCallbackPrefixOutstream( const VistaCallbackPrefixOutstream& oCopy );
	VistaCallbackPrefixOutstream& operator= ( const VistaCallbackPrefixOutstream& oCopy );
};

/**
 * class VistaNullOutstream: stream that outputs nothing
 * can be used to swallow unwanted output. Note, however, that usually
 * this is not significantly faster than, e.g., writing to a file, since
 * all formating and conversion is still performed.
 */
class VISTATOOLSAPI VistaNullOutstream : public std::ostream
{			
public:
	VistaNullOutstream();
	virtual ~VistaNullOutstream();

	// to have a more performant null stream, we overload all input operators
	// note: these, of course, only work when writing directly into a Nullstream,
	// (which is pretty senseless). If it is casted to an std::ostream, all
	// types will still be converted and only discarded upon flushing them
	template<typename T>
	inline VistaNullOutstream& operator<<( const T& ) { return *this; }
	inline VistaNullOutstream& operator<<( std::ostream& ( *pf )( std::ostream& ) ) { return *this; }
	inline VistaNullOutstream& operator<<( std::ios& ( *pf )( std::ios&)  ) { return *this; }
	inline VistaNullOutstream& operator<<( std::ios_base& ( *pf )( std::ios_base& )) { return *this; }

private:
	class Buffer;
};

#endif // _VISTASTREAMS_H

