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


#ifndef IDLVISTAROUTINGPIPE_H
#define IDLVISTAROUTINGPIPE_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaPipe.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <vector>
#include <list>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IDLVistaDataPacket;
class IDLVistaPipeComponent;
class VistaMutex;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This gives a general interface for a pipe which routes packets to several outputs.
 * It has one input and routes this to one of several outputs.
 * In order to implement a concrete routing strategy, the RoutePacket(...) method has to be redefined.
 */
class VISTAINTERPROCCOMMAPI IDLVistaRoutingPipe :public IDLVistaPipe
{
protected:
	IDLVistaRoutingPipe();
public:
	virtual ~IDLVistaRoutingPipe();
	/**
	 * we aren't ever full
	 */
	virtual bool IsFull() const {return false;}
	/**
	 * we aren't ever empty either
	 */
	virtual bool IsEmpty() const {return false;}
	/**
	 * we got a whole lot of space
	 */
	virtual int Capacity() const;

	virtual bool IsOutputComponent(IDLVistaPipeComponent *pComp) const;
	virtual IDLVistaPipeComponent *GetOutboundByIndex(int iIndex) const;
	virtual int GetNumberOfOutbounds() const;
	virtual bool AttachOutputComponent(IDLVistaPipeComponent *pComp );
	/**
	 *  Detach all output at once
	 */
	virtual bool DetachAllOuputs();
	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);
	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);
	virtual IDLVistaDataPacket * GivePacket(bool bBlock);
	virtual IDLVistaDataPacket *ReturnPacket();
	virtual bool InitPacketMgmt();
	virtual std::list<IDLVistaPipeComponent *> GetOutputComponents() const;

protected:
	/**
	 * this is the central routing method i.e. here the packet routing happens.
	 * @return  int indicating to port to which the packet should be routed (-1 if no routing or error)
	 */
	virtual int RoutePacket(IDLVistaDataPacket* p) = 0;

	/**
	 * in order to maintain a valid routing table we do not allow arbitrary detachment!
	 */
	virtual bool DetachOutputComponent(IDLVistaPipeComponent *pComp ){return false;}

protected:
	/**
	* Output ports
	*/
	std::vector<IDLVistaPipeComponent*> m_vecOutputs;
	VistaMutex *m_pOutputLock;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVistaRoutingPipe_H

