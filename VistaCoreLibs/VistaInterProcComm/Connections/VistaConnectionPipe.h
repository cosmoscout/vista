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


#ifndef _VISTACONNECTIONPIPE_H__
#define _VISTACONNECTIONPIPE_H__

#include <VistaInterProcComm/Connections/VistaConnection.h>


class VISTAINTERPROCCOMMAPI VistaConnectionPipe : public VistaConnection
{
public:
	VistaConnectionPipe();
	~VistaConnectionPipe();

	bool Open();
	void Close();

	int Receive( void *buffer, const int length, int iTimeout=0 );
	int Send( const void *buffer, const int length );

	bool HasPendingData() const;
	unsigned long PendingDataSize() const;

	HANDLE GetConnectionDescriptor() const;
	HANDLE GetConnectionWaitForDescriptor();

private:
#if !defined(WIN32)
	int m_fd[2];
#else

#endif
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif  //_VISTACONNECTIONPIPE_H__
