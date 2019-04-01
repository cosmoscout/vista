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


#include "VistaStandaloneDataTunnel.h"

#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPipeComponent.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataProducer.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataConsumer.h>

#include <VistaInterProcComm/DataLaVista/Construct/VistaCheckError.h>
#include <VistaInterProcComm/DataLaVista/Construct/VistaPipeConstructionKit.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaStandaloneDataTunnel::VistaStandaloneDataTunnel()
{
}

VistaStandaloneDataTunnel::~VistaStandaloneDataTunnel()
{
	this->DisconnectDownstreamInput();
	this->DisconnectDownstreamOutput();
	this->DisconnectUpstreamInput();
	this->DisconnectDownstreamOutput();
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaStandaloneDataTunnel::ConnectUpstreamInput(IDLVistaPipeComponent* pInput)
{
	if(this->GetUpstreamInput() != NULL)
		this->DisconnectUpstreamInput();

	this->SetUpstreamInput(pInput);
	if(this->GetUpstreamOutput())
	{
		//connect to consumer
		DLVistaPipeConstructionKit::Join(pInput, this->GetUpstreamOutput());
	}
	return true;
}

bool VistaStandaloneDataTunnel::ConnectDownstreamInput(IDLVistaPipeComponent* pInput)
{
	if(this->GetDownstreamInput() != NULL)
		this->DisconnectDownstreamInput();

	this->SetDownstreamInput(pInput);
	if(this->GetDownstreamOutput())
	{
		DLVistaPipeConstructionKit::Join(pInput, this->GetDownstreamOutput());
	}
	return true;
}

bool VistaStandaloneDataTunnel::ConnectUpstreamOutput(IDLVistaPipeComponent* pOutput)
{
	if(this->GetUpstreamOutput() != NULL)
		this->DisconnectUpstreamOutput();

	this->SetUpstreamOutput(pOutput);
	if(this->GetUpstreamInput())
	{
		DLVistaPipeConstructionKit::Join(this->GetUpstreamInput(), pOutput);
	}
	return true;
}


bool VistaStandaloneDataTunnel::ConnectDownstreamOutput(IDLVistaPipeComponent* pOutput)
{
	if(this->GetDownstreamOutput() != NULL)
		this->DisconnectDownstreamOutput();

	this->SetDownstreamOutput(pOutput);
	if(this->GetDownstreamInput())
	{
		DLVistaPipeConstructionKit::Join(this->GetDownstreamInput(), pOutput);
	}
	return true;
}

bool VistaStandaloneDataTunnel::DisconnectUpstreamInput()
{
	/*
	Since the upstream output is directly connected to the producing pipe, 
	this means disconnect the incoming from our upstream consumer
	--- I know, this looks kinda stupid, but we need something like this 
		to hide the internal data tunnel pipe in the cluster case...
	*/
	IDLVistaPipeComponent* pOutput = this->GetUpstreamOutput();
	if(pOutput)
	{
		IDLVistaPipeComponent* pIncoming = pOutput->GetInboundByIndex(0);
		if(pIncoming)
		{
			pOutput->DetachInputComponent(pIncoming);
			pIncoming->DetachOutputComponent(pOutput);
			this->SetUpstreamInput(NULL);
			return true;
		}
	}
	return false;
}
bool VistaStandaloneDataTunnel::DisconnectUpstreamOutput()
{
	/*
	Since the upstream input is directly connected to the consuming pipe, 
	this means disconnect the outgoing from our upstream producer
	*/
	IDLVistaPipeComponent* pInput = this->GetUpstreamInput();
	if(pInput)
	{
		IDLVistaPipeComponent* pOutgoing = pInput->GetOutboundByIndex(0);
		if(pOutgoing)
		{
			pInput->DetachOutputComponent(pOutgoing);
			pOutgoing->DetachInputComponent(pInput);
			this->SetUpstreamOutput(NULL);
			return true;
		}
	}
	return false;
}
bool VistaStandaloneDataTunnel::DisconnectDownstreamInput()
{
	/*
	Since the downstream consumer is directly connected to the producer, 
	this means disconnect any incomings from our upstream consumer
	*/
	IDLVistaPipeComponent* pOutput = this->GetDownstreamOutput();
	if(pOutput)
	{
		IDLVistaPipeComponent* pIncoming = pOutput->GetInboundByIndex(0);
		if(pIncoming)
		{
			pOutput->DetachInputComponent(pIncoming);
			pIncoming->DetachOutputComponent(pOutput);
			this->SetDownstreamInput(NULL);
			return true;
		}
	}
	return false;
}
bool VistaStandaloneDataTunnel::DisconnectDownstreamOutput()
{
	/*
	Since the downstream producer is directly connected to the consuming pipe, 
	this means disconnect the outgoing from our upstream producer
	*/
	IDLVistaPipeComponent* pInput = this->GetDownstreamInput();
	if(pInput)
	{
		IDLVistaPipeComponent* pOutgoing = pInput->GetOutboundByIndex(0);
		if(pOutgoing)
		{
			pInput->DetachOutputComponent(pOutgoing);
			pOutgoing->DetachInputComponent(pInput);
			this->SetDownstreamOutput(NULL);
			return true;
		}
	}
	return false;
}
