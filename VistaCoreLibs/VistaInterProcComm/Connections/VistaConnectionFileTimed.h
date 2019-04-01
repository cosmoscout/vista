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


#ifndef _VISTACONNECTIONFILETIMED_H
#define _VISTACONNECTIONFILETIMED_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <ctime>
#include "VistaConnectionFile.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VISTAINTERPROCCOMMAPI VistaConnectionFileTimed : public VistaConnectionFile
{
public:
	VistaConnectionFileTimed( const std::string & sFileName,
				   const bool bPulse,
				   const float fFrequency,
				   const bool bIncoming = true ) ;
	virtual ~VistaConnectionFileTimed() {};

	 /**
	 * Receive shall read some information from the connection
	 *
	 * @param   length   maximum length of bytes to be received
	 * @param  buffer   the received information
	 *
	 * @return  number of received bytes
	 */
	int Receive ( void * buffer, const int length, int iTimeout = 0 );

	/**
	 * Send shall write some information to the connection
	 *
	 * @param   buffer   the information which shall be sent
	 *          length   the length of the information to send
	 *
	 * @return  1   if data was completely sent
	 *          0   otherwise
	 */
	int Send    ( const void * buffer, const int length ) ;

private:
	bool         m_bStatusOK;
	bool         m_bIncoming;
	bool         m_bPulse;

	clock_t      m_LastTime;
	float        m_fFrequency;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif

