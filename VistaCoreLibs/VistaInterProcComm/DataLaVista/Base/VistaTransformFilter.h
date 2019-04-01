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


#ifndef IDLVISTATRANSFORMFILTER_H
#define IDLVISTATRANSFORMFILTER_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaFilter.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;
class DLVistaPacketQueue;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IDLVistaTransformFilter : public IDLVistaFilter
{
private:
	IDLVistaTransformFilter(IDLVistaTransformFilter &);

	int m_iOutboundQueueSize;

	int m_iFilterCount;

	int m_iDropCount;

protected:
	IDLVistaTransformFilter();

	virtual bool DoTransform(IDLVistaDataPacket *, IDLVistaDataPacket *pBuf) = 0;

	DLVistaPacketQueue *m_pOutboundPackets;
public:

	virtual ~IDLVistaTransformFilter();

	virtual IDLVistaDataPacket *FilterPacketL(IDLVistaDataPacket *pPacket );

	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	virtual IDLVistaDataPacket *GivePacket(bool bBlock);

	void ConsumePacket(IDLVistaDataPacket *pPacket) {};

	virtual bool InitPacketMgmt();

	void SetOutboundQueueSize(int iSize);

	int GetOutboundQueueSize() const;

	/**
	 * The return Packet method for transform filters always returns NULL
	 */
	virtual IDLVistaDataPacket *ReturnPacket();
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //IDLVISTAFILTER_H

