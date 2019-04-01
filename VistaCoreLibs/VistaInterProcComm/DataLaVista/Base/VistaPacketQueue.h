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


#ifndef DLVISTAPACKETQUEUE_H
#define DLVISTAPACKETQUEUE_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <deque>


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;
class IDLVistaPipeComponent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is an abstraction for packet-management using a deque.
 * Most pipe-components will use this in order to maintain their in and outbound packets.
 */

class VISTAINTERPROCCOMMAPI DLVistaPacketQueue
{
	/**
	 * This is a simple typedef to ease typing
	 */
	typedef std::deque<IDLVistaDataPacket *> PQUEUE;
public:

	/**
	 * Destructor, deletes ALL packets in the current queue, regardless of their current state.
	 * Additionally it does give back the memory for m_pquPackets,
	 * @see ClearQueue()
	 */
	virtual ~DLVistaPacketQueue();

private:

	/**
	 * A pointer to the main storage for this queue's packets.
	 */
	PQUEUE *m_pquPackets;

	/**
	 * Any queue has its owner which is stored here. The link to the owner is needed in order to
	 * correctly create and destroy packets that are kept in this queue.
	 */
	IDLVistaPipeComponent *m_pOwner;

protected:

	/**
	 * Iterates over all packets in this queue and calls delete for them.
	 */
	void ClearQueue();
public:
	/**
	 * Constructor, pass the owner of this queue here and a proper iQueueSize to set this queue to.
	 * @param pComp the owner of this queue; this will be used to create packets for this queue
	 * @param iQueueSize the number of packets to allocate in this queue.
	 */
	DLVistaPacketQueue(IDLVistaPipeComponent *pComp, int iQueueSize);

	/**
	 * In case the client (e.g. producer) wants a fresh packet, it can call this method.
	 * @return a fresh packet from this queue that can be filled with information, or NULL if none is available
	 */
	IDLVistaDataPacket *GiveFreshPacket();

	IDLVistaDataPacket *GetPacketByIndex(int iIndex) const;

	/**
	 * Checks whether the pPacket does belong to this queue, if so, it does invalidate its timestamp (sets it to (-1,-1)),
	 * unlocks it and requeues the packet to be used again in the next iterations.
	 * @return true iff the packet belongs to this queue and could be recycles, else false
	 */
	bool RecyclePacket(IDLVistaDataPacket *pPacket);

	/**
	 * Returns the number of packets in this queue as a whole.
	 * @return the number of packets in this queue
	 */
	int GetPacketCount() const;

	/**
	 * Returns the number of available packets in this queue (which can be less or equal to GetPacketCount()
	 * @return the number of available packets before this queue runs out of stock
	 */
	int GetAvailablePacketCount() const;

	/**
	 * Tries to allocate iNewSize packets for this queue. All old packets will be deleted before the action.
	 * @param iNewSize the new number of packets for this queue
	 * @return the number of packets that could be allocated (usually equal to iNewSize)
	 */
	int Resize(int iNewSize);

	/**
	 * This is for debug output on small queues and most likely to be removed in future revisions.
	 * @todo Remove this method.
	 */
	void PrintQueue() const;

	/**
	 * A queue can only store one type of packets. This method will return the typeindicator that is given back
	 * by the packets themselves.
	 * You may only call this if GetPacketCount() > 0
	 * @return the typeindicator of the allocated data-packets.
	 */
	int GetPacketType() const;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTAPACKETQUEUE_H

