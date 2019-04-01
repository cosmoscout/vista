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


#ifndef IDLVISTADATACONSUMER_H
#define IDLVISTADATACONSUMER_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaPipeComponent.h"

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IDLVistaDataConsumer : public IDLVistaPipeComponent
{
private:
protected:
	IDLVistaPipeComponent * m_pDataInput;

	IDLVistaDataConsumer();
public:

	virtual ~IDLVistaDataConsumer() = 0;

	virtual bool ConsumePacket(IDLVistaDataPacket *) =0;

	virtual bool PullPacket(bool bBlock = true) =0;

	virtual bool IsDataProducer() const { return false;}

	virtual bool IsDataConsumer() const { return true; }

	virtual bool IsOutputComponent(IDLVistaPipeComponent *pComp) const { return false; /* we do not have no output */ };

	virtual bool IsInputComponent(IDLVistaPipeComponent *pComp) const;

	virtual bool AttachInputComponent(IDLVistaPipeComponent *pComp);

	virtual bool AttachOutputComponent(IDLVistaPipeComponent *pComp);

	virtual bool DetachInputComponent(IDLVistaPipeComponent *pComp);

	virtual bool DetachOutputComponent(IDLVistaPipeComponent *pComp);

	virtual IDLVistaPipeComponent *GetOutboundByIndex(int iIndex) const;

	virtual IDLVistaPipeComponent *GetInboundByIndex(int iIndex) const;

	virtual int GetNumberOfOutbounds() const;

	virtual int GetNumberOfInbounds() const;


	virtual std::list<IDLVistaPipeComponent *> GetInputComponents() const;

	virtual std::list<IDLVistaPipeComponent *> GetOutputComponents() const;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVISTADATACONSUMER_H

