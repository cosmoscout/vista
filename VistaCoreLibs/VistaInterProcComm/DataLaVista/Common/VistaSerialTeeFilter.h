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


#ifndef DLVISTASERIALTEEFILTER_H
#define DLVISTASERIALTEEFILTER_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaTeeFilter.h>

#include <map>
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


class VISTAINTERPROCCOMMAPI DLVistaSerialTeeFilter : public IDLVistaTeeFilter
{
public:

	DLVistaSerialTeeFilter();

	~DLVistaSerialTeeFilter();

	IDLVistaDataPacket *FilterPacketL(IDLVistaDataPacket *);

	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	virtual IDLVistaDataPacket * ReturnPacket();


protected:
	int GetPacketIndex(IDLVistaDataPacket *) const;
	void UpdatePacketIndex(IDLVistaDataPacket *pPacket, int iIndex);

private:
	typedef std::map<IDLVistaDataPacket *, int> PACKETMAP;
	PACKETMAP *m_mpPacketMap;
	VistaMutex *m_pMapMutex;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTATEEPIPE_H
