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


#ifndef _VISTACONNECTIONPULSED_H_
#define _VISTACONNECTIONPULSED_H_


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/Connections/VistaConnection.h>
#include <VistaInterProcComm/Concurrency/VistaTicker.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaConnectionPulsed : public VistaConnection
{
public:
	enum SubConnectionDeleteBehavior
	{
		DELETE_SUB_CONNECTION=0,
		DO_NOT_DELETE_SUB_CONNECTION
	};

	VistaConnectionPulsed( VistaConnection *main_connection, unsigned int pulse_distance_in_ms, SubConnectionDeleteBehavior sbB = DELETE_SUB_CONNECTION );
	virtual ~VistaConnectionPulsed();

	virtual bool Open();
	virtual void Close();

	virtual int Receive( void* pBuffer, const int nLength, int iTimeout = 0 );

	virtual int Send( const void* pBuffer, const int nLength );


	virtual bool HasPendingData() const;
	virtual unsigned long WaitForIncomingData(int timeout=0);
	virtual unsigned long WaitForSendFinish(int timeout=0);
	virtual unsigned long PendingDataSize() const;

	virtual bool GetIsBlocking() const;
	virtual bool GetIsBuffering() const;

	virtual void SetIsBlocking( bool bBlocking );
	virtual void SetIsBuffering( bool bBuffering );

	virtual int  GetReadTimeout() const;
	virtual bool SetReadTimeout( int nReadTimeout );

	virtual bool Flush();

	virtual bool GetIsFine() const;


	virtual HANDLE GetConnectionDescriptor() const;
	virtual HANDLE GetConnectionWaitForDescriptor();
private:
	VistaConnection  *m_main_connection;
	VistaTicker      *m_ticker;
	VistaTicker::TriggerContext *m_context;

	SubConnectionDeleteBehavior m_sub_con_del_behavior;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTACONNECTIONPULSED_H_


