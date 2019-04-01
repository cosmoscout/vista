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


/**
 * Description:
 * This is the DebuggingToolsDemo, showing the usage of VistaStackWalker,
 * VistaStreams+VistaStreamUtils, and VistaBasicProfiler
 * All the interesting stuff is in DebuggingToolsDemo.cpp
 */


#include "DebuggingToolsDemo.h"

#include <VistaBase/VistaExceptionBase.h>

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int	main( int argc, char *argv[] )
{
	try
	{
		DebuggingToolsDemoAppl oDemoAppl( argc, argv );
		// start application as an endless loop
		oDemoAppl.Run();
	}
	catch( VistaExceptionBase &e )
	{
		e.PrintException();
	}
	catch( std::exception &e )
	{
		std::cerr << "Exception:" << e.what() << std::endl;
	}

	return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "main.cpp"                                                    */
/*============================================================================*/
