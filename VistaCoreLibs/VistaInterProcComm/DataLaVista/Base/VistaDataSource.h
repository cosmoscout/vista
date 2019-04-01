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


#ifndef DLVISTADATASOURCE_H
#define DLVISTADATASOURCE_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include "VistaDataProducer.h"
#include <deque>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;
class DLVistaPacketQueue;
class IDLVistaRTC;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is an intermediate class that does define a buffered semantic for a producer like
 * pipe component. It does allocate a buffer packet queue, define algorithms of packet
 * production and recycling and marginal support for statistics (including a count for the
 * number of packet-drops and the number of produced packets.
 */
class VISTAINTERPROCCOMMAPI DLVistaDataSource : public IDLVistaDataProducer
{
public:
	/**
	 * Produces a new packet. The algorithm is as follows:
	 * <ul>
	 *  <li>Get a fresh packet (GetFreshPacket())
	 *  <li>In case there is none, call FillUp() to get as many recycled packets as possible
	 *  <li>If there is a packet, stamp it with a given time-stamp, else increase drop-count, return NULL
	 *  <li>Fill packet with content (sub-class defined, via FillPacket())
	 *  <li>increase package count
	 *  <li>give back the produced and filled packet
	 * </ul>
	 * @return a filled packet that is produced and locked
	 */
	virtual IDLVistaDataPacket * ProducePacket();

	/**
	 * Simply calls ProducePacket and returns its output.
	 * @param bBlock is currently ignored
	 * @see ProducePacket()
	 * @return the rerturn of ProducPacket()
	 * @todo implement the meaning of bBlock or drop
	 */
	IDLVistaDataPacket *GivePacket(bool bBlock);

	/**
	 * Destructor. Deletes the allocated PacketQueue.
	 */
	virtual ~DLVistaDataSource();


	/**
	 * Sets the number of Packets for the buffer-queue to the number given via SetQueueSize().
	 * @see SetQueueSize()
	 * @return true iff the number of desired packets could be allocated, else false
	 */
	bool InitPacketMgmt();

	/**
	 * This producer is told to Push a packet into its outbound pipe via this method. This is usually
	 * called by external active components.
	 * @param pPacket iff a packet is given, it is passed to the outbound component via AcceptDataPacket(pPacket), if no packet is given (default) the packet cerated by ProducePacket() is passed (if any could be produced)
	 * @return true iff a packet could be delivered
	 */
	virtual bool PushPacket(IDLVistaDataPacket *pPacket = 0);
	bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock = false);

	/**
	 * A producer does not consume packets, this method will always return false.
	 * @param pPacket the packet to consume
	 * @return always false
	 */
	bool ConsumePacket(IDLVistaDataPacket *pPacket);

	virtual IDLVistaDataPacket *GetEmptyPacket(bool bBlock = true);
	/**
	 * Simply passed the incoming packet to the PacketQueue for recycling.
	 * @param pPacket the packet to recycle
	 * @param bBlock  indicates whether the call to this method shall block until the packet is fully recycled or not
	 * @return true iff the packet could be successfully recycled
	 * @todo the bBlock semantics are not defined
	 */
	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock = false);

	/**
	 * Sets the queue size for the outbound buffer packets to the given argument. Note that this method
	 * will automatically call DLVistaPacketQueue::resize() and does set this method's result as the
	 * current queue-size.
	 * @param iSize the 'best you can get' for the outbound queue size.
	 */
	void SetQueueSize(int iSize);

	/**
	 * Returns the output-packet type for this component.
	 * This can be used to check pipe-integrity.
	 * @return the valid type-id for outgoing packets (-1 iff this component does not produce packets, e.g. is a consumer)
	 */
	virtual int GetOutputPacketType() const;

	 int GetPacketCount() const;

 protected:
	/**
	 * A pointer to an instance of an RTC that is needed to timestamp a produced packet.
	 *
	 */
	IDLVistaRTC *m_pTimer;


	/**
	 * Any data source can buffer a number of outgoing packets, this queue is the storage for
	 * those packets.
	 */
	DLVistaPacketQueue *m_pPacketQueue;

	/**
	 * Constructor. Initializes the buffer-packet-queue to zero packets, gets an instance of the global RTC
	 * and sets the counting variables to zero
	 */
	DLVistaDataSource();


	/**
	 * In case this producer does run out of packets, the production method does call FillUp() in order to
	 * gain ALL packages that are waiting for recycling at the outbound queue. This strategy tries to
	 * minimize the times a queue gets empty. If a producer runs out of packets, the consumer is much
	 * slower than the producer.
	 * @return a fresh packet that can be used, NULL if there are no fresh packets left
	 */
	virtual IDLVistaDataPacket *FillUp();

private:

	/**
	 * This is for debugging purposes on small queues and likely to be removed in future revision.
	 * @todo Remove me ;)
	 */
	void PrintQueue() const;

	int m_iQueueSize,   /**< defines the number of buffer packets to create @see m_pPacketQueue() **/
		m_iDropCount,   /**< counts the number of times that data had to be dropped because there were no outgoing packets left **/
		m_iPackageCount;/**< counts the number of packets that were produced by this producer **/
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTADATASOURCE_H

