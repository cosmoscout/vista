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

#include "VistaActiveDataConsumer.h"
#include <VistaInterProcComm/Concurrency/VistaThreadLoop.h>
#include <VistaInterProcComm/Concurrency/VistaPriority.h>

#include <iostream>
using namespace std;


#if defined(VISTA_IPC_USE_EXCEPTIONS)
#include <VistaBase/VistaExceptionBase.h>
#endif


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

class DLVistaActiveConsumerLoop : public VistaThreadLoop
{
private:
	DLVistaActiveDataConsumer *m_pConsumer;
protected:
public:
	DLVistaActiveConsumerLoop(DLVistaActiveDataConsumer *pConsumer)
	{
		m_pConsumer = pConsumer;
	}

	~DLVistaActiveConsumerLoop() {}

	bool LoopBody();
};

bool DLVistaActiveConsumerLoop::LoopBody()
{
#if defined(VISTA_IPC_USE_EXCEPTIONS)
	try
	{
#endif
		if(!m_pConsumer->PullPacket(true))
		{
		}
#if defined(VISTA_IPC_USE_EXCEPTIONS)
	}
	// we will not catch any exception, only those we know
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		// processing does not really make sense now, does it?
		IndicateLoopEnd();
	} 
#endif
	return false;
}

DLVistaActiveDataConsumer::DLVistaActiveDataConsumer(IDLVistaDataConsumer *pConsumer )
: IDLVistaDataConsumer()
{
	m_pRealConsumer = pConsumer;
	m_pThreadLoop = new DLVistaActiveConsumerLoop(this);
}

DLVistaActiveDataConsumer::~DLVistaActiveDataConsumer()
{
	delete m_pThreadLoop;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


bool DLVistaActiveDataConsumer::StartComponent()
{
	//printf("DLVistaActiveConsumer::StartComponent()\n");
	return m_pThreadLoop->Run();
}

bool DLVistaActiveDataConsumer::PauseComponent(bool bJoin)
{
	//printf("DLVistaActiveDataConsumer::PauseComponent(%d)\n", bJoin);
	return m_pThreadLoop->PauseThread(bJoin);
}

bool DLVistaActiveDataConsumer::UnPauseComponent(bool bJoin)
{
	m_pThreadLoop->UnpauseThread();
	return true;
}


bool DLVistaActiveDataConsumer::StopComponent(bool bJoin)
{
	m_pThreadLoop->IndicateLoopEnd();
	m_pThreadLoop->Join(); 

	return true;
}

bool DLVistaActiveDataConsumer::HaltComponent()
{
	//cout << "DLVistaActiveDataConsumer::HaltComponent()" << std::endl;
	return m_pThreadLoop->Abort();
}

bool DLVistaActiveDataConsumer::StopComponentGently(bool bJoin)
{
	return m_pThreadLoop->StopGently(bJoin);
}

void DLVistaActiveDataConsumer::IndicateConsumptionEnd()
{
	m_pThreadLoop->IndicateLoopEnd();
}


bool DLVistaActiveDataConsumer::IsComponentRunning() const
{
	//cout << "DLVistaActiveDataConsumer::IsComponentRunning()" << std::endl;
	return m_pThreadLoop->IsRunning();
}

bool DLVistaActiveDataConsumer::IsComponentPausing() const
{
	//cout << "DLVistaActiveDataConsumer::IsComponentPausing()" << std::endl;
	return m_pThreadLoop->IsPausing();
}

bool DLVistaActiveDataConsumer::ConsumePacket(IDLVistaDataPacket *pPacket)
{
	return m_pRealConsumer->ConsumePacket(pPacket);
}

bool DLVistaActiveDataConsumer::PullPacket(bool bBlock)
{
	return m_pRealConsumer->PullPacket(bBlock);
}


bool DLVistaActiveDataConsumer::AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock)
{
	return m_pRealConsumer->AcceptDataPacket(pPacket, pSender, bBlock);
}

bool DLVistaActiveDataConsumer::RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock) 
{
	return m_pRealConsumer->RecycleDataPacket(pPacket, pSender, bBlock);
}


bool DLVistaActiveDataConsumer::IsDataProducer() const
{
	return m_pRealConsumer->IsDataProducer();
}


bool DLVistaActiveDataConsumer::IsDataConsumer() const
{
	return m_pRealConsumer->IsDataConsumer();
}


bool DLVistaActiveDataConsumer::IsOutputComponent(IDLVistaPipeComponent *pComp) const
{
	return m_pRealConsumer->IsOutputComponent(pComp);
}

bool DLVistaActiveDataConsumer::IsInputComponent(IDLVistaPipeComponent *pComp) const
{
	return m_pRealConsumer->IsInputComponent(pComp);
}


bool DLVistaActiveDataConsumer::AttachInputComponent(IDLVistaPipeComponent *pComp)
{
	return m_pRealConsumer->AttachInputComponent(pComp);
}


bool DLVistaActiveDataConsumer::AttachOutputComponent(IDLVistaPipeComponent *pComp)
{
	return m_pRealConsumer->AttachOutputComponent(pComp);
}


bool DLVistaActiveDataConsumer::DetachInputComponent(IDLVistaPipeComponent *pComp)
{
	return m_pRealConsumer->DetachInputComponent(pComp);
}


bool DLVistaActiveDataConsumer::DetachOutputComponent(IDLVistaPipeComponent *pComp)
{
	return m_pRealConsumer->DetachOutputComponent(pComp);
}


int DLVistaActiveDataConsumer::GetInputPacketType() const
{
	return m_pRealConsumer->GetInputPacketType();
}

int DLVistaActiveDataConsumer::GetOutputPacketType() const
{
	return m_pRealConsumer->GetOutputPacketType();
}


list<IDLVistaPipeComponent *> DLVistaActiveDataConsumer::GetInputComponents() const
{
	return m_pRealConsumer->GetInputComponents();
}

list<IDLVistaPipeComponent *> DLVistaActiveDataConsumer::GetOutputComponents() const
{
	return m_pRealConsumer->GetOutputComponents();
}

IDLVistaDataPacket *DLVistaActiveDataConsumer::GivePacket(bool bBlock) 
{
	return m_pRealConsumer->GivePacket(bBlock);
}

bool DLVistaActiveDataConsumer::InitPacketMgmt()
{
	return m_pRealConsumer->InitPacketMgmt();
}


IDLVistaDataPacket *DLVistaActiveDataConsumer::CreatePacket()
{
	return m_pRealConsumer->CreatePacket();
}


void DLVistaActiveDataConsumer::DeletePacket(IDLVistaDataPacket *pPacket)
{
	m_pRealConsumer->DeletePacket(pPacket);
}

IDLVistaPipeComponent *DLVistaActiveDataConsumer::GetOutboundByIndex(int iIndex) const
{
	return m_pRealConsumer->GetOutboundByIndex(iIndex);
}


IDLVistaPipeComponent *DLVistaActiveDataConsumer::GetInboundByIndex(int iIndex) const
{
	return m_pRealConsumer->GetInboundByIndex(iIndex);
}


int DLVistaActiveDataConsumer::GetNumberOfOutbounds() const
{
	return m_pRealConsumer->GetNumberOfOutbounds();
}

int DLVistaActiveDataConsumer::GetNumberOfInbounds() const
{
	return m_pRealConsumer->GetNumberOfInbounds();
}


int  DLVistaActiveDataConsumer::SetComponentPriority(const VistaPriority &pPrio)
{
	m_pThreadLoop->SetPriority(pPrio);
	return pPrio.GetSystemPriority();
}

void DLVistaActiveDataConsumer::SetThreadName(const std::string& strName)
{
	m_pThreadLoop->SetThreadName(strName);
}

std::string DLVistaActiveDataConsumer::GetThreadName() const
{
	return m_pThreadLoop->GetThreadName();
}

