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


#ifndef DLVISTABYTEBUFFERPACKET_H
#define DLVISTABYTEBUFFERPACKET_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <string>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaByteBufferPacket : public IDLVistaDataPacket
{
private:
	std::string *m_pSBuffer;

public:
	DLVistaByteBufferPacket(IDLVistaPipeComponent *);

	virtual IDLVistaDataPacket* CreateInstance(IDLVistaPipeComponent* ) const;

	virtual ~DLVistaByteBufferPacket();

	const char * GetCharBuffer() const;
	std::string *GetStringBuffer() const;
	unsigned int GetCharBufferLength() const;

	void *GetByteBuffer() const;

	void AppendToBuffer(const char * );
	void AppendToBuffer(const std::string &);

	int GetPacketType() const;

	int GetPacketSize() const { return (int)(*m_pSBuffer).size(); };

	void SetDataSize(int);
	int GetDataSize() const;

	void SetBuffer(const char *);
	void SetBuffer(const std::string &);


	IDLVistaDataPacket * Clone() const;

	/**
	*	VISTA SERIALIZABLE MUST HAVES
	*/
	/**
	 * Think of this as "SAVE"
	 */
	int Serialize(IVistaSerializer &) const;
	/**
	 * Think of this as "LOAD"
	 */
	int DeSerialize(IVistaDeSerializer &);

	std::string GetSignature() const{
		return std::string("DLVistaByteBufferPacket");
	}
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //DLVISTABYTEBUFFERPACKET_H
