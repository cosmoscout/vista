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


#ifndef IDLVISTAPIPE_H
#define IDLVISTAPIPE_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaDataProducer.h"
#include "VistaDataConsumer.h"

#include <list>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IDLVistaPipe : public IDLVistaPipeComponent
{
protected:
	IDLVistaPipe();
public:
	virtual ~IDLVistaPipe();

	virtual bool IsFull() const =0;
	virtual bool IsEmpty() const =0;
	virtual int Capacity() const =0;

	/**
	 * This is a method for constraint checking.
	 * @param pComp the component to see whether this is registered as input to this component
	 * @return true if pComp is a valid input to this component.
	 */
	virtual bool IsInputComponent(IDLVistaPipeComponent *pComp) const;

	/**
	 * This is a method for constraint checking.
	 * @param pComp the component to see whether this is registered as input to this component
	 * @return true if pComp is a valid input to this component.
	 */
	virtual bool IsOutputComponent(IDLVistaPipeComponent *pComp) const;
	virtual bool AttachInputComponent(IDLVistaPipeComponent * pComp);

	virtual bool AttachOutputComponent(IDLVistaPipeComponent * pComp);

	virtual bool DetachInputComponent(IDLVistaPipeComponent * pComp);

	virtual bool DetachOutputComponent(IDLVistaPipeComponent * pComp);

	virtual IDLVistaDataPacket *CreatePacket() { /** we de not create packets **/ return 0; };

	virtual void DeletePacket(IDLVistaDataPacket *pPacket) {/** we do not delete packets **/ };

	virtual std::list<IDLVistaPipeComponent *> GetInputComponents() const;

	virtual std::list<IDLVistaPipeComponent *> GetOutputComponents() const;

	/**
	 * This component will accept any incoming packet, this method will return the type that the attached output will
	 * expect.
	 * @return this pipe's output component's input type or -1 iff this pipe is not connected
	 */
	virtual int GetInputPacketType() const;

	/**
	 * This component will forward incoming packets without modification, so this method will return the
	 * type that is given as input to this pipe
	 * @return this pipe's input component's output type ot -1 iff this pipe is not connected
	 */
	virtual int GetOutputPacketType() const;

	virtual IDLVistaPipeComponent *GetOutboundByIndex(int iIndex) const ;

	virtual int GetNumberOfOutbounds() const ;


	virtual int GetNumberOfInbounds() const;

	virtual IDLVistaPipeComponent *GetInboundByIndex(int iIndex) const ;

protected:
	IDLVistaPipeComponent * m_pInput;

	IDLVistaPipeComponent * m_pOutput;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVISTAPIPE_H


