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


#ifndef DLVISTAACTIVEDATACONSUMER_H
#define DLVISTAACTIVEDATACONSUMER_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaActiveComponent.h"
#include "VistaDataConsumer.h"

#include <string>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaThreadLoop;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VISTAINTERPROCCOMMAPI DLVistaActiveDataConsumer : public IDLVistaDataConsumer, public IDLVistaActiveComponent
{
public:

	/**
	 * Pass a pointer to the consumer that is supposed to come alive.
	 * The component will show its activeness by looping over the PullPacket() method.
	 * @param pComp a pointer to the consumer that should constantly suck for new packets :)
	 */
	DLVistaActiveDataConsumer(IDLVistaDataConsumer *pComp );
	/**
	 * Destructor. If called it will delete the thread-instance WITHOUT stopping it. You have to do that.
	 */
	~DLVistaActiveDataConsumer();

	bool StartComponent();

	bool IsComponentRunning() const;

	bool IsComponentPausing() const;

	bool StopComponentGently(bool bJoin);

	bool PauseComponent(bool bJoin);

	bool UnPauseComponent(bool bJoin);

	bool StopComponent(bool bJoin);

	bool HaltComponent();

	void SetThreadName(const std::string& strName);

	std::string GetThreadName() const;

	/**
	 * tell the component that active sucking is about to be stopped.
	 * Use this to indicate termination whenever the component may
	 * be in a blocking wait internally.
	 */
	void IndicateConsumptionEnd();


	virtual int  SetComponentPriority(const VistaPriority &pPrio);


	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 */

	/**
	 * The ActiveConsumer is a decorator pattern, so this is a simple forwarder to
	 * the active component's method.
	 * @param pPacket the packet to be consumed by the active consumer.
	 */
	virtual bool ConsumePacket(IDLVistaDataPacket *pPacket);

	/**
	 * The ActiveConsumer is a decorator pattern, so this is a simple forwarder to
	 * the active component's method.
	 * @param bBlock true iff the consumer should be blocked until a new packet was delivered by the pipe
	 * @return true iff a packet could be delivered and was scheduled by this consumer.
	 */
	virtual bool PullPacket(bool bBlock = true);

	/**
	 * A call to this method will activate the consumer component. Technically speaking:
	 * it will start the thread which will loop over PullPacket().
	 * @return true iff the component was started successfully.
	 */

	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false) ;
	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false) ;

	virtual bool IsDataProducer() const;

	virtual bool IsDataConsumer() const;


	virtual IDLVistaDataPacket *GivePacket(bool bBlock) ;
	virtual bool InitPacketMgmt();

	/**
	 * Creates a proper outbound packet for this component and can be used to
	 * deliver the right packages in subclasses.
	 * @todo work over this
	 */
	virtual IDLVistaDataPacket *CreatePacket();

	/**
	 * Deletes a packet that was produced by this component. This is a legacy method
	 * and will most likely be put out of function.
	 * @param pPacket the packet to destroy and give free.
	 */
	virtual void DeletePacket(IDLVistaDataPacket *pPacket);


	virtual bool IsOutputComponent(IDLVistaPipeComponent *pComp) const;

	virtual bool IsInputComponent(IDLVistaPipeComponent *pComp) const;

	virtual bool AttachInputComponent(IDLVistaPipeComponent *pComp);

	virtual bool AttachOutputComponent(IDLVistaPipeComponent *pComp);

	virtual bool DetachInputComponent(IDLVistaPipeComponent *pComp);

	virtual bool DetachOutputComponent(IDLVistaPipeComponent *pComp);

	virtual IDLVistaPipeComponent *GetOutboundByIndex(int iIndex) const;

	virtual IDLVistaPipeComponent *GetInboundByIndex(int iIndex) const;

	virtual int GetNumberOfOutbounds() const;

	virtual int GetNumberOfInbounds() const;


	virtual int GetInputPacketType() const;
	virtual int GetOutputPacketType() const;

	virtual std::list<IDLVistaPipeComponent *> GetInputComponents() const;
	virtual std::list<IDLVistaPipeComponent *> GetOutputComponents() const;

private:
	/**
	 * An instance of a thread that defines the pulse for this component.
	 */
	VistaThreadLoop *m_pThreadLoop;

	IDLVistaDataConsumer *m_pRealConsumer;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //DLVISTAACTIVECONSUMER_H

