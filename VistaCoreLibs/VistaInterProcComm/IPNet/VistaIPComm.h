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


#ifndef _VISTAIPCOMM_H
#define _VISTAIPCOMM_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <string>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is a hack-around class for winsock. Users: please feel free to ignore this
 * class. It will open winsock-dll, and close it iff it is no more needed.
 * There will be reference-counting to indicate that state. Note that this is NOT
 * designed to be thread safe. An instance of this class will be constructed after
 * the programm was loaded AUTOMATICALLY. Do not expect win32-sockets to work prior to
 * the call to main()!
 */
class VISTAINTERPROCCOMMAPI VistaIPComm
{
public:

	/**
	 * This will call UseIPComm
	 */
	VistaIPComm();

	/**
	 * This will call CloseIPComm
	 */
	~VistaIPComm();

	/**
	 * This will open winsock once and then simply increment a reference count after each call.
	 * @return the value of the reference count.
	 */
	static int UseIPComm();

	/**
	 * This will decrease the reference-counter and close winsock iff that count is zero.
	 * @return the value of the reference counter.
	 */
	static int CloseIPComm();

	static std::string GetHostname();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

