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
/* StackWalker.h                                                              */
/*	Author:                                                                   */
/*		Jochen Kalmbach, Germany                                              */
/*    (c) 2002-2005 (Freeware)                                                */
/*    http://www.codeproject.com/KB/threads/StackWalker.aspx                  */
/*                                                                            */
/*============================================================================*/


#ifndef _VISTASTACKWALKER_H
#define _VISTASTACKWALKER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaToolsConfig.h"

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

class VISTATOOLSAPI VistaStackWalker
{
public:
	//This is the struct all entries get stored in.
	typedef struct CallStackEntry
	{
		std::string m_sFilename;
		unsigned int m_iLineNumber;
		std::string m_sFunctionName;
	} CallStackEntry;

	typedef std::vector<CallStackEntry> CallStack;
	
	// Retrieves the Callstack
	static bool GetCallstack( CallStack& vecCallStack );

	//Output Functions. Basically these functions print out output. NOTE: These functions are Static!
	static bool PrintCallStack( std::ostream& oOut = std::cout );
	static bool PrintCallStack( const CallStack& vecCallStack, std::ostream& oOut = std::cout );
	static bool PrintCallStackEntry( const CallStackEntry& oEntry, std::ostream& oOut = std::cout );
};

#endif //_VISTASTACKWALKER_H

