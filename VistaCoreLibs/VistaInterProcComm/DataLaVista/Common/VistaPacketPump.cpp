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


#include "VistaPacketPump.h"
#include <VistaInterProcComm/Concurrency/VistaThreadLoop.h>

// inner class: PumpLoop

class VistaPumpLoop : public VistaThreadLoop
{
private:
	IDLVistaPipeComponent *m_pComponent;
	DLVistaPacketPump    *m_pDest;
	int                    m_msecs;
protected:
public:
	VistaPumpLoop(DLVistaPacketPump *pPumpComp);
	~VistaPumpLoop();

	bool LoopBody();

	IDLVistaPipeComponent *GetPumpSourceComponent() const { return m_pComponent; }
	void                   SetPumpSourceComponent(IDLVistaPipeComponent *pComponent) { m_pComponent = pComponent; }

	int                    GetPumpBurst() const { return m_msecs; }
	void                   SetPumpBurst(int msecs) { m_msecs = msecs; }
};


VistaPumpLoop::VistaPumpLoop(DLVistaPacketPump *pComponent)
{
	m_pDest = pComponent;
	m_pComponent = NULL;
	m_msecs = 1;
}

VistaPumpLoop::~VistaPumpLoop()
{
}

bool VistaPumpLoop::LoopBody()
{

	IDLVistaDataPacket *pPacket = m_pComponent->ReturnPacket();
	bool bFlag = false;
	while(m_pComponent && pPacket)
	{
		m_pDest->RecycleDataPacket(pPacket, m_pComponent, false);
		bFlag = true;
		pPacket = m_pComponent->ReturnPacket();
	}

	if(bFlag)
		return false;
	return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DLVistaPacketPump::DLVistaPacketPump(int iPumpBurst)
: IDLVistaPipe(), IDLVistaActiveComponent()
{
	m_pPumpLoop = new VistaPumpLoop(this);
	static_cast<VistaPumpLoop *>(m_pPumpLoop)->SetPumpBurst(iPumpBurst);
}

DLVistaPacketPump::~DLVistaPacketPump()
{
	delete m_pPumpLoop;
}

bool DLVistaPacketPump::StartComponent() 
{
	return m_pPumpLoop->Run();
}

bool DLVistaPacketPump::StopComponentGently(bool bJoin) 
{
	return m_pPumpLoop->StopGently(bJoin);
}

bool DLVistaPacketPump::PauseComponent(bool bJoin) 
{
	return m_pPumpLoop->PauseThread(bJoin);
}

bool DLVistaPacketPump::UnPauseComponent(bool bJoin) 
{
	return m_pPumpLoop->UnpauseThread();
}


bool DLVistaPacketPump::StopComponent(bool bJoin) 
{
	return m_pPumpLoop->Suspend();
}

bool DLVistaPacketPump::HaltComponent() 
{
	return m_pPumpLoop->Abort();
}

bool DLVistaPacketPump::IsComponentRunning() const 
{
	return m_pPumpLoop->IsRunning();
}

bool DLVistaPacketPump::IsFull() const 
{
	return false;
}

bool DLVistaPacketPump::IsEmpty() const 
{
	return false;
}

int DLVistaPacketPump::Capacity() const 
{
	return false;
}

bool DLVistaPacketPump::AttachOutputComponent(IDLVistaPipeComponent * pComp)
{
	
	bool bRet = IDLVistaPipe::AttachOutputComponent(pComp);
	if(bRet)
	{
		// we should not do this while running!
		if(m_pPumpLoop->IsRunning())
		{
			m_pPumpLoop->PauseThread(true);
			static_cast<VistaPumpLoop *>(m_pPumpLoop)->SetPumpSourceComponent(pComp);
			m_pPumpLoop->UnpauseThread();
		}
		else
			static_cast<VistaPumpLoop *>(m_pPumpLoop)->SetPumpSourceComponent(pComp);

		return true;
	}
	return false;
}

bool DLVistaPacketPump::DetachOutputComponent(IDLVistaPipeComponent * pComp)
{
	bool bRet = IDLVistaPipe::DetachOutputComponent(pComp);

	if(bRet && m_pPumpLoop->IsRunning())
	{
		m_pPumpLoop->PauseThread(true);
		static_cast<VistaPumpLoop *>(m_pPumpLoop)->SetPumpSourceComponent(NULL);
		m_pPumpLoop->UnpauseThread();
	}

	return bRet;
}

bool DLVistaPacketPump::AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock)
{
	return m_pOutput->AcceptDataPacket(pPacket, this, bBlock);
}

bool DLVistaPacketPump::RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock)
{
	return m_pInput->RecycleDataPacket(pPacket, this, bBlock);
}

IDLVistaDataPacket * DLVistaPacketPump::GivePacket(bool bBlock)
{
	// forward this request
	return this->m_pOutput->GivePacket(bBlock);
}

IDLVistaDataPacket *DLVistaPacketPump::ReturnPacket()
{
	// forward this request!
	return m_pOutput->ReturnPacket();
}

bool DLVistaPacketPump::InitPacketMgmt() 
{
	return true;
}


