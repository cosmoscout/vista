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


#ifndef _VISTACONNECTIONFILE_H
#define _VISTACONNECTIONFILE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <vector>

#include "VistaConnection.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaConnectionFile : public VistaConnection
{
public:
	VistaConnectionFile( const std::string& sFilename, int iMode ) ;
	virtual ~VistaConnectionFile();

	bool Open();
	void Close() ;

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
	int Send    ( const void * buffer, const int length );

	bool Flush();

	HANDLE GetConnectionDescriptor() const;
	HANDLE GetConnectionWaitForDescriptor();

	unsigned long PendingDataSize() const ;
	bool HasPendingData() const ;

	VistaType::uint64 GetFileSize () const;

	enum MODES {
		READ,
		WRITE,
		READWRITE,
		APPEND
	};
	
	virtual bool GetIsBuffering() const;
	virtual void SetIsBuffering( bool bBuffering );

private:
	std::string  m_sFilename;
	int          m_iMode;
	FILE        *m_sStream;
	bool		 m_bBuffering;

protected:

	VistaConnectionFile();


};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif

