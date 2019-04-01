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


#ifndef IDLVISTAACTIVEDATAPRODUCER_H
#define IDLVISTAACTIVEDATAPRODUCER_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaActiveComponent.h"
#include "VistaDataProducer.h"

#include <string>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;
class IDLVistaDataConsumer;
class DLVistaProducerLoop;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaActiveDataProducer : public IDLVistaDataProducer, public IDLVistaActiveComponent
{
private:
	DLVistaProducerLoop *m_pThreadLoop;
	IDLVistaDataProducer *m_pRealProducer;

	bool m_bStopOnDestruct;
public:
	DLVistaActiveDataProducer(IDLVistaDataProducer *);
	virtual ~DLVistaActiveDataProducer();

	virtual IDLVistaDataPacket *ProducePacket() ;

	virtual bool PushPacket(IDLVistaDataPacket *pPacket = 0) ;

	virtual bool HasPacket() const ;

	virtual bool IsDataProducer() const { return true;}

	virtual bool IsDataConsumer() const { return false; }

	virtual bool AttachInputComponent(IDLVistaPipeComponent *pComp);
	virtual bool AttachOutputComponent(IDLVistaPipeComponent *pComp);

	virtual IDLVistaPipeComponent *GetOutboundByIndex(int iIndex) const;

	virtual IDLVistaPipeComponent *GetInboundByIndex(int iIndex) const;
	virtual int GetNumberOfOutbounds() const;

	virtual int GetNumberOfInbounds() const;

	virtual int GetInputPacketType() const;
	virtual int GetOutputPacketType() const;

	virtual std::list<IDLVistaPipeComponent *> GetInputComponents() const;
	virtual std::list<IDLVistaPipeComponent *> GetOutputComponents() const;

	virtual int TryToReclaimPendingPackets();
	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false) ;
	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false) ;

	virtual IDLVistaDataPacket *GivePacket(bool bBlock) ;
	virtual IDLVistaDataPacket *ReturnPacket();

	virtual bool IsActiveComponent() const { return true; }

	virtual bool InitPacketMgmt() ;

	virtual IDLVistaDataPacket *CreatePacket();

	virtual void DeletePacket(IDLVistaDataPacket *pPacket);

	bool StartComponent();

	bool StopComponentGently(bool bJoin);

	/**
	 * tell the component that active sucking is about to be stopped.
	 * Use this to indicate termination whenever the component may
	 * be in a blocking wait internally.
	 */
	void IndicateProductionEnd();

	bool PauseComponent(bool bJoin);

	bool UnPauseComponent(bool bJoin);

	bool StopComponent(bool bJoin);

	bool HaltComponent();


	int  SetComponentPriority(const VistaPriority &pPrio);
	bool IsComponentRunning() const;


	bool GetWaitForData() const;
	void SetWaitForData(bool bWait);

	void SetWaitTimeout(int iTimeout);
	int  GetWaitTimeout() const;

	void SetThreadName(const std::string& strName);

	std::string GetThreadName() const;


	virtual bool WaitForNextPacket(int iTimeout) const;

	bool GetStopOnDestruct() const;
	void SetStopOnDestruct(bool bStop);
protected:
	virtual void FillPacket(IDLVistaDataPacket * pPacket) ;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVISTADATAPRODUCER_H

