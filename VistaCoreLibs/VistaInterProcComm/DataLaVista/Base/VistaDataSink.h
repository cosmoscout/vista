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


#ifndef DLVISTADATASINK_H
#define DLVISTADATASINK_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaDataProducer.h"
#include "VistaDataConsumer.h"
#include "VistaDataConsumer.h"

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaPipeComponent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaDataSink : public IDLVistaDataConsumer
{
public:

	virtual ~DLVistaDataSink();

	virtual bool PullPacket(bool bBlock = true);

	/**
	 * A sink "consumes" an incoming packet, so this is to be the default: call "ConsumePacket"
	 * @param pPacket the packet to be consumed, should be a valid type for this specific sink
	 * @param pComp the component that is giving the incoming packet
	 * @param bBlock true iff the operation shall block until sucessfull (default)
	 * @return true iff the packet could be consumed
	 * @see ConsumePacket()
	 */
	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock);


	/**
	 * A sink does not produce packets, so it will not recycle any package, this method will return
	 * false as a default. Incoming parameters are ignored
	 * @return always false for sinks
	 */
	virtual bool RecycleDataPacket(IDLVistaDataPacket *, IDLVistaPipeComponent *pComp, bool bBlock);


	/**
	 * A sink does not produce packets, so it will refuse to give some. This method will always return 0.
	 * Incoming parameters are ignored
	 * @return NULL always
	 */

	virtual IDLVistaDataPacket *GivePacket(bool bBlock);

	/**
	 * A sink does not produce packets, so its does not need packet managment. This method will always
	 * return true, as setting up "nothing" will always succeed.
	 * @return true always
	 */
	virtual bool InitPacketMgmt();


	/**
	 * A sink does not create packets, so this will return NULL
	 * @return NULL always
	 */
	virtual IDLVistaDataPacket *CreatePacket();

	/**
	 * A sink does not create packets, so as a consequence, it does not delete packets.
	 * All incoming arguments are ignored;
	 */
	virtual void DeletePacket(IDLVistaDataPacket *);

protected:
	DLVistaDataSink();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTADATASINK_H

