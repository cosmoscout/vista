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


#ifndef IDLVISTADATAPRODUCER_H
#define IDLVISTADATAPRODUCER_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaPipeComponent.h"

#include <list>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;
class IDLVistaDataConsumer;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IDLVistaDataProducer : public IDLVistaPipeComponent
{
private:
protected:
	virtual void FillPacket(IDLVistaDataPacket *pPacket ) =0;
	IDLVistaPipeComponent * m_pDatipcoutput;
	IDLVistaDataProducer();
public:

	virtual ~IDLVistaDataProducer() = 0;

	virtual IDLVistaDataPacket * ProducePacket() =0;

	virtual bool PushPacket(IDLVistaDataPacket *pPacket = 0) =0;

	virtual bool HasPacket() const =0;

	virtual bool IsDataProducer() const { return true;}

	virtual bool IsDataConsumer() const { return false; }


	virtual bool IsInputComponent(IDLVistaPipeComponent *pComp) const { return false; /* we do not have no input */ };

	virtual bool IsOutputComponent(IDLVistaPipeComponent *pComp) const;

	virtual bool AttachInputComponent(IDLVistaPipeComponent *pComp);
	virtual bool DetachInputComponent(IDLVistaPipeComponent *pComp);
	virtual bool AttachOutputComponent(IDLVistaPipeComponent *pComp);
	virtual bool DetachOutputComponent(IDLVistaPipeComponent *pComp);

	virtual IDLVistaPipeComponent *GetOutboundByIndex(int iIndex) const;

	virtual IDLVistaPipeComponent *GetInboundByIndex(int iIndex) const;

	virtual int GetNumberOfOutbounds() const;

	virtual int GetNumberOfInbounds() const;


	virtual std::list<IDLVistaPipeComponent *> GetInputComponents() const;

	virtual std::list<IDLVistaPipeComponent *> GetOutputComponents() const;


	/**
	 * In case this producer does run out of packets, the production method does call FillUp() in order to
	 * gain ALL packages that are waiting for recycling at the outbound queue. This strategy tries to
	 * minimize the times a queue gets empty. If a producer runs out of packets, the consumer is much
	 * slower than the producer.
	 * @return a fresh packet that can be used, NULL if there are no fresh packets left
	 */
	virtual int TryToReclaimPendingPackets();

	virtual bool WaitForNextPacket(int iTimeout) const;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVISTADATAPRODUCER_H

