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


#ifndef _VISTACONNECTIONNAMEDPIPE_H__
#define _VISTACONNECTIONNAMEDPIPE_H__

#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <string>


class VISTAINTERPROCCOMMAPI VistaConnectionNamedPipe : public VistaConnection
{
public:
	VistaConnectionNamedPipe( const std::string& sName, const bool bIsServer );
	~VistaConnectionNamedPipe();
		
	virtual bool Open();
	virtual void Close();

	std::string GetPipeName() const;

	virtual bool HasPendingData() const;
	virtual unsigned long WaitForIncomingData( int nTimeout = 0 );
	virtual unsigned long PendingDataSize() const;
	
protected:
	virtual int Send( const void* pBuffer, const int nLength );
	virtual int Receive( void* pBuffer, const int nLength, int iTimeout = 0 );

private:
	const std::string m_sPipeName;
	const bool m_bIsServer;

#if !defined(WIN32)
	int m_nReadFifo;
	int m_nWriteFifo;
#else
	HANDLE m_oPipe;
#endif
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif  //_VISTACONNECTIONPIPE_H__
