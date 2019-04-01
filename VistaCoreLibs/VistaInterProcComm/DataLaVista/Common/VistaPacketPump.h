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


#ifndef IDLVISTAPACKETPUMP_H
#define IDLVISTAPACKETPUMP_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPipe.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaActiveComponent.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaThreadLoop;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaPacketPump : public IDLVistaPipe, public IDLVistaActiveComponent
{
private:
	VistaThreadLoop *m_pPumpLoop;
protected:
public:
	DLVistaPacketPump(int iPumpBurst);
	virtual ~DLVistaPacketPump();


	/**
	 * Any active component should start its task after the call to this
	 * method.
	 * @return true iff the component was started, false if the component was not started and something failed.
	 * @todo we should define a way to propagate error-states and messages from component to this interface
	 */
	virtual bool StartComponent() ;

	/**
	 * Components do usually iterate over a single task. A call to this method will
	 * gently ask to stop its work. After the next iteration the component will
	 * cease to work. You can specify to return after the component has surely ceased.
	 * @param bJoin if set to true, this call will return as soon as the component has stopped
	 * @return true iff the stop message could be given to the component.
	 */
	virtual bool StopComponentGently(bool bJoin) ;

	/**
	 * Use this method to pause the work of this component. A pause-message will be
	 * send to the running thread.
	 * @param bJoin set to true in order to return after the running thread has paused
	 * @return true iff the message could be sent to the running thread
	 * @todo we should define a way to propagate error-states and messages from component to this interface
	 */
	virtual bool PauseComponent(bool bJoin) ;

	/**
	 * Use this method to unpause a paused thread. Note that a call to this method
	 * for an unpaused thread may or may not reveal nondeterministic behaviour.
	 * @param bJoin indicates whether the calling thread shall be blocked until the active thread has resumed to function
	 * @return true iff the UnPause-message could be sent to the paused thread.
	 */
	virtual bool UnPauseComponent(bool bJoin) ;


	/**
	 * Use this method to stop the running component. Stopping means to halt it immediately.
	 * A stop message will be sent to the running thread. Note that a thread may catch the stop
	 * signal and exit cleanly without doing another iteration.
	 * @param bJoin set to true in order to return after the thread has stopped
	 * @return true iff the message could be sent to the running thread
	 * @todo we should define a way to propagate error-states and messages from component to this interface
	 */
	virtual bool StopComponent(bool bJoin) ;

	/**
	 * Use this message in case of emergency. A halted component will exit without the chance
	 * to cleanup.
	 * @return true iff the message could be sent to the running thread
	 * @todo we should define a way to propagate error-states and messages from component to this interface
	 */
	virtual bool HaltComponent() ;

	/**
	 * Use this method in order to find out whether this component is running or not.
	 * @todo there is no way to find out whether a thread is running or not in VistaThread
	 * @return true iff the component is running
	 */
	virtual bool IsComponentRunning() const ;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	virtual bool IsFull() const ;

	virtual bool IsEmpty() const ;

	virtual int Capacity() const ;

	virtual bool AttachOutputComponent(IDLVistaPipeComponent * pComp);

	virtual bool DetachOutputComponent(IDLVistaPipeComponent * pComp);

	/**
	 * The input component can call this method in order to deliver an incomming
	 * packet. The component will act according to its semantics in order to work
	 * on the new packet.
	 * @param pPacket the packet to receive for this component
	 * @param bBlock indicates whether the sender should be blocked until the packet was delivered
	 * @param pSender points to the sender of this packet
	 * @return true iff the packet was delivered and is being worked on
	 */
	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	/**
	 * This method can be called by an outbound pipe-partner in order to receive
	 * an old and worked on packet for recycling. The incoming packet will be
	 * put in a state that makes it possible to reuse it later on.
	 * @param pPacket the old packet to take back
	 * @param bBlock indicates whether the outbound partner will be blicked until the packet was recycled at a whole
	 */
	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);


	/**
	 * An outbound pipe-component can ask this component to deliver a new packet.
	 * @param bBlock indicates whether the calling component will be blocked until a new packet can be delivered.
	 * @return a pointer to a new packet that can be worked on or NULL iff no packet could be retrieved.
	 */
	virtual IDLVistaDataPacket * GivePacket(bool bBlock);

	/**
	 * An inbound pipe-component can call this method in order to get back
	 * already processed packets (for recycling). This is mostly used for pipe-instances.
	 * @return an old and processed packet ready for recycling
	 */
	virtual IDLVistaDataPacket *ReturnPacket();


	/**
	 * Any component has the possibility to put up its own package-management and packet-queues.
	 * This method will initialize this and has to be called after the component was constructed.
	 * This is a two-level approach as some things might not be clear directly after creating the
	 * component using the constructor.
	 * @return true iff anything went well and the component is usable.
	 */
	virtual bool InitPacketMgmt() ;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTAPACKETPUMP_H


