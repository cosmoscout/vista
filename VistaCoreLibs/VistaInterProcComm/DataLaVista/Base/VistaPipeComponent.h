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


#ifndef IDLVISTAPIPECOMPONENT_H
#define IDLVISTAPIPECOMPONENT_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <list>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is the base class for all pipe-components.
 * It does define the interface for attaching and detaching of in- and output to other pipe-components.
 * This is true for most elements, except maybe teeing or aggregating filters
 * and can be overridden at the cost of two pointers ;).
 * Furthermore we can see here that any component has an input and an output type-id
 * for packets; this id will be the same for all elements except transforming filters,
 * most probably. Any component can be the end and the beginning pipe-element. In order
 * to check pipe-constraints this has to be unambigiously defined (see IsDataProducer/IsDataConsumer).
 * Components can be either active or passive. In case the component is active, there
 * will be something like an update-thread that takes care of the function of the
 * component. A non-active component is passive.
 */
class VISTAINTERPROCCOMMAPI IDLVistaPipeComponent
{
protected:
	IDLVistaPipeComponent();
public:

	virtual ~IDLVistaPipeComponent();
	/**
	 * Returns the input-packet type for this component.
	 * This can be used to check pipe-integrity.
	 * @return the valid type-id for incoming packets (-1 iff this component does not accept any incoming packet, e.g. is a producer)
	 */
	virtual int GetInputPacketType() const;

	/**
	 * Returns the output-packet type for this component.
	 * This can be used to check pipe-integrity.
	 * @return the valid type-id for outgoing packets (-1 iff this component does not produce packets, e.g. is a consumer)
	 */
	virtual int GetOutputPacketType() const;


	/**
	 * This is a method for constraint checking.
	 * @param pComp the component to see whether this is registered as input to this component
	 * @return true if pComp is a valid input to this component.
	 */
	virtual bool IsInputComponent(IDLVistaPipeComponent *pComp) const = 0;

	/**
	 * This is a method for constraint checking.
	 * @param pComp the component to see whether this is registered as input to this component
	 * @return true if pComp is a valid input to this component.
	 */
	virtual bool IsOutputComponent(IDLVistaPipeComponent *pComp) const = 0;


	/**
	 * This method does attach a given pipecomponent to this component as input.
	 * It has to be defined by subclasses in order to reflect various strategies
	 * for handling input components
	 * @param pComp the component to receive incoming packages from
	 * @return true iff the component could be attached (see subclasses for explanations)
	 */
	virtual bool AttachInputComponent(IDLVistaPipeComponent * pComp) =0;


	virtual IDLVistaPipeComponent *GetOutboundByIndex(int iIndex) const = 0;

	virtual int GetNumberOfOutbounds() const = 0;

	/**
	 * In order to get a component out of a pipe, this method can be used to
	 * withdraw an attached onput component from this component.
	 * Note that the detach of a non-attached component should return false here.
	 * @param pComp the input component to disconnect from
	 * @return true iff the component could be detached.
	 */
	virtual bool DetachInputComponent(IDLVistaPipeComponent * pComp) =0;

	virtual int GetNumberOfInbounds() const = 0;

	virtual IDLVistaPipeComponent *GetInboundByIndex(int iIndex) const = 0;

	/**
	 * This method does attach a given pipecomponent to this component as output.
	 * It has to be defined by subclasses in order to reflect various strategies
	 * for handling output components
	 * @param pComp the component to send outgoing packages to
	 */
	virtual bool AttachOutputComponent(IDLVistaPipeComponent *pComp ) =0;

	/**
	 * In order to get a component out of a pipe, this method can be used to
	 * withdraw an attached output component from this component.
	 * Note that the detach of a non-attached component should return false here.
	 * @param pComp the output component to disconnect from
	 * @return true iff the component could be detached.
	 */
	virtual bool DetachOutputComponent(IDLVistaPipeComponent *pComp ) =0;


	/**
	 * Indicates whether this component is a producer (meaning: the beginning of a pipe).
	 * @return false always, this is default and has to be defined by subclasses.
	 */
	virtual bool IsDataProducer() const { return false; };

	/**
	 * Indicates whether this component is a consumer (meaning: the end of a pipe).
	 * @return false always, this is defualt and has to be defined by subclasses.
	 */
	virtual bool IsDataConsumer() const { return false; }

	/**
	 * The input component can call this method in order to deliver an incomming
	 * packet. The component will act according to its semantics in order to work
	 * on the new packet.
	 * @param pPacket the packet to receive for this component
	 * @param bBlock indicates whether the sender should be blocked until the packet was delivered
	 * @param pSender points to the sender of this packet
	 * @return true iff the packet was delivered and is being worked on
	 */
	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false) =0;

	/**
	 * This method can be called by an outbound pipe-partner in order to receive
	 * an old and worked on packet for recycling. The incoming packet will be
	 * put in a state that makes it possible to reuse it later on.
	 * @param pPacket the old packet to take back
	 * @param bBlock indicates whether the outbound partner will be blicked until the packet was recycled at a whole
	 */
	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false) =0;


	/**
	 * An outbound pipe-component can ask this component to deliver a new packet.
	 * @param bBlock indicates whether the calling component will be blocked until a new packet can be delivered.
	 * @return a pointer to a new packet that can be worked on or NULL iff no packet could be retrieved.
	 */
	virtual IDLVistaDataPacket * GivePacket(bool bBlock) =0;

	/**
	 * An inbound pipe-component can call this method in order to get back
	 * already processed packets (for recycling). This is mostly used for pipe-instances.
	 * @return an old and processed packet ready for recycling
	 */
	virtual IDLVistaDataPacket *ReturnPacket();


	/**
	 * Indicates whether this component is active or passive.
	 * An active component is associated to a processing-thread that will take care
	 * of the proper semantics for this component.
	 * @return true iff this component is active and thus working on its own (default is false).
	 */
	virtual bool IsActiveComponent() const { return false; }


	/**
	 * Any component has the possibility to put up its own package-management and packet-queues.
	 * This method will initialize this and has to be called after the component was constructed.
	 * This is a two-level approach as some things might not be clear directly after creating the
	 * component using the constructor.
	 * @return true iff anything went well and the component is usable.
	 */
	virtual bool InitPacketMgmt() =0;

	/**
	 * Creates a proper outbound packet for this component and can be used to
	 * deliver the right packages in subclasses.
	 * @todo work over this
	 */
	virtual IDLVistaDataPacket *CreatePacket() = 0;

	/**
	 * Deletes a packet that was produced by this component. This is a legacy method
	 * and will most likely be put out of function.
	 * @param pPacket the packet to destroy and give free.
	 */
	virtual void DeletePacket(IDLVistaDataPacket *pPacket) = 0;

	/**
	 * In theory, it is possible to connect a single IDLVistaComponent to a multiple number of
	 * other IDLVistaPipeComponents, this method returns a list of PipeComponents that are attached
	 * to this component as Input-Components.
	 * @return a list of pointers to attached input components to this component
	 */
	virtual std::list<IDLVistaPipeComponent *> GetInputComponents() const = 0;

	/**
	 * In theory, it is possible to connect a single IDLVistaComponent to a multiple number of
	 * other IDLVistaPipeComponents, this method returns a list of PipeComponents that are attached
	 * to this component as Output-Components.
	 * @return a list of pointers to attached output components to this component
	 */
	virtual std::list<IDLVistaPipeComponent *> GetOutputComponents() const = 0;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVISTAPIPECOMPONENT_H

