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


#ifndef _VISTAMSGCHANNEL_H
#define _VISTAMSGCHANNEL_H

#include <string>


#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaAspects/VistaObserveable.h>

#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaMsg.h>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaConnection;
class VistaMsg;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaMsgChannel : public IVistaObserveable
{
public:
	VistaMsgChannel();
	virtual ~VistaMsgChannel();

	bool WriteGeneralMsg( VistaMsg &);
	bool WriteMsg(int iMethodToken, const VistaPropertyList &rList, VistaMsg::MSG *pAnswer=NULL);
	bool WriteMsg(int iMethodToken, const VistaPropertyList &rList, VistaPropertyList &rAnswer);

	void SetConnection(VistaConnection *pConnection);
	VistaConnection *GetConnection() const;

	bool DisconnectL();

	bool GetIsOperational() const;

	std::string GetLastErrorString() const;

	enum
	{
		MSG_CONNECTED = IVistaObserveable::MSG_LAST,
		MSG_DISCONNECTED,
		MSG_CONNECTIONCHANGED,
		MSG_LAST
	};


protected:
	VistaMsgChannel(VistaMsgChannel&);
private:
	VistaConnection *m_pConnection;
	VistaByteBufferSerializer m_rSer;
	VistaByteBufferDeSerializer m_rDeSer;

	std::string m_sLastError;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

