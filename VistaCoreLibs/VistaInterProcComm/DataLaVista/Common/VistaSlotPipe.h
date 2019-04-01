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


#ifndef DLVISTASLOTPIPE_H
#define DLVISTASLOTPIPE_H


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


class VISTAINTERPROCCOMMAPI DLVistaSlotPipe : public IDLVistaPipe
{
public:

	DLVistaSlotPipe();

	~DLVistaSlotPipe();

	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket,
								  IDLVistaPipeComponent *pSender,
								  bool bBlock=false);
	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket,
								   IDLVistaPipeComponent *pSender,
								   bool bBlock=false);

	virtual IDLVistaDataPacket * GivePacket(bool bBlock);
	virtual IDLVistaDataPacket * ReturnPacket();

	bool InitPacketMgmt();

	bool IsFull() const;
	bool IsEmpty() const;
	int Capacity() const { return -1; };
	int OwnerLockEmpty();


private:
	IDLVistaDataPacket *m_pInValue;

	std::deque<IDLVistaDataPacket *> *m_pquRecycleQueue;

	VistaMutex *m_pMutexIn, *m_pMutexOut, *m_pLockEmpty;
	VistaThreadEvent *m_pRecycleEvent;

	VistaMutex *GrabMutex(VistaMutex *pMutex, bool bBlock);
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //DLVISTARAMQUEUEPIPE_H

