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



/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/


#include <cstdio>
#include <string>
#include <list>
using namespace std;

#include "VistaPipeChecker.h"
#include "VistaNullError.h"
#include "VistaCheckError.h"
#include <VistaInterProcComm/DataLaVista/Base/VistaRegistration.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaRTC.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaRegistration.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPipeComponent.h>

#if defined(WIN32)
#pragma warning(disable: 4996)
#endif

#define BUFFER_SIZE 4096

enum eErrorNames
{
	EN_BEGINNER=0,
	EN_ENDPOINT,
	EN_CROSSCHECK,
	EN_SETUP

};

static const string cSErrorNames[] =
{
	"Beginner-Error",
	"Endpoint-Error",
	"Cross-Check-Error",
	"General Setup-Error"
};


enum eBeginnerMsgs
{
	BEG_NOPRODUCER=0
};


static const char *ccBeginnerErrorMsgs[] =
{
	"The Beginner @ %X in this pipe is no producer."
};

enum eEndpointMsgs
{
	END_NOCONSUMER=0
};


static const char *ccEndpointErrorMsgs[] = 
{
	"The Endpoint @%X in this pipe is no consumer."
};


enum eCrossCheckMessages
{
	CCHK_NOVALIDINPUT=0,
	CCHK_NOVALIDOUTPUT,
	CCHK_UNITIALIZED_INPUT,
	CCHK_UNITIALIZED_OUTPUT,
	CCHK_PACKET_TYPE_MISMATCH,
	CCHK_PACKET_TYPE_MISMATCH_NO_REG
};


enum eSetupMsgs
{
	NORTC_ERROR
};

static const char *ccSetupErrorMsgs[] =
{
	"No RTC specified. This WILL CRASH."
};


static const char *ccCrossCheckErrors[] =
{
	"Component @ %X is no valid input for outbound component @ %X",
	"Component @ %X is no valid output for inbound component @ %X",
	"The input component @ %X returned -1 as InputPacket-Type. That indicates either an unproper class (forgot to override GetInputPacketType()?) or a non connectable Input-Component (e.g. DataSource, used as DataSink)",
	"The output component @ %X returned -1 as OutputPacket-Type. That indicates either an unproper class (forgot to override GetOutputPacketType()?) or a non connectable Output-Component (e.g. DataSink, used as DataSource)",
	"Packet type mismatch between component @ %X and component @ %X.\n[%X] accepts packages with [%d,%s] as %s\n[%X] delivers packages with [%d,%s] as %s",
	"Packet type mismatch between component @ %X and component @ %X.\n[%X] accepts packages with [%d] as %s\n[%X] accepts packages with [%d] as %s\n(NO REGISTRATION AVAILABLE)"
};


/*============================================================================*/
/* STATICS                                                                    */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/


DLVistaPipeChecker::DLVistaPipeChecker()
{
	m_pPacketRegistration = IDLVistaRegistration::GetRegistrationSingleton();
}

DLVistaPipeChecker::~DLVistaPipeChecker()
{

}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


DLVistaCheckError DLVistaPipeChecker::CheckPipe(IDLVistaPipeComponent *pBeginning, IDLVistaPipeComponent *pEnd) const
{
	char ccBuffer[BUFFER_SIZE];
	// check startpoint

	// first of all check for the existence of a valid RTC!
	IDLVistaRTC *pRTC = IDLVistaRTC::GetRTCSingleton();
	if(!pRTC)
	{
		sprintf(ccBuffer, "%s", ccSetupErrorMsgs[NORTC_ERROR]);
		return DLVistaCheckError(cSErrorNames[EN_SETUP], string(ccBuffer));
	}


	if(!pBeginning->IsDataProducer())
	{
		sprintf(ccBuffer, ccBeginnerErrorMsgs[BEG_NOPRODUCER], pBeginning);

		return DLVistaCheckError(cSErrorNames[EN_BEGINNER], string(ccBuffer));
	}

	if(!pEnd->IsDataConsumer())
	{
		sprintf(ccBuffer, ccEndpointErrorMsgs[END_NOCONSUMER], pEnd);
		return DLVistaCheckError(cSErrorNames[EN_ENDPOINT], string(ccBuffer));
	}


	list<IDLVistaPipeComponent *>::const_iterator it;
	list<IDLVistaPipeComponent *> liList = pBeginning->GetOutputComponents();
	for(it=liList.begin(); it != liList.end(); ++it)
	{
		DLVistaCheckError err = CheckInboundReference(*it);
		if(!err.IsNullError())
		{
		  return err;
		}
	}

	liList = pEnd->GetInputComponents();
	for(it=liList.begin(); it != liList.end(); ++it)
	{
		DLVistaCheckError err = CheckOutboundReference(*it);
		if(!err.IsNullError())
			return err;
	}

	return DLVistaNullError();
}


DLVistaCheckError DLVistaPipeChecker::CheckInboundReference(IDLVistaPipeComponent *pComp) const
{
	list<IDLVistaPipeComponent *>::const_iterator it;
	list<IDLVistaPipeComponent *> liList = pComp->GetOutputComponents();
	for(it=liList.begin(); it != liList.end(); ++it)
	{
		if((*it)->IsInputComponent(pComp) == false)
		{
			char ccBuffer[BUFFER_SIZE];
			sprintf(ccBuffer, ccCrossCheckErrors[CCHK_NOVALIDINPUT], pComp, *it);
			return DLVistaCheckError(cSErrorNames[EN_CROSSCHECK], ccBuffer);
		}
		
		// do we have a valid registry?
		if(m_pPacketRegistration)
		{
			// check for packet integrity between those two parties
			DLVistaCheckError err0 = CheckPacketIntegrity(pComp, *it);
			if(!err0.IsNullError())
				return err0;
		}


		// ok, the inbound reference seems to be valid for this entity, so lets keep on checking
		// recusively dig into this
		DLVistaCheckError err = CheckInboundReference(*it);
		if(!err.IsNullError())
			return err; // we got an error here
	}


	return DLVistaNullError();
}


DLVistaCheckError DLVistaPipeChecker::CheckOutboundReference(IDLVistaPipeComponent *pComp) const
{
	list<IDLVistaPipeComponent *>::const_iterator it;
	list<IDLVistaPipeComponent *> liList = pComp->GetInputComponents();
	for(it=liList.begin(); it != liList.end(); ++it)
	{
		if((*it)->IsOutputComponent(pComp) == false)
		{
			char ccBuffer[BUFFER_SIZE];
			sprintf(ccBuffer, ccCrossCheckErrors[CCHK_NOVALIDOUTPUT], pComp, *it);
			return DLVistaCheckError(cSErrorNames[EN_CROSSCHECK], ccBuffer);
		}
		DLVistaCheckError err = CheckInboundReference(*it);
		if(!err.IsNullError())
			return err;
		
	}

	return DLVistaNullError();
}

DLVistaCheckError DLVistaPipeChecker::CheckPacketIntegrity(IDLVistaPipeComponent *pSender, IDLVistaPipeComponent *pReceiver) const
{
	// now lets check for correct packet-types
	if(pReceiver->GetInputPacketType() != pSender->GetOutputPacketType())
	{
		// we have a mismatch here
		// check if the inbound does accept all packet-types
		if(!(pReceiver->GetInputPacketType()==0))
		{
			// no, this is real trouble
			// check for unitialized methods
			// see whether reveiver can not recieve
			char ccBuffer[BUFFER_SIZE];
			if(pReceiver->GetInputPacketType() == -1)
			{
				sprintf(ccBuffer, ccCrossCheckErrors[CCHK_UNITIALIZED_INPUT], pReceiver);
				return DLVistaCheckError(cSErrorNames[EN_CROSSCHECK], ccBuffer);
			}
			
			// ok, could be that the sendet does not send
			if(pSender->GetOutputPacketType() == -1)
			{
				sprintf(ccBuffer, ccCrossCheckErrors[CCHK_UNITIALIZED_OUTPUT], pSender);
				return DLVistaCheckError(cSErrorNames[EN_CROSSCHECK], ccBuffer);
			}

			// ok, if we are here we have the following situation
			// 1. the receiver expects packets, that are not sent be the sender
			// 2. the receiver does not accept ANY packets
			// 3. the receiver is able to receive
			// 4. the sender is able to send
			// => we have a conflict in packet types, this pipe will crash!


			// see if we can resolve packet-names, for the sake of debugging
			IDLVistaRegistration *pReg = IDLVistaRegistration::GetRegistrationSingleton();
			if(pReg)
			{
				// yes, we might be able to resolve the packet names
				sprintf(ccBuffer, ccCrossCheckErrors[CCHK_PACKET_TYPE_MISMATCH], pReceiver, pSender, 
					pReceiver, pReceiver->GetInputPacketType(), pReg->GetNameForTypeId(pReceiver->GetInputPacketType()).c_str(), "input",
					pSender, pSender->GetOutputPacketType(), pReg->GetNameForTypeId(pSender->GetOutputPacketType()).c_str(), "output");
				return DLVistaCheckError(cSErrorNames[EN_CROSSCHECK], ccBuffer);
			}
			else
			{
				// no, there is no application registry.
				// give the information we have
				sprintf(ccBuffer, ccCrossCheckErrors[CCHK_PACKET_TYPE_MISMATCH_NO_REG], pReceiver, pSender, 
					pReceiver, pReceiver->GetInputPacketType(), "input",
					pSender, pSender->GetOutputPacketType(), "output");
				return DLVistaCheckError(cSErrorNames[EN_CROSSCHECK], ccBuffer);
			}
 
		}
	}

	return DLVistaNullError();
}

