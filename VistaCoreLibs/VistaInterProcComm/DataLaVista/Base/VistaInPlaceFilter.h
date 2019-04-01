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


#ifndef IDLVISTAINPLACEFILTER_H
#define IDLVISTAINPLACEFILTER_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaFilter.h"

#include <string>


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;
class DLVistaPacketQueue;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * An in-place filter is a special kind of filter that does only local modifications
 * to incoming packets before giving them to the oubound component. Modifications can
 * include things as modifying time-stamps, simply counting or profiling, or things
 * like content-tranformation (e.g. changing coordinates for device-packets and the like).
 * Subtypes should only need to define FilterPacketL() and return a non-NULL packet
 * to be delivered. Note that in-place filtering means, that<br>
 * <h3>p1 = (*this).FilterPacketL(p2) =&gt; (p1 == p2) == true</h3><br>
 * ('==' compares pointers, not content!)
 */
class VISTAINTERPROCCOMMAPI IDLVistaInPlaceFilter : public IDLVistaFilter
{
private:
	/**
	 * Copy-constructor, leave it alone ;)
	 * (we have not defined semantics for that)
	 */
	IDLVistaInPlaceFilter(IDLVistaInPlaceFilter &);

	std::string *m_pPacketName;
protected:
	/**
	 * Constructor, empty
	 */
	IDLVistaInPlaceFilter();

	void SetPacketTypeName(const std::string &);

public:
	/**
	 * Destructor, empty
	 */
	virtual ~IDLVistaInPlaceFilter();

	/**
	 * Simply passes pPacket to the inbound component.
	 * @param pPacket the packet to recycle
	 * @param bBlock indicates, whether the calling component shall be blocked until the packet could be returned successfully
	 * @return true iff no error occured and the packet was returned successfully
	 */
	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	/**
	 * Accepts an incoming packet and calls FilterPacketL().
	 * The Inplace Filter expects a non-NULL return form FilterPacketL(). If this
	 * is the case, the returned packet will be forwarded to the outbound pipe.
	 * If the FilterPacketL() does return NULL, then pPacket will be recycled
	 * using the inbound pipe and no packet will be forwarded to the outbound.
	 * @param pPacket the packet to filter
	 * @param bBlock true iff the incoming request shall block until a packet was delivered or none was filtered.
	 * @return true iff pPacket was successfully filtered and delivered to the outbound
	 */
	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	/**
	 * An in-place filter will ask its input via GivePacket() for a new packet,
	 * then it will filter this packet via FilterPacketL(). If this was successful,
	 * the filterered packet will be returned (which is the same as the input packet).
	 * In case filtering does not work out, the incoming packet will be sent back to
	 * the input for recycling via RecycleDataPacket().
	 * @param bBlock indicates whether the caller shall be blocked until a fresh packet could be retrieved from the input
	 * @return the new packet or NULL if something failed.
	 */
	IDLVistaDataPacket *GivePacket(bool bBlock);

	/**
	 * An in-place filter has (usually) no need to creat new packaged, so this will return
	 * NULL by default.
	 */
	IDLVistaDataPacket *CreatePacket();

	/**
	 * A call to this method on an in-place filter does not do anything.
	 * @param pPacket can by anything, as this is an empty method
	 */
	void                DeletePacket(IDLVistaDataPacket *pPacket);

	/**
	 * An in-place filter does (usually) not need any form of packet-management,
	 * so this is an empty method.
	 * @return always true
	 */
	bool                InitPacketMgmt();

	/**
	 * This component will accept any incoming packet, this method will return the type that the attached output will
	 * expect.
	 * @return this pipe's output component's input type or -1 iff this pipe is not connected
	 */
	virtual int GetInputPacketType() const;

	/**
	 * This component will forward incoming packets without modification, so this method will return the
	 * type that is given as input to this pipe
	 * @return this pipe's input component's output type ot -1 iff this pipe is not connected
	 */
	virtual int GetOutputPacketType() const;


};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVISTAINPLACEFILTER_H

