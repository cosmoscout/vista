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


#ifndef IDLVISTAFILTER_H
#define IDLVISTAFILTER_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <deque>
#include <list>

#include "VistaPipeComponent.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;
class DLVistaPacketQueue;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IDLVistaFilter : public IDLVistaPipeComponent
{
private:
	IDLVistaFilter(IDLVistaFilter &);
protected:
	IDLVistaFilter();

	IDLVistaPipeComponent * m_pInput;

	IDLVistaPipeComponent * m_pOutput;
public:

	virtual ~IDLVistaFilter();


	/**
	 * This is the main method for subclasses to implement. During packet transport,
	 * FilterPacketL will be called by the pipeline components, so there is no need
	 * to worry about the packet transport at all. Note that the L as a postfix
	 * indicates that this method may "[L]eave", which is: it is allowed to throw
	 * an exception.
	 * @param pPacket the packet to be filtered
	 * @return NULL in case of a rejected/failed packet filtering, a valid packet (pPacket?) else
	 */
	virtual IDLVistaDataPacket *FilterPacketL(IDLVistaDataPacket * pPacket) = 0;

	virtual void ConsumePacket(IDLVistaDataPacket *pPacket );
	virtual IDLVistaDataPacket *PullPacket(bool bBlock = true);

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


	virtual bool AttachInputComponent(IDLVistaPipeComponent *pComp );
	virtual bool AttachOutputComponent(IDLVistaPipeComponent * pComp);
	virtual bool DetachInputComponent(IDLVistaPipeComponent * pComp);
	virtual bool DetachOutputComponent(IDLVistaPipeComponent *pComp );


	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);
	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);


	virtual std::list<IDLVistaPipeComponent *> GetInputComponents() const;

	virtual std::list<IDLVistaPipeComponent *> GetOutputComponents() const;

	virtual IDLVistaPipeComponent *GetOutboundByIndex(int iIndex) const ;

	virtual int GetNumberOfOutbounds() const ;


	virtual int GetNumberOfInbounds() const;

	virtual IDLVistaPipeComponent *GetInboundByIndex(int iIndex) const ;

	/**
	 * The return Packet method for filters simply forwards the return request
	 * to the outbound component.
	 */
	virtual IDLVistaDataPacket *ReturnPacket();

 };


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVISTAFILTER_H

