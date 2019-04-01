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


#ifndef IDLVISTADATAPACKET_H
#define IDLVISTADATAPACKET_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaDLVTypes.h"
#include <VistaAspects/VistaSerializable.h>
#include <string>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaPipeComponent;
class DLVistaTimeStamp;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IDLVistaDataPacket : public IVistaSerializable
{
private:
	IDLVistaPipeComponent * m_pPacketSource;

	DLVistaTimeStamp *m_pTimeStamp;

	int m_iPacketType;

	bool m_bIsValid;

	bool m_bIsLocked;
protected:
	IDLVistaDataPacket(const IDLVistaDataPacket &);
public:

	IDLVistaDataPacket(IDLVistaPipeComponent * );


	virtual ~IDLVistaDataPacket();

	DLVistaTimeStamp *GetTimeStamp() const;

	virtual int GetPacketSize() const =0;

	IDLVistaPipeComponent * GetDataSource() const;

	virtual void SetDataSize(int ) =0;

	virtual int GetDataSize() const = 0;

	void Stamp(DLV_INT64 i64MicroStamp, DLV_INT32 i32MacroStamp);

	int GetPacketType() const;

	void SetPacketType(int iPacketType);

	virtual IDLVistaDataPacket* CreateInstance(IDLVistaPipeComponent*) const = 0;
	virtual IDLVistaDataPacket * Clone() const;

	bool IsValid() const;

	void SetIsValid(bool bValid);

	bool IsLocked() const;

	void Lock();

	bool Unlock();
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVISTADATAPACKET_H

