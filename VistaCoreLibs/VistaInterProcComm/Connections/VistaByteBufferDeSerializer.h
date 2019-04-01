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


#ifndef _VISTABYTEBUFFERDESERIALIZER_H
#define _VISTABYTEBUFFERDESERIALIZER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaAspects/VistaDeSerializer.h>

#include <deque>
#include <string>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaConnectionIP;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAINTERPROCCOMMAPI VistaByteBufferDeSerializer : public IVistaDeSerializer
{
public:
	VistaByteBufferDeSerializer();
	virtual ~VistaByteBufferDeSerializer();

	/**
	 * Returns whether this connctions cares for byte-order of the sent-data or not.
	 * Note that enabling this feature might have impact on the runtime-behaviour, efficiency
	 * and even more.
	 * @return true iff this connection cares for the byte-order of the sent or received data
	 * @see SetByteorderSwapFlag()
	 */
	VistaSerializingToolset::ByteOrderSwapBehavior GetByteorderSwapFlag() const;

	/**
	 * Sets whether this connection cares for byte-order or not.
	 * Note that enabling this feature might have impact on the runtime-behaviour, efficiency
	 * and even more.
	 * @see GetByteorderSwapFlag()
	 * @param bDoesIt true iff byte-order is significant for this class, false else
	 */
	void SetByteorderSwapFlag( VistaSerializingToolset::ByteOrderSwapBehavior bDoesIt );

	virtual int ReadShort16( VistaType::ushort16 &us16Val);

	virtual int ReadInt32( VistaType::sint32 &si32Val);
	virtual int ReadInt32( VistaType::uint32 &si32Val);

	virtual int ReadInt64( VistaType::sint64 &si64Val);
	virtual int ReadUInt64( VistaType::uint64 &ui64Val);

	virtual int ReadFloat32( VistaType::float32 &fVal);
	virtual int ReadFloat64( VistaType::float64 &f64Val);

	virtual int ReadDouble( double &dDoubleVal);

	virtual int ReadRawBuffer(void *pBuffer, int iLen);

	virtual int ReadBool(bool &bVal) ;

	virtual int ReadString(std::string &sIn, const int iMaxLen) ;
	virtual int ReadDelimitedString(std::string &, char cDelim = '\0');
	virtual int ReadEncodedString(std::string &);

	virtual int ReadSerializable(IVistaSerializable &obj) ;


	/**
	 *  FillBuffer() _copies_ iLength bytes of the VistaType::byte vector given in pcBuff to its internal
	 *  buffer. Any external associated buffer will be deleted (if stated by the user upon
	 *  assigning the external buffer with SetBuffer()).
	 */
	bool FillBuffer( const VistaType::byte* pBuff, int iLength );

	/**
	* Set the buffer to a given memory location. This will save the costly copy operation
	* at the expense of reduced "safety". The bDeleteAfterUse-Flag indicates
	* wheter ownership of the given memory area is passed to the deserializer
	* i.e. iff the deserializer will delete the given memory when the buffer is no longer
	* needed.
	*/
	bool SetBuffer( const VistaType::byte* pBuff, int iLength, bool bDeleteAfterUse = false );
	const VistaType::byte* GetBuffer() const;
	const VistaType::byte* GetReadHead() const;

	void ClearBuffer();

	unsigned int GetTailSize() const;

private:
	bool DoRead( VistaType::byte* pBuf, int iSize, bool bSwap = true );
private:
	VistaSerializingToolset::ByteOrderSwapBehavior m_bDoSwap;

	//members for externally set buffer space...
	const VistaType::byte* m_pExternalBuffer;
	int m_iExternalSize;
	int m_iCurrentBufferPos;
	bool m_bDeleteAfterUsage;

	std::deque<VistaType::byte> m_vecBuffer;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTABYTEBUFFERDESERIALIZER_H

