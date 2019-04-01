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


#ifndef _DLVISTADEMUX_H
#define _DLVISTADEMUX_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaTransformFilter.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <list>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * DLVistaDemux is a base class for demultiplexing filters.
 * It is used to synchronize input data coming from n different
 * (potentially active) sources and relays this data to one single output.
 *
 */
class VISTAINTERPROCCOMMAPI DLVistaDemux : public IDLVistaTransformFilter
{
public:
	DLVistaDemux();
	virtual ~DLVistaDemux();
	/**
	 * attach another input to this filter
	 */
	virtual bool AttachInputComponent(IDLVistaPipeComponent *pComp);
	/**
	 * remove the given pipe component from the demux
	 */
	virtual bool DetachInputComponent(IDLVistaPipeComponent *pComp);
	/*
	 * Get the n-th inbound component
	 *
	 * NOTE: Due to the const identifier, it is impossible to lock
	 *       internally. Thus this method is NOT THREADSAFE!
	 */
	virtual IDLVistaPipeComponent *GetInboundByIndex(int i) const;
	/*
	 * Check if the given component is an input to this filter.
	 *
	 * NOTE: Due to the const identifier, it is impossible to lock
	 *       internally. Thus this method is NOT THREADSAFE!
	 */
	virtual bool IsInputComponent(IDLVistaPipeComponent *pIn ) const;
	/**
	 * accept a packet from the input designated by pComp and pass
	 * it to the downstream component
	 */
	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock);
	/**
	 * fetch one packet from the "next" upstream component that has one in a
	 * round robin fashion. Pass this packet to the oubound (calling) component.
	 */
	virtual IDLVistaDataPacket *GivePacket(bool bBlock);
protected:

private:
	DLVistaDemux(const DLVistaDemux &);
	/**
	 * handle multiple inputs
	 */
	std::list<IDLVistaPipeComponent*> m_liInputs;
	/**
	 * circularly walk through the inputs to handle packets
	 */
	std::list<IDLVistaPipeComponent*>::iterator m_itNextInput;
	/**
	 * we'll have to sync access to the input/output
	 */
	VistaMutex m_oFilterLock;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //IDLVISTAFILTER_H

