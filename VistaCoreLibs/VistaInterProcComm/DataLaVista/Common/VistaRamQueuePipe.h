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


#ifndef DLVISTARAMQUEUEPIPE_H
#define DLVISTARAMQUEUEPIPE_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPipe.h>
#include <deque>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


class IDLVistaDataPacket;
class DLVistaPacketQueue;
class VistaMutex;
class VistaThreadEvent;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaRamQueuePipe : public IDLVistaPipe
{
public:

	DLVistaRamQueuePipe();

	~DLVistaRamQueuePipe();

	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	virtual IDLVistaDataPacket * GivePacket(bool bBlock);
	virtual IDLVistaDataPacket * ReturnPacket();

	bool InitPacketMgmt();

	bool IsFull() const;
	bool IsEmpty() const;
	int Capacity() const { return -1; };
	int OwnerLockEmpty();

	bool GetIsBlockedPending() const;
	bool SignalPendingRequest();

	/**
	 * You can set a ThreadEvent from outside which is signaled
	 * whenever the m_pQueueNotEmptyEvent is signaled.
	 * This is useful if you do not want to block
	 * on PullPacket, but on your own event.
	 */
	void SetOutsideThreadEvent (VistaThreadEvent*);

	void SetDoesBlockOnReturnPacket(bool b);
	bool GetDoesBlockOnReturnPacket() const;

private:
	std::deque<IDLVistaDataPacket *> *m_pquInQueue;
	std::deque<IDLVistaDataPacket *> *m_pquRecycleQueue;

	VistaMutex *m_pMutexIn, *m_pMutexOut, *m_pLockEmpty;
	VistaThreadEvent *m_pRecycleEvent;
	VistaThreadEvent *m_pQueueNotEmptyEvent;
	VistaThreadEvent *m_pOutsideEvent;

	VistaMutex *GrabMutex(VistaMutex *pMutex, bool bBlock);

	bool m_bCanRecycle, m_bBlockedPending, m_bDoesBlockOnReturn;

	int m_iNumDownstreamPackets;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //DLVISTARAMQUEUEPIPE_H

