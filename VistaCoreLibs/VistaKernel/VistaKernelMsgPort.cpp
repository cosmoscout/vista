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


#include "VistaKernelMsgPort.h"

#include <vector>
#include <deque>
#include <string>
using namespace std;

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>

#include <VistaBase/VistaVersion.h>
#include <VistaBase/VistaStreamUtils.h>

#if defined(WIN32)
#include <winsock2.h>
#include <Windows.h>
#endif


#include <VistaInterProcComm/IPNet/VistaTCPServerSocket.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataSource.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataSink.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaActiveDataProducer.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaRamQueuePipe.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/Concurrency/VistaThreadPool.h>
#include <VistaInterProcComm/Concurrency/VistaThread.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaProgressMessage.h>

#include <VistaBase/VistaTimerImp.h>
#include <VistaTools/VistaEnvironment.h>


//#include <VistaKernel/InteractionManager/VistaOldInteractionManager.h>
//#include <VistaKernel/InteractionManager/VistaInputDevice.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include "VistaKernelMsgTab.h"


#include <VistaBase/VistaExceptionBase.h>
#include <VistaInterProcComm/IPNet/VistaAcceptor.h>



/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

// ############################################################################


// #############################################
// ########## VistaKernelMsgPacket ############
// #############################################

VistaKernelMsgPacket::VistaKernelMsgPacket(IDLVistaPipeComponent*p)
: IDLVistaDataPacket(p)
{
	m_pMessage = new VistaMsg;
	m_iPacketNumber=-1;
	m_pSourcePort = NULL;
}

VistaKernelMsgPacket::~VistaKernelMsgPacket()
{
	delete m_pMessage;
}


int VistaKernelMsgPacket::GetPacketNumber() const
{
	return m_iPacketNumber;
}

void VistaKernelMsgPacket::SetPacketNumber(int i)
{
	m_iPacketNumber = i;
}


int VistaKernelMsgPacket::GetPacketSize() const
{
	return sizeof(*m_pMessage);
}

void VistaKernelMsgPacket::SetDataSize(int iSize)
{
}

int VistaKernelMsgPacket::GetDataSize() const
{
	return GetPacketSize();
}

string VistaKernelMsgPacket::GetClName() const
{
	return "VistaKernelMsgPacket";
}

int VistaKernelMsgPacket::Serialize(IVistaSerializer &rSer) const
{
	//int iLength = IDLVistaDataPacket::Serialize(rSer);
	int iLength = 0;
	iLength += (*m_pMessage).Serialize(rSer);
	iLength += rSer.WriteInt32(m_iPacketNumber);
	return iLength;
}

int VistaKernelMsgPacket::DeSerialize(IVistaDeSerializer &rSer)
{
	//int iLength = IDLVistaDataPacket::DeSerialize(rSer);
	int iLength = 0;
	iLength += (*m_pMessage).DeSerialize(rSer);
	iLength += rSer.ReadInt32(m_iPacketNumber);
	return iLength;
}

VistaMsg *VistaKernelMsgPacket::GetPacketMessage() const
{
	return m_pMessage;
}

void        VistaKernelMsgPacket::SetPacketMessage(VistaMsg *pMsg)
{
	(*m_pMessage) = *pMsg;
}

string VistaKernelMsgPacket::GetSignature() const
{
	return "VistaKernelMsgPacket";
}

IDLVistaDataPacket* VistaKernelMsgPacket::CreateInstance(IDLVistaPipeComponent*pProd) const
{
	return new VistaKernelMsgPacket(pProd);
}

VistaKernelMsgPort *VistaKernelMsgPacket::GetSourcePort() const
{
	return m_pSourcePort;
}

void VistaKernelMsgPacket::SetSourcePort(VistaKernelMsgPort *pPort)
{
	m_pSourcePort = pPort;
}

// ############################################################################

// #######################################################

class VistaMsgSource : public DLVistaDataSource
{
public:
	VistaMsgSource();
	~VistaMsgSource();

	void SetIncomingConnection(VistaConnectionIP *pIp);
	VistaConnectionIP *GetIncomingConnection();

	IDLVistaDataPacket *CreatePacket();
	void                DeletePacket(IDLVistaDataPacket *);

	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock = false);

	void FillPacket(IDLVistaDataPacket *);
	bool HasPacket() const;

	bool WaitForNextPacket(int iTimeout) const
	{
		unsigned long uiRet = (*m_pIncomingConnection).WaitForIncomingData(iTimeout);
		if(uiRet == ~0U)
		{
			// timeout
		}
		else if(uiRet == 0)
		{
			VISTA_THROW("Blocking connection with bytes ready 0", 0x00000002);
		}
		return true;
	}

protected:
private:
	VistaConnectionIP *m_pIncomingConnection;
	VistaMutex         m_rIncoming;

	VistaByteBufferDeSerializer m_deSer;
	vector<VistaType::byte> m_vecTmp;
};

class VistaAcceptWork : public IVistaAcceptor
{
public:
	VistaAcceptWork(VistaMsgSource *pSrc,
		DLVistaActiveDataProducer *pActivator,
		const string &sInterfaceName,
		int iPort,
		const string &strIniFileName);
	virtual ~VistaAcceptWork();
protected:

	virtual bool HandleIncomingClient(VistaTCPSocket *pSocket);
	virtual bool HandleAbortMessage()
	{
		return true;
	}
private:
	VistaMsgSource *m_pSource;
	DLVistaActiveDataProducer *m_pActivator;
	string m_strIniFileName;
};

VistaAcceptWork::VistaAcceptWork(VistaMsgSource *pSrc,
								 DLVistaActiveDataProducer *pActivator,
								 const string &sInterfaceName,
								 int iPort,
								 const string &strIniFileName)
								 : IVistaAcceptor(sInterfaceName, iPort),
								 m_pSource(pSrc),
								 m_pActivator(pActivator),
								 m_strIniFileName(strIniFileName)
{
}

VistaAcceptWork::~VistaAcceptWork()
{
}


bool VistaAcceptWork::HandleIncomingClient(VistaTCPSocket *pSocket)
{
	// this should be true!
	VistaConnectionIP *pConnection = NULL;
	try
	{
		pSocket->SetIsBlocking(true);
		pConnection = new VistaConnectionIP(pSocket);

		//pConnection->SetLingerTime(25);
		m_pSource->SetIncomingConnection(pConnection);
		m_pActivator->StartComponent();
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		delete pConnection;
		return false;
	}
	return true;
}


VistaMsgSource::VistaMsgSource()
{
	m_pIncomingConnection = NULL;
	SetQueueSize(100);
}

VistaMsgSource::~VistaMsgSource()
{
	delete m_pIncomingConnection;
}

VistaConnectionIP *VistaMsgSource::GetIncomingConnection()
{
	VistaMutexLock lock(m_rIncoming);
	return m_pIncomingConnection;
}


void VistaMsgSource::SetIncomingConnection(VistaConnectionIP *pIp)
{
	VistaMutexLock lock(m_rIncoming);
	m_pIncomingConnection = pIp;
}

IDLVistaDataPacket *VistaMsgSource::CreatePacket()
{
	return new VistaKernelMsgPacket(this);
}

void VistaMsgSource::DeletePacket(IDLVistaDataPacket *p)
{
	delete p;
}

void VistaMsgSource::FillPacket(IDLVistaDataPacket *pPacket)
{
	// HasPacket() returns false if there is no connection,
	// in that case, we should not reach this point
	// so it is safe to assume that we have a valid connection right now
	
	VistaKernelMsgPacket *pMsg = static_cast<VistaKernelMsgPacket*>(pPacket);

	int iMsgLength = 0;

	int iRet = (*m_pIncomingConnection).WaitForIncomingData();

	if(iRet == 0)
		VISTA_THROW("0 on blocking-socket, connection closed", 0x000000001);

	if(iRet < 0)
		goto error_onRet;
	if(iRet < 4)
	{
#if defined(DEBUG)
		vstr::errp() << "[VistaMsgSource::FillPacket]: should read size (int32) -- but can not read enough bytes ["
			<< iRet << "] possible." << std::endl;
#endif
		goto error_onRet;
	}

	iRet = (*m_pIncomingConnection).ReadInt32(iMsgLength);
	//printf("VistaMsgSource::FillPacket() -- msg lenght is [%d]\n", iMsgLength);


	m_vecTmp.resize(iMsgLength);
	//printf("-- bytes pending: %d\n", m_pIncomingConnection->PendingDataSize());

	m_pIncomingConnection->WaitForIncomingData();
	//unsigned long ulReady = m_pIncomingConnection->WaitForIncomingData();
	//printf("-- bytes pending: %d\n", bReady);

	iRet = (*m_pIncomingConnection).ReadRawBuffer( &m_vecTmp[0], iMsgLength);

	//printf("VistaMsgSource::FillPacket() -- read [%d] bytes from stream\n", iRet);
	if(iRet<0)
		goto error_onRet;


	// SetBuffer calls ClearBuffer!
	m_deSer.SetBuffer( &m_vecTmp[0], iMsgLength );

	if(pMsg->DeSerialize(m_deSer)>0) // read from stream
	{
		// ok, this should have worked.
		pMsg->SetIsValid(true);
		//printf("VistaMsgSource::FillPacket() -- PacketNumber = [%d]\n", pMsg->GetPacketNumber());
		return;
	}

error_onRet:
	pMsg->SetIsValid(false);
	return;
}

bool VistaMsgSource::HasPacket() const
{
	//    return true;
	if(!m_pIncomingConnection)
		return false; // we do not deliver when we do not have a connection
	return m_pIncomingConnection->HasPendingData();
}

bool VistaMsgSource::RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock)
{
	VistaMutexLock lock(m_rIncoming);
	if(m_pIncomingConnection)
	{
		VistaKernelMsgPacket *pMsg = static_cast<VistaKernelMsgPacket*>(pPacket);
		VistaByteBufferSerializer ser;
		pMsg->Serialize(ser);
		//printf("VistaMsgSource::RecycleDataPacket([%d]) -- writing %d bytes\n", pMsg->GetPacketNumber(), ser.GetBufferSize());
		m_pIncomingConnection->WriteInt32(ser.GetBufferSize());
		m_pIncomingConnection->WriteRawBuffer( ser.GetBuffer(), ser.GetBufferSize());

		m_pIncomingConnection->WaitForSendFinish();
		lock.Unlock(); // release
	}
	return DLVistaDataSource::RecycleDataPacket(pPacket, pSender, bBlock);
}

// ############################################
// ############################################

class VistaMsgSink : public DLVistaDataSink
{
public:
	VistaMsgSink();
	~VistaMsgSink();

	bool ConsumePacket(IDLVistaDataPacket *p);
	VistaKernelMsgPacket *GetCurrentPacket();
	void                   AnswerQuery(VistaKernelMsgPacket *);
	void                   PutQuery(VistaMsg *pMsg);
protected:
private:
	VistaKernelMsgPacket *m_pCurrentPacket;
};

VistaMsgSink::VistaMsgSink()
{
	m_pCurrentPacket = NULL;
}

VistaMsgSink::~VistaMsgSink()
{
}

bool VistaMsgSink::ConsumePacket(IDLVistaDataPacket *pPacket)
{
	VistaKernelMsgPacket *pPack = static_cast<VistaKernelMsgPacket*>(pPacket);
	m_pCurrentPacket = pPack; // set new value
	//printf("VistaMsgSink::ConsumePacket()\n");
	return true;
}

void VistaMsgSink::PutQuery(VistaMsg *pMsg)
{
	// we are a sink, so we do not "put" a msg
	// but, what we can do is: order a free packet
	VistaKernelMsgPacket *pPack = static_cast<VistaKernelMsgPacket *>(m_pDataInput->GivePacket(true));
	if(pPack)
	{
		pPack->SetPacketMessage(pMsg);
		m_pDataInput->RecycleDataPacket(pPack, this, true);
	}

}

VistaKernelMsgPacket *VistaMsgSink::GetCurrentPacket()
{
	return m_pCurrentPacket;
}

void  VistaMsgSink::AnswerQuery(VistaKernelMsgPacket *pPacket)
{
	m_pDataInput->RecycleDataPacket(pPacket, this, true); // never forget to recycle on a sink!
}

// ############################################################################


//VistaKernelMsgPort::VistaKernelMsgPort(VistaSystem &rSystem, const string &sIniFile,
//										 const string &sIniSection,
//                                        const string &sApplicationName)
//{
//    m_sApplicationName = sApplicationName;
//    m_pMsg = new VistaProgressMessage;
//    (*m_pMsg).SetApplicationName(m_sApplicationName);
//    m_pSystem = &rSystem;
//
//    m_bRescheduleFlag = false;
//    m_pPool = new VistaThreadPool(1);
//    m_pSource = new VistaMsgSource;
//    m_pProducer = new DLVistaActiveDataProducer(m_pSource);
//	m_pProducer->SetWaitForData(true); // blocking production
//	m_pProducer->SetWaitTimeout(10);
//
//    m_pQueue = new DLVistaRamQueuePipe;
//    m_pSink = new VistaMsgSink;
//
//    m_pSource->AttachOutputComponent(m_pQueue);
//    m_pQueue->AttachInputComponent(m_pSource);
//    m_pQueue->AttachOutputComponent(m_pSink);
//    m_pSink->AttachInputComponent(m_pQueue);
//
//
//    string sHost;
//
//	VistaProfiler Profile(m_pSystem->GetIniSearchPaths());
//    Profile.GetTheProfileString(  sIniSection, "MSGPORTIP","localhost",sHost,sIniFile);
//    int iPort = Profile.GetTheProfileInt(sIniSection, "MSGPORTPORT", 6666, sIniFile);
//    //m_pAcceptThread->Run(); // detach
//
//    m_pAcceptWork = new VistaAcceptWork(m_pSource, m_pProducer, sHost, iPort);
//    m_pPool->AddWork(m_pAcceptWork);
//
//    m_pPool->StartPoolWork();
//
//	bool bCreateIndicator =
//		 Profile.GetTheProfileBool( sIniSection, "PROGRESSINDICATOR", false, sIniFile);
//        cout << "[ViMspPort]: Section (" << sIniSection << ") in file (" << sIniFile << ") tells me "
//             << (bCreateIndicator ? "" : "NOT") << " to reach PROGRESSINDICATOR.\n";
//
//	if(bCreateIndicator)
//	{
//		string sProgressHost;
//		Profile.GetTheProfileString(sIniSection, "PROGRESSHOST", "localhost", sProgressHost, sIniFile);
//		int iProgressPort = Profile.GetTheProfileInt(sIniSection, "PROGRESSPORT", 6667, sIniFile);
//		try
//		{
//			m_pProgressConnection = new VistaConnectionIP(VistaConnectionIP::CT_UDP, sProgressHost, iProgressPort);
//		}
//		catch(VistaExceptionBase &x)
//		{
//			x.PrintException();
//		}
//	}
//	else
//		m_pProgressConnection = NULL;
//}

VistaKernelMsgPort::VistaKernelMsgPort( VistaSystem* pVistaSystem,
									   const string &sHost,
									   int iPort,
									   const string &sApplicationName,
									   bool bCreateIndicator,
									   const string &sIndicatorHost,
									   int iIndicatorPort,
									   IDLVistaPipeComponent *pFrontPipe)
{
	m_sApplicationName = sApplicationName;
	m_pMsg = new VistaProgressMessage;
	(*m_pMsg).SetApplicationName(m_sApplicationName);
	m_pSystem = pVistaSystem;

	m_bRescheduleFlag = false;
	m_pPool = new VistaThreadPool(1);

	// create the end-part of the pipe first
	m_pQueue = new DLVistaRamQueuePipe;
	m_pSink = new VistaMsgSink;
	m_pQueue->AttachOutputComponent(m_pSink);
	m_pSink->AttachInputComponent(m_pQueue);

	if(pFrontPipe)
	{
		m_pSource = NULL;
		m_pUserFront = pFrontPipe;

		m_pUserFront->AttachOutputComponent(m_pQueue);
		m_pQueue->AttachInputComponent(m_pUserFront);
	}
	else
	{
		m_pUserFront = NULL;
		m_pSource = new VistaMsgSource;
		m_pProducer = new DLVistaActiveDataProducer(m_pSource);
		m_pProducer->SetWaitForData(true); // blocking production
		m_pProducer->SetWaitTimeout(10);

		m_pSource->AttachOutputComponent(m_pQueue);
		m_pQueue->AttachInputComponent(m_pSource);

		m_pAcceptWork = new VistaAcceptWork(m_pSource, m_pProducer, sHost, iPort,
			pVistaSystem->GetIniFile());
		m_pPool->AddWork(m_pAcceptWork);

		m_pPool->StartPoolWork();
	}

	if(bCreateIndicator)
	{
		try
		{
			m_pProgressConnection = new VistaConnectionIP(VistaConnectionIP::CT_UDP,
				sIndicatorHost,
				iIndicatorPort);
		}
		catch(VistaExceptionBase &x)
		{
			x.PrintException();
		}
	}
	else
		m_pProgressConnection = NULL;
}

VistaKernelMsgPort::~VistaKernelMsgPort()
{
	delete m_pMsg;
	if(m_pAcceptWork)
		m_pAcceptWork->SetAbortSignal();

	m_pPool->StopPoolWork();
	m_pPool->WaitForAllJobsDone();


	if(m_pProducer && m_pProducer->IsComponentRunning())
	{
		m_pProducer->StopComponentGently(true);
	}

	delete m_pQueue;
	delete m_pProducer;
	delete m_pSource;
	delete m_pUserFront;

	delete m_pSink;

	delete m_pPool;
	delete m_pAcceptWork;


	if(m_pProgressConnection)
		m_pProgressConnection->Close(); // just to make sure....
	delete m_pProgressConnection;
}

int VistaKernelMsgPort::HasMessage()
{
	/** @todo we should clean-up the IsEmpty semantics! */
	return (!m_pQueue->IsEmpty() ? 1 : 0);
}

bool VistaKernelMsgPort::GetIsConnected() const
{
	return m_pAcceptWork->GetIsDone() && m_pAcceptWork->GetIsConnected(); // we are connected when this job is done?
}


VistaMsg *VistaKernelMsgPort::GetNextMsg()
{
	if(m_pSink->PullPacket())
	{
		// ok, we have a packet here
		VistaKernelMsgPacket *p = m_pSink->GetCurrentPacket();
		if(p)
		{
			p->SetSourcePort(this);
			m_mpMsgMap.insert(MSGMAP::value_type(p->GetPacketMessage(), p));
			return p->GetPacketMessage();
		}
	}
	return NULL;
}

void  VistaKernelMsgPort::AnswerQuery(VistaMsg *pMsg)
{
	if(m_pProducer && m_pProducer->IsComponentRunning())
	{
		MSGMAP::iterator it = m_mpMsgMap.find(pMsg);
		if(it != m_mpMsgMap.end())
		{
			(*it).second->SetSourcePort(NULL);
			m_pSink->AnswerQuery((*it).second);
			m_mpMsgMap.erase(it); // kill from table
		}
	}
	else
	{
		// ignoring answer
	}
}

bool VistaKernelMsgPort::PutQuery(VistaMsg *pMsg)
{
	if(m_pProducer && m_pProducer->IsComponentRunning())
	{
		m_pSink->PutQuery(pMsg);
	}
	return false;
}

void VistaKernelMsgPort::Disconnect()
{
	vstr::outi() << "VistaKernelMsgPort::Disconnect()" << std::endl;
	if(m_pProducer && m_pProducer->IsComponentRunning())
	{
		// we will stop to watch connection
		m_pProducer->StopComponentGently(true);

		if(m_pSource)
		{
			// we will close connection, which will close the socket
			m_pSource->GetIncomingConnection()->Close();
			delete m_pSource->GetIncomingConnection();
			m_pSource->SetIncomingConnection(NULL); // reset

			int iPackets = m_pSource->TryToReclaimPendingPackets();
			printf("VistaKernelMsgPort::Disconnect() -- reclaimed %d pending packets\n", iPackets);
		}
	}

	if(m_pAcceptWork && m_pAcceptWork->GetIsDone())
	{
		vstr::outi() << "-- Rescheduling accept job." << std::endl;
		m_pPool->RemoveDoneJob(m_pAcceptWork->GetJobId()); // remove
		m_pPool->AddWork(m_pAcceptWork);
	}
}

void VistaKernelMsgPort::SetRescheduleFlag(bool b)
{
	m_bRescheduleFlag = b;
}

bool VistaKernelMsgPort::GetRescheduleFlag() const
{
	return m_bRescheduleFlag;
}


bool VistaKernelMsgPort::WriteProgress(eProgressType eType, int iProg, const string &sProgMessage)
{
	if(!m_pProgressConnection)
		return false; // no connection, no fun...
	try
	{
		VistaByteBufferSerializer ser;
		// we should serialize the message now
		/** @todo */
		switch(eType)
		{
		case PRG_APP:
			{
				(*m_pMsg).SetSubtaskProgressLabel(sProgMessage);
				(*m_pMsg).SetSubtaskProgress(iProg);
				break;
			}
		case PRG_DONE:
			{
				(*m_pMsg).SetHideFlag(true);
			}
		case PRG_SYSTEM:
			{
				(*m_pMsg).SetTotalProgress(iProg);
				(*m_pMsg).SetTotalProgressLabel(sProgMessage);
				break;
			}

		default:
			break;
		}

		(*m_pMsg).Serialize(ser);

		if(m_pProgressConnection->WriteRawBuffer( ser.GetBuffer(), ser.GetBufferSize())== -1)
			return false; // something went wrong, but no exception...

		//VistaTimeUtils::Sleep(2000);
		// we could send the message.
		return true;
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		if(m_pProgressConnection)
			m_pProgressConnection->Close();
		delete m_pProgressConnection;
		m_pProgressConnection = NULL; // clear pointer,
		return false;
	}
}


bool VistaKernelMsgPort::DispatchKernelMsg(VistaMsg &rMsg)
{
	VistaMsg *pMsg = &rMsg;

	//	cout << "*********** HandleKernelMsg ************" << std::endl;
	VistaByteBufferDeSerializer dsSer;
	VistaMsg::MSG vecMsg = pMsg->GetThisMsg();
	dsSer.FillBuffer( &vecMsg[0], (int)vecMsg.size() );

	int iKernelTarget=0;

	dsSer.ReadInt32(iKernelTarget);
	// ok, ready to rumble
	int iMsgType = 0;
	dsSer.ReadInt32(iMsgType);
	pMsg->SetMsgSuccess(true); // we are optimistic

	switch(iKernelTarget)
	{
	case SYSTEMMSG:
		{
			DispatchSystemMsg(iMsgType, pMsg, dsSer);
			break;
		}
	case DISPLAYMSG:
		{
			DispatchDisplayMsg(iMsgType, pMsg, dsSer);
			break; // DISPLAYMSG
		}
	case INTERACTIONMSG:
		{
			DispatchInteractionMsg(iMsgType, pMsg, dsSer);
			break; // INTERACTIONMSG
		}
	case INTERACTIONDEVICEMSG:
		{
			DispatchInteractionDeviceMsg(iMsgType, pMsg, dsSer);
			break; // INTERACTIONDEVICEMSG
		}
	case UNDEFINEDMSG:
	default:
		pMsg->SetMsgSuccess(false);
		break;
	}

	return pMsg->GetMsgSuccess();
}

bool VistaKernelMsgPort::DispatchInteractionMsg(int iMsgType, VistaMsg *pMsg, VistaByteBufferDeSerializer &dsSer)
{
	switch(iMsgType)
	{
		case GETAVAILABLEDEVICES:
		case GETCURRENTDEVICES:
		case GETCURRENTTRACKER:
		case GETAVAILABLETRACKER:
			//{
			//	VistaByteBufferSerializer ser;
			//	VistaMsg::MSG veAnswer;
			//	list<string> liDevices;
			//	switch(iMsgType)
			//	{
			//	case GETAVAILABLETRACKER:
			//		{
			//			liDevices = m_pSystem->GetOldInteractionManager()->GetAvailableTrackerDrivers();
			//			break;
			//		}
			//	case GETCURRENTDEVICES:
			//		{
			//			liDevices = m_pSystem->GetOldInteractionManager()->GetCurrentDeviceDrivers();
			//			break;
			//		}
			//	case GETCURRENTTRACKER:
			//		{
			//			liDevices = m_pSystem->GetOldInteractionManager()->GetCurrentTrackerDrivers();
			//			break;
			//		}
			//	case GETAVAILABLEDEVICES:
			//		{
			//			liDevices = m_pSystem->GetOldInteractionManager()->GetAvailableDeviceDrivers();
			//			break;
			//		}
			//	default:
			//		break;
			//	}
			//	ser.WriteInt32(VistaType::uint32(liDevices.size()));
			//	for(list<string>::const_iterator cit = liDevices.begin();
			//		cit != liDevices.end();
			//		++cit)
			//		{
			//			ser.WriteInt32(VistaType::uint32((*cit).size()));
			//			ser.WriteString(*cit);
			//		}
			//	ser.GetBuffer(veAnswer);
			//	pMsg->SetMsgAnswer(veAnswer);
				//break;
			//}
		default:
			pMsg->SetMsgSuccess(false);
			break;
	}

	return pMsg->GetMsgSuccess();
}

bool VistaKernelMsgPort::DispatchSystemMsg(int iMsgType, VistaMsg *pMsg, VistaByteBufferDeSerializer &dsSer)
{
	switch(iMsgType)
	{
	case GETVISTAVERSION:
		{
			string answer = string(VistaVersion::GetVersion()) + string(" ")+string(VistaVersion::GetBuildDateString());
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer, veAnswer);
			//veAnswer.assign(answer.begin(), answer.end());
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case GETAPPLICATIONNAME:
		{
			string answer = m_pSystem->GetApplicationName();
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer, veAnswer);
			//veAnswer.assign(answer.begin(), answer.end());
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case GETOSTYPE:
		{
			string answer = VistaEnvironment::GetOSystem();
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer, veAnswer);
			//veAnswer.assign(answer.begin(), answer.end());
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case GETCPUTYPE:
		{
			string answer = VistaEnvironment::GetCPUType();
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer, veAnswer);
			//veAnswer.assign(answer.begin(), answer.end());
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case GETNUMBEROFPROCESSORS:
		{
			string answer = VistaEnvironment::GetNumberOfProcessors();
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer, veAnswer);
			//veAnswer.assign(answer.begin(), answer.end());
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case GETMEMORY:
		{
			string answer = VistaEnvironment::GetMemory();
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer, veAnswer);
			//veAnswer.assign(answer.begin(), answer.end());
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case QUITAPP:
		{
			m_pSystem->Quit();
			break;
		}
	case DISCONNECT:
		{
			Disconnect();
			break;
		}
	case ISCLUSTERLEADER:
		{
			string answer = ( m_pSystem->GetClusterMode()->GetIsLeader() ? "true" : "false");
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer, veAnswer);
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case ISCLUSTERFOLLOWER:
		{
			string answer = ( m_pSystem->GetClusterMode()->GetIsFollower() ? "true" : "false");
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer,veAnswer);
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case GETCLUSTERMODE:
		{
			string answer = m_pSystem->GetClusterMode()->GetClusterModeName();
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer,veAnswer);
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case GETINIFILE:
		{
			string answer = m_pSystem->GetIniFile();
			VistaMsg::MSG veAnswer;
			VistaMsg::AssignMsgByString(answer, veAnswer);
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case SETINIFILE:
		{
			vector<VistaType::byte> vecMsg = pMsg->GetThisMsg();
#if defined(LINUX) || (defined(WIN32) && (_MSC_VER>=1300)) || defined(DARWIN)
			string sIniFile(vecMsg.begin(), vecMsg.begin()+vecMsg.size());
#else
			string sIniFile((char*)vecMsg.begin(), (char*)vecMsg.begin()+vecMsg.size());
#endif
			//sIniFile.assign((const char*)vec.begin(), (const char*)vec.end());
			m_pSystem->SetIniFile(sIniFile);
			sIniFile = "OK";
			VistaMsg::MSG veAnswer; //(sIniFile.begin(), sIniFile.end());
			VistaMsg::AssignMsgByString(sIniFile, veAnswer);
			//veAnswer.assign((VistaType::byte*)sIniFile.begin(), (unsigned char *)sIniFile.end());
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	default:
		pMsg->SetMsgSuccess(false);
		break;
	}
	return pMsg->GetMsgSuccess();
}

bool VistaKernelMsgPort::DispatchDisplayMsg(int iMsgType, VistaMsg *pMsg, VistaByteBufferDeSerializer &dsSer)
{
	switch(iMsgType)
	{
	case GETROOT:
		{
			VistaByteBufferSerializer ser;
			VistaMsg::MSG veAnswer;
			//ser.SetBufferByReference(veAnswer);

			//VistaSceneGraph *pSG = m_pSystem->GetGraphicsManager()->GetSceneGraph();
			VistaTransformMatrix trans;
			/**
			* @todo think about the protokoll
			* pSG->GetRoot()->GetTransform(trans);
			*/
			for(int i=0; i < 4; ++i)
				for(int j=0; j < 4; ++j)
					ser.WriteFloat32(trans.GetValue(i,j));

			//printf("veAnswer size=%d\n", veAnswer.size());
			ser.GetBuffer(veAnswer);
			pMsg->SetMsgAnswer(veAnswer);
			//vector<VistaType::byte> vec = pMsg->GetMsgAnswer();
			//for(int k=0; k < vec.size(); ++k)
			//    printf("%x ", vec[k]);

			break;
		}
	case GETDISPLAYSYSTEM:
		{
			VistaByteBufferSerializer ser;
			VistaMsg::MSG veAnswer;

			//                VistaDisplayManager *pDispMan = m_pSystem->GetDisplayManager();
			//                VistaDisplaySystem  *pDispSys = pDispMan->GetCurDisplaySystem();
			//                ser.WriteSerializable(*pDispSys);
			//                ser.GetBuffer(veAnswer);
			//                pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	case SETDISPLAYSYSTEM:
		{
			VistaMsg::MSG veAnswer;
			//                VistaDisplayManager *pDispMan = m_pSystem->GetDisplayManager();
			//                VistaDisplaySystem  *pDispSys = pDispMan->GetCurDisplaySystem();
			//                dsSer.ReadSerializable(*pDispSys);

			//                pDispSys->UpdateViewPoint();

			veAnswer.push_back('O');
			veAnswer.push_back('K');
			pMsg->SetMsgAnswer(veAnswer);
			break;
		}
	default:
		pMsg->SetMsgSuccess(false);

	}
	return pMsg->GetMsgSuccess();
}

bool VistaKernelMsgPort::DispatchInteractionDeviceMsg(int iMsgType, VistaMsg *pMsg, VistaByteBufferDeSerializer &dsSer)
{
	switch(iMsgType)
	{
	case GETDEVICE:
		//{
		//		VistaMsg::MSG veAnswer;
		//		int iLength = 0;
		//		string sDevRole;

		//		dsSer.ReadInt32(iLength);
		//		dsSer.ReadString(sDevRole, iLength);
		//		VistaInputDevice *pDevice = m_pSystem->GetOldInteractionManager()->GetInputDeviceByRole(sDevRole);
		//		if(pDevice)
		//		{
		//			VistaByteBufferSerializer ser;
		//			(*pDevice).Serialize(ser);

		//			ser.GetBuffer(veAnswer);
		//			pMsg->SetMsgAnswer(veAnswer);
		//		}
		//		else
		//			pMsg->SetMsgSuccess(false);
		//		break;
		//	}
		//case SETDEVICE:
		//	{
		//		VistaMsg::MSG veAnswer;
		//		int iLength = 0;
		//		string sDevRole;

		//		dsSer.ReadInt32(iLength);
		//		dsSer.ReadString(sDevRole, iLength);
		//		VistaInputDevice *pDevice = m_pSystem->GetOldInteractionManager()->GetInputDeviceByRole(sDevRole);
		//		if(pDevice)
		//		{
		//			VistaByteBufferDeSerializer deSer;
		//			(*pDevice).DeSerialize(deSer);

		//			pMsg->SetMsgSuccess(true);
		//		}
		//		else
		//			pMsg->SetMsgSuccess(false);
		//		break;
		//	}
	default:
		pMsg->SetMsgSuccess(false);
		break;
	}

	return pMsg->GetMsgSuccess();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

