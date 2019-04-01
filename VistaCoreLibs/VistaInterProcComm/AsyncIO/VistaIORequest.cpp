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


#include "VistaIORequest.h"
#include "VistaIOScheduler.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaIORequest::IVistaIORequest()
{

   m_eReqDir = RD_NONE;
	m_eState = REQ_NONE;
}

IVistaIORequest::~IVistaIORequest()
{
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaIORequest::eRequestDir IVistaIORequest::GetRequestDir() const
{
	return m_eReqDir;
}

IVistaIORequest::eRequestState IVistaIORequest::GetRequestState() const
{
	return m_eState;
}

void IVistaIORequest::SetRequestDir(eRequestDir eDir)
{
	m_eReqDir = eDir;
}

void IVistaIORequest::SetRequestState(eRequestState eState)
{
	m_eState = eState;
}

void IVistaIORequest::PreWork()
{
	SetRequestState(REQ_PENDING);
}

void IVistaIORequest::PostWork()
{
	SetRequestState(REQ_FINISHED);
}

/*
 void IVistaIORequest::DefinedThreadWork()
{
	try
	{
	   int iChannelTicket = (*m_pMultiplexer).GetNextTicket();
		VistaIOMultiplexer::eIODir eDir = VistaIOMultiplexer::MP_IONONE;
		switch(GetRequestDir())
		{
		case RD_IN:
			{
				eDir = VistaIOMultiplexer::MP_IOIN;
				break;
			}
		case RD_OUT:
			{
				eDir = VistaIOMultiplexer::MP_IOOUT;
				break;
			}
		case RD_ERR:
			{
				eDir = VistaIOMultiplexer::MP_IOERR;
				break;
			}
		case RD_INOUT:
		case RD_NONE:
		default:
			break; // we *should* panic here
		}


		(*m_pMultiplexer).AddMultiplexPoint(GetMultiplexHandle(), iChannelTicket, eDir);

		SetRequestState(REQ_PENDING);

		int iRet = iChannelTicket;
		do
		{
			SetRequestState(REQ_PROCESSING);
			// ok
			if(iRet == iChannelTicket)
			{
				if(!PerformRequest() || (GetPerformedSize() == GetRequestSize()))
					// done
					break;
			}
			else
			{
				// ?? 
				break; // leave loop
			}
		 }
		 while((iRet=(*m_pMultiplexer).Demultiplex())!=-1);
	}
	catch(...)
	{
	}

}
*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


