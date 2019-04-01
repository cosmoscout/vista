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


#ifndef _VISTASTREAMUTILS_H
#define _VISTASTREAMUTILS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaBaseConfig.h"

#include "VistaStreamManager.h"

#include <ostream>
#include <iostream>
#include <iomanip>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


namespace vstr
{
	// general outstreams as they are used by Vista
	VISTABASEAPI std::ostream& out();
	VISTABASEAPI std::ostream& warn();
	VISTABASEAPI std::ostream& err();
	VISTABASEAPI std::ostream& debug();

	// outstreams with prefixed vstr::indent
	VISTABASEAPI std::ostream& outi();
	VISTABASEAPI std::ostream& warni();
	VISTABASEAPI std::ostream& erri();
	VISTABASEAPI std::ostream& debugi();

	// outstreams with prefixed vstr::indent and vstr::warnprefix or vstr::errprefix
	VISTABASEAPI std::ostream& warnp();
	VISTABASEAPI std::ostream& errp();

	/**
	 * The following functions set the different standard streams.
	 * Note that, since these may be used concurrently, these streams should be thread-safe
	 * (if in doubt, use VistaTools' function VistaStreams::MakeStreamThreadSafe)
	 * @see VistaStreams::MakeStreamThreadSafe
	 */
	VISTABASEAPI void SetOutStream( std::ostream* oStream );
	VISTABASEAPI void SetWarnStream( std::ostream* oStream  );
	VISTABASEAPI void SetErrStream( std::ostream* oStream  );
	VISTABASEAPI void SetDebugStream( std::ostream* oStream  );

	//////////////////////////////////////////////
	////  STREAM MANAGER
	//////////////////////////////////////////////
	/**
	 *  returns the StreamManager Singleton (and creates one if necessary)
	 */
	VISTABASEAPI VistaStreamManager* GetStreamManager();
	VISTABASEAPI void SetStreamManager( VistaStreamManager* pStreamManager,
											const bool bDeleteOldManager = true );
	VISTABASEAPI void DestroyStreamManager();
	/**
	 * Returns the Stream with the specified name from the StreamManager Singleton;
	 */
	VISTABASEAPI std::ostream& Stream( const std::string& sName = "" );

	//////////////////////////////////////////////
	////  NULL STREAM SINGLETON
	//////////////////////////////////////////////
	VISTABASEAPI std::ostream& GetNullStream();

	//////////////////////////////////////////////
	////  STREAM MANIPULATOR
	//////////////////////////////////////////////	
	/**
	 * stream manipulator that prints the current system time
	 */
	VISTABASEAPI std::ostream& systime( std::ostream& oStream );
	/**
	 * stream manipulator that prints the current mcirotime
	 */
	VISTABASEAPI std::ostream& relativetime( std::ostream& oStream );
	/**
	 * stream manipulator that prints the current frameclock
	 * only works if the StreamManagers InfoInterface was set (prints 0 otherwise)
	 */
	VISTABASEAPI std::ostream& frameclock( std::ostream& oStream );
	/**
	 * stream manipulator that prints the current date, formatted as
	 * YYMMDD_HHMMSS
	 * only works if the StreamManagers InfoInterface was set (prints "" otherwise)
	 */
	VISTABASEAPI std::ostream& date( std::ostream& oStream );
	/**
	 * stream manipulator that displays the number of frames since the
	 * creation of the Logger singleton
	 * only works if the StreamManagers InfoInterface was set (prints 0 otherwise)
	 */
	VISTABASEAPI std::ostream& framecount( std::ostream& oStream );
	/**
	 * stream manipulator that prints general information as defined by
	 * the InfoLayout functionality
	 */
	VISTABASEAPI std::ostream& info( std::ostream& oStream );
	/**
	 * stream manipulator that prints the name of the current cluster node
	 * only works if the StreamManagers InfoInterface was set (returns "" otherwise)
	 */
	VISTABASEAPI std::ostream& nodename( std::ostream& oStream );
	/**
	 * stream manipulator that prints the current framerate
	 * only works if the StreamManagers InfoInterface was set (returns 0 otherwise)
	 */
	VISTABASEAPI std::ostream& framerate( std::ostream& oStream );
	/**
	 * stream manipulators that indents by string that
	 * correspond to the indentation level of the stream manager
	 */
	VISTABASEAPI std::ostream& indent( std::ostream& oStream );
	/**
	 * stream manipulators that indents by string that
	 * correspond to one single indentation level
	 */
	VISTABASEAPI std::ostream& singleindent( std::ostream& oStream );
	/**
	 * stream manipulators that indents by string that
	 * correspond to the indentation prefix
	 */
	VISTABASEAPI std::ostream& indentprefix( std::ostream& oStream );
	/**
	 * stream manipulators that indents by string that
	 * correspond to the indentation postfix
	 */
	VISTABASEAPI std::ostream& indentpostfix( std::ostream& oStream );
	/**
	 * Object that increases the intentation by 1 until it goes out of scope
	 */
	class VISTABASEAPI IndentObject
	{
	public:
		IndentObject();
		~IndentObject();
	};
	/**
	 * stream manipulators that outputs the default warning prefix
	 * that is configurable in the VistaStreamManager
	 */
	VISTABASEAPI std::ostream& warnprefix( std::ostream& oStream );
	/**
	 * stream manipulators that outputs the default error prefix
	 * that is configurable in the VistaStreamManager
	 */
	VISTABASEAPI std::ostream& errprefix( std::ostream& oStream );


	/**
	 * stream manipulator that formats a time appropriately, i.e. by default with 6
	 * decimal places. The stream's formatting is not altered after a call.
	 */
	class VISTABASEAPI formattime
	{
	public:
		formattime( const double dTime, const int fPrecision = 6 );
		std::ostream& operator()( std::ostream& oStream ) const;
	private:
		double m_dTime;
		int	m_nPrecision;
	};
}

inline VISTABASEAPI std::ostream& operator<< ( std::ostream& oStream, const vstr::formattime& oObj )
{
	return oObj( oStream );
}


#endif // _VISTASTREAMUTILS_H

