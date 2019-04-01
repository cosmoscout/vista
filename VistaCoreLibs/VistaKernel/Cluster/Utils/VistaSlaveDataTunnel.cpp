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


#include "VistaSlaveDataTunnel.h"

#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPipeComponent.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataProducer.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaActiveDataProducer.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataSource.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataSink.h>

#include <VistaInterProcComm/DataLaVista/Common/VistaByteBufferPacket.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaNetDataCollector.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaRamQueuePipe.h>

#include <VistaInterProcComm/DataLaVista/Construct/VistaCheckError.h>
#include <VistaInterProcComm/DataLaVista/Construct/VistaPipeConstructionKit.h>

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
class VistaSlaveDataTunnel::DeadEnd : public DLVistaDataSink
{
public:
	DeadEnd(){}
	virtual ~DeadEnd(){}
	//DataLaVista interface
	bool ConsumePacket(IDLVistaDataPacket* p)
	{
		m_pDataInput->RecycleDataPacket(p,this,true);
		return true;
	}
};

class VistaSlaveDataTunnel::DeadProd : public DLVistaDataSource
{
public:
	DeadProd(){}
	virtual ~DeadProd(){}
	IDLVistaDataPacket* CreatePacket()
	{
		return new DLVistaByteBufferPacket(this);
	}
	void FillPacket(IDLVistaDataPacket* p){
		p->SetIsValid(false);
	}
	bool HasPacket() const
	{
		return true; 
	}
	void DeletePacket(IDLVistaDataPacket* p)
	{
		delete p;
	}
};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaSlaveDataTunnel::VistaSlaveDataTunnel(const std::string& strHostName, int iPortNum,
										   int iQueueSize, IDLVistaDataPacket* pProto)
:	m_pUpstreamDeadEnd(new DeadEnd),
	m_pDownstreamDeadEnd(new DeadEnd),
	m_pUpstreamDeadProd(new DeadProd)
{
	//build and configure pipeline
	m_pCollector = new DLVistaNetDataCollector(strHostName, iPortNum, pProto);
	m_pCollector->SetQueueSize(iQueueSize);
	m_pActivator = new DLVistaActiveDataProducer(m_pCollector);
	m_pActivator->SetThreadName("SlaveDataTunnelDSActivator");
	m_pQueue = new DLVistaRamQueuePipe;

	//be defensive about dropped packets
	m_pQueue->SetDoesBlockOnReturnPacket(true);
	
	//connect pipeline
	DLVistaPipeConstructionKit::Join(m_pCollector, m_pQueue);
	//start pumping
	m_pActivator->StartComponent();
}

VistaSlaveDataTunnel::VistaSlaveDataTunnel( VistaConnectionIP* pConnection,
								int iQueueSize, IDLVistaDataPacket* pProto )
:	m_pUpstreamDeadEnd(new DeadEnd),
	m_pDownstreamDeadEnd(new DeadEnd),
	m_pUpstreamDeadProd(new DeadProd)
{
	//build and configure pipeline
	m_pCollector = new DLVistaNetDataCollector( pConnection, pProto );
	m_pCollector->SetQueueSize(iQueueSize);
	m_pActivator = new DLVistaActiveDataProducer(m_pCollector);
	m_pActivator->SetThreadName("SlaveDataTunnelDSActivator");
	m_pQueue = new DLVistaRamQueuePipe;

	//be defensive about dropped packets
	m_pQueue->SetDoesBlockOnReturnPacket(true);
	
	//connect pipeline
	DLVistaPipeConstructionKit::Join(m_pCollector, m_pQueue);
	//start pumping
	m_pActivator->StartComponent();
}

VistaSlaveDataTunnel::~VistaSlaveDataTunnel()
{
	//stop pumping
	m_pActivator->SetStopOnDestruct(false);
	m_pActivator->HaltComponent();

	//m_pActivator->StopComponent(true);

	//disconnect from externals
	this->DisconnectDownstreamInput();
	this->DisconnectDownstreamOutput();
	this->DisconnectUpstreamInput();
	this->DisconnectDownstreamOutput();
	
	//destruct internal objects
	delete m_pUpstreamDeadProd;
	delete m_pDownstreamDeadEnd;
	delete m_pUpstreamDeadEnd;
	delete m_pQueue;
	delete m_pActivator;
	delete m_pCollector;
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaSlaveDataTunnel::ConnectUpstreamInput(IDLVistaPipeComponent* pInput)
{
	if(this->GetUpstreamInput())
		this->DisconnectUpstreamInput();

	this->SetUpstreamInput(pInput);
	//the upstream will be trapped in a dead end
	DLVistaPipeConstructionKit::Join(pInput, m_pUpstreamDeadEnd);
	return true;
}

bool VistaSlaveDataTunnel::ConnectDownstreamInput(IDLVistaPipeComponent* pInput)
{
	if(this->GetDownstreamInput() != NULL)
		this->DisconnectDownstreamInput();

	this->SetDownstreamInput(pInput);
	//the locally incoming downstream will be trapped in a dead end
	DLVistaPipeConstructionKit::Join(pInput, m_pDownstreamDeadEnd);
	return true;
}
bool VistaSlaveDataTunnel::ConnectUpstreamOutput(IDLVistaPipeComponent* pOutput)
{
	if(this->GetUpstreamOutput() != NULL)
		this->DisconnectUpstreamOutput();

	this->SetUpstreamOutput(pOutput);
	//the upstream consumer will be presented with empty byte buffer packets
	DLVistaPipeConstructionKit::Join(m_pUpstreamDeadProd, pOutput);
	return true;
}
bool VistaSlaveDataTunnel::ConnectDownstreamOutput(IDLVistaPipeComponent* pOutput)
{
	if(this->GetDownstreamOutput() != NULL)
		this->DisconnectDownstreamOutput();

	this->SetDownstreamOutput(pOutput);
	//the downstream to the application comes from the queue buffer
	DLVistaPipeConstructionKit::Join(m_pQueue,pOutput);
	return true;
}
bool VistaSlaveDataTunnel::DisconnectUpstreamInput()
{
	/*
	* disconnect inbound from upstream dead end
	*/
	IDLVistaPipeComponent* pIncoming = m_pUpstreamDeadEnd->GetInboundByIndex(0);
	if(pIncoming && pIncoming == this->GetUpstreamInput())
	{
		m_pUpstreamDeadEnd->DetachInputComponent(pIncoming);
		pIncoming->DetachOutputComponent(m_pUpstreamDeadEnd);
		this->SetUpstreamInput(NULL);
		return true;
	}
	return false;
}

bool VistaSlaveDataTunnel::DisconnectUpstreamOutput()
{
	/*
	* disconnect outbound from dead prod
	*/
	IDLVistaPipeComponent* pOutgoing = m_pUpstreamDeadProd->GetOutboundByIndex(0);
	if(pOutgoing && pOutgoing == this->GetUpstreamOutput())
	{
		pOutgoing->DetachInputComponent(m_pUpstreamDeadProd);
		m_pUpstreamDeadProd->DetachOutputComponent(pOutgoing);
		this->SetUpstreamOutput(NULL);
		return true;
	}
	return false;
}

bool VistaSlaveDataTunnel::DisconnectDownstreamInput()
{
	/*
	* disconnect downstream inbound from dead end
	*/
	IDLVistaPipeComponent* pIncoming = m_pDownstreamDeadEnd->GetInboundByIndex(0);
	if(pIncoming && pIncoming == this->GetDownstreamInput())
	{
		m_pDownstreamDeadEnd->DetachInputComponent(pIncoming);
		pIncoming->DetachOutputComponent(m_pDownstreamDeadEnd);
		this->SetDownstreamInput(NULL);
		return true;
	}
	return false;
}

bool VistaSlaveDataTunnel::DisconnectDownstreamOutput()
{
	/*
	* disconnect outbound component from queue
	*/
	IDLVistaPipeComponent* pOutgoing = m_pCollector->GetOutboundByIndex(0);
	if(pOutgoing && pOutgoing == this->GetDownstreamOutput())
	{
		pOutgoing->DetachInputComponent(m_pQueue);
		m_pCollector->DetachOutputComponent(pOutgoing);
		this->SetDownstreamOutput(NULL);
		return true;
	}
	return false;
}

void VistaSlaveDataTunnel::StartTunneling()
{
	m_pActivator->StartComponent();
}


void VistaSlaveDataTunnel::StopTunnelingGently(bool bJoin)
{
	m_pActivator->StopComponentGently(bJoin);
}


void VistaSlaveDataTunnel::HaltTunneling()
{
	m_pActivator->HaltComponent();
}

void VistaSlaveDataTunnel::IndicateTunnelingEnd()
{
	m_pActivator->IndicateProductionEnd();
}

void VistaSlaveDataTunnel::SignalPendingRequest()
{
	m_pQueue->SignalPendingRequest();
}
