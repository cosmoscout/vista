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

#include "VistaActiveFilter.h"
#include <VistaInterProcComm/Concurrency/VistaThreadLoop.h>
#include <VistaInterProcComm/Concurrency/VistaPriority.h>

#include <iostream>
#include <list>
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

class DLVistaFilterLoop : public VistaThreadLoop
{
private:
	DLVistaActiveFilter *m_pTarget;
protected:
public:
	DLVistaFilterLoop(DLVistaActiveFilter *pTarget) { m_pTarget = pTarget; };
	virtual ~DLVistaFilterLoop() {};

	bool LoopBody();
};


bool DLVistaFilterLoop::LoopBody()
{
	// pull packet
	bool bRet = false;
	//IDLVistaDataPacket *p = m_pTarget->PullPacket(false);
	IDLVistaDataPacket *p = m_pTarget->PullPacket(true);
	if(p)
	{
		//printf("DLVistaFilterLoop::LoopBody() -- pull packet @ %x\n", p);
		m_pTarget->AcceptDataPacket(p, m_pTarget);
		//return false;
		bRet = true;
	}
	//for each packet we pull out of our input try to reclaim/return one to him
	p = m_pTarget->ReturnPacket();
	if(p)
	{
		//printf("DLVistaFilterLoop::LoopBody() -- return packet @ %x\n", p);
		m_pTarget->RecycleDataPacket(p, m_pTarget, true);
		bRet = true;
		
	}
	//else
		//printf("DLVistaFilterLoop::LoopBody() -- NULL on return packet.\n");
	return bRet;
}


DLVistaActiveFilter::DLVistaActiveFilter(IDLVistaFilter *pFilter)
{
	this->m_pRealFilter = pFilter;
	this->m_pThread = new DLVistaFilterLoop(this);
	m_bStopOnDestruct = true;
}

DLVistaActiveFilter::~DLVistaActiveFilter()
{
	if(m_bStopOnDestruct)
		m_pThread->StopGently(true);
	delete m_pThread;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


IDLVistaDataPacket *DLVistaActiveFilter::FilterPacketL(IDLVistaDataPacket * pPacket)
{
	//cout << "DLVistaActiveFilter::FilterPacketL("
	//	 << pPacket << ")" << std::endl;

	return m_pRealFilter->FilterPacketL(pPacket);
}

void DLVistaActiveFilter::ConsumePacket(IDLVistaDataPacket * pPacket)
{
	//cout << "DLVistaActiveFilter::ConsumePacket( "
	//	 <<  pPacket << ")" << std::endl;
	m_pRealFilter->ConsumePacket(pPacket);
}

IDLVistaDataPacket *DLVistaActiveFilter::PullPacket(bool bBlock)
{
	//printf("DLVistaActiveFilter::PullPacket(%d)\n", bBlock);
	return m_pRealFilter->PullPacket(bBlock);
}

bool DLVistaActiveFilter::AttachInputComponent(IDLVistaPipeComponent * pComp)
{
	//cout << "DLVistaActiveFilter::AttachInputComponent("
	//	 << pComp << ")" << std::endl;

	return m_pRealFilter->AttachInputComponent(pComp);
}

bool DLVistaActiveFilter::AttachOutputComponent(IDLVistaPipeComponent * pComp)
{
	//cout << "DLVistaActiveFilter::AttachOutputComponent("
	//	 << pComp << ")";

	return m_pRealFilter->AttachOutputComponent(pComp);
}


bool DLVistaActiveFilter::RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock)
{
//	printf("DLVistaActiveFilter::RecycleDataPacket(%X, %d)\n", pPacket, bBlock);
	return m_pRealFilter->RecycleDataPacket(pPacket, this, bBlock);
}

bool DLVistaActiveFilter::AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock)
{
	//printf("DLVistaActiveFilter::AcceptDataPacket(%X, %d)\n", pPacket, bBlock);
	return m_pRealFilter->AcceptDataPacket(pPacket, this, bBlock);
}


bool DLVistaActiveFilter::StartComponent()
{
	//cout << "DLVistaActiveFilter::StartComponent()" << std::endl;
	return m_pThread->Run();
}

bool DLVistaActiveFilter::PauseComponent(bool bJoin)
{
	//cout << "DLVistaActiveFilter::PauseComponent("
	//	 << bJoin << ")";
	return m_pThread->PauseThread(bJoin);
}

bool DLVistaActiveFilter::UnPauseComponent(bool bJoin)
{
	m_pThread->UnpauseThread();
	return true;
}

bool DLVistaActiveFilter::StopComponent(bool bJoin)
{
	//cout << "DLVistaActiveFilter::StopComponent("
	//	 <<  bJoin << ")" << std::endl;
	m_pThread->Suspend();
	if(bJoin)
		m_pThread->Join(); // this could be a deadlock!
	return true;
}

bool DLVistaActiveFilter::HaltComponent()
{
	//cout << "DLVistaActiveFilter::HaltComponent()" << std::endl;
	return m_pThread->Abort();
}

bool DLVistaActiveFilter::StopComponentGently(bool bJoin)
{
	return m_pThread->StopGently(bJoin);
}


void DLVistaActiveFilter::IndicateFilteringEnd()
{
	m_pThread->IndicateLoopEnd();
}

bool DLVistaActiveFilter::IsComponentRunning() const
{
	//cout << "DLVistaActiveFilter::IsComponentRunning()" << std::endl;
  return m_pThread->IsRunning();
}


IDLVistaDataPacket *DLVistaActiveFilter::CreatePacket()
{
	//cout << "DLVistaActiveFilter::CreatePacket()" << std::endl;
	return m_pRealFilter->CreatePacket();
}

void DLVistaActiveFilter::DeletePacket(IDLVistaDataPacket *pPacket)
{
	//cout << "DLVistaActiveFilter::DeletePacket(pPacket)" << std::endl;
	m_pRealFilter->DeletePacket(pPacket);
}

bool DLVistaActiveFilter::InitPacketMgmt()
{
	//printf("DLVistaActiveFilter::InitPacketMgmt()\n");
	return m_pRealFilter->InitPacketMgmt();
}


IDLVistaDataPacket *DLVistaActiveFilter::GivePacket(bool bBlock)
{
	//cout << "DLVistaActiveFilter::GivePacket("
	//	 << bBlock << ")" << std::endl;
	return m_pRealFilter->GivePacket(bBlock);
}


int DLVistaActiveFilter::GetInputPacketType() const
{
	return m_pRealFilter->GetInputPacketType();
}

int DLVistaActiveFilter::GetOutputPacketType() const
{
	return m_pRealFilter->GetOutputPacketType();
}


list<IDLVistaPipeComponent *> DLVistaActiveFilter::GetInputComponents() const
{
	return m_pRealFilter->GetInputComponents();
}


list<IDLVistaPipeComponent *> DLVistaActiveFilter::GetOutputComponents() const
{
	return m_pRealFilter->GetOutputComponents();
}

bool DLVistaActiveFilter::IsInputComponent(IDLVistaPipeComponent *pComp) const
{
	return m_pRealFilter->IsInputComponent(pComp);
}

bool DLVistaActiveFilter::IsOutputComponent(IDLVistaPipeComponent *pComp) const
{
	return m_pRealFilter->IsOutputComponent(pComp);
}

int  DLVistaActiveFilter::SetComponentPriority(const VistaPriority &pPrio)
{
	m_pThread->SetPriority(pPrio);
	return pPrio.GetSystemPriority();
}

bool DLVistaActiveFilter::GetStopOnDestruct() const
{
	return m_bStopOnDestruct;
}

void DLVistaActiveFilter::SetStopOnDestruct(bool bStop)
{
	m_bStopOnDestruct = bStop;
}

IDLVistaDataPacket *DLVistaActiveFilter::ReturnPacket()
{
	if(m_pRealFilter)
		return m_pRealFilter->ReturnPacket();
	return NULL;
}

void DLVistaActiveFilter::SetThreadName(const std::string& strName)
{
	m_pThread->SetThreadName(strName);
}

std::string DLVistaActiveFilter::GetThreadName() const
{
	return m_pThread->GetThreadName();
}


