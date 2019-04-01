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


#ifndef DLVISTAPROFILINGFILTER_H
#define DLVISTAPROFILINGFILTER_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/DataLaVista/Base/VistaDLVTypes.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaInPlaceFilter.h>


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;
class IDLVistaRTC;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaProfilingFilter : public IDLVistaInPlaceFilter
{
public:
	DLVistaProfilingFilter();


	virtual ~DLVistaProfilingFilter();

	IDLVistaDataPacket *GivePacket(bool);
	IDLVistaDataPacket *CreatePacket();
	void                DeletePacket(IDLVistaDataPacket *);
	bool                InitPacketMgmt();

	/**
	 * This component will accept any incoming packet, so this method will always return 0.
	 */
	virtual int GetInputPacketType() const;

	/**
	 * This component will forward incoming packets without modification, so this will
	 * always return 0.
	 */
	virtual int GetOutputPacketType() const;

protected:
	IDLVistaDataPacket *FilterPacketL(IDLVistaDataPacket *pPacket);
private:
	DLV_INT64 m_i64MicroMean, m_i64MicroMin, m_i64MicroMax;
	DLV_INT32 m_i32MacroMean, m_i32MacroMin, m_i32MacroMax;

	DLV_INT32 m_i32PacketCount;

	IDLVistaRTC *m_pTimer;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTATOTEXTFILTER_H

