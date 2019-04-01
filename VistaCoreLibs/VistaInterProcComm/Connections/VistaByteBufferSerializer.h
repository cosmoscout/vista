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


#ifndef _VISTABYTEBUFFERSERIALIZER_H
#define _VISTABYTEBUFFERSERIALIZER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaAspects/VistaSerializer.h>
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <vector>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaByteBufferSerializer : public IVistaSerializer
{
public:
	/**
	 * Specify the buffer size to use as a first guess, 1k is usually enough for
	 * most requests, and on the 1025s byte, the buffer gets doubled and copied
	 * usually it does not shrink ;) If you know that the message you are going
	 * to serialize will be greater than 1k, specify a proper value to avoid
	 * memory reallocation.
	 * @param uiInitialBufferSize determines the size of the initial buffer in bytes
	 */
	VistaByteBufferSerializer(unsigned int uiInitialBufferSize = 1024);


	virtual ~VistaByteBufferSerializer();
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

	/**
	 * Returns a pointer to the internal buffer. Be careful what you do with it! You should
	 * treat is _read only_. This API is meant for a quick access to legacy API, e.g.,
	 * network or file operations that work on byte buffers.
	 * @return a pointer to the internal buffer of this serializer. Consider it read only.
	 */
	const VistaType::byte* GetBuffer() const;

	/**
	 * Returns the size of the _serialized_ buffer, not the total size of the buffer that
	 * is serialized to!
	 * @return the number of bytes that make sense for the serialized stream
	 */
	int   GetBufferSize() const;

	/**
	 * A get-by-copy call API to retrieve the serialized buffer. The serialized buffer is
	 * _copied_ to the argument vector as VistaType::byte.
	 */

	void  GetBuffer( std::vector<VistaType::byte>& );

	virtual int WriteShort16(  VistaType::ushort16 us16Val) ;
	virtual int WriteInt32(  VistaType::sint32 si32Val) ;
	virtual int WriteInt32(  VistaType::uint32 si32Val) ;
	virtual int WriteInt64(  VistaType::sint64 si64Val) ;
	virtual int WriteUInt64(  VistaType::uint64 ui64Val) ;
	virtual int WriteFloat32(  VistaType::float32 fVal);
	virtual int WriteFloat64(  VistaType::float64 f64Val);
	virtual int WriteDouble(  double dVal) ;
	virtual int WriteRawBuffer( const void *pBuffer, const int iLen) ;
	virtual int WriteBool( bool bVal) ;

	virtual int WriteString( const std::string &sString) ;
	virtual int WriteDelimitedString( const std::string &sString, char cDelim = '\0') ;
	virtual int WriteEncodedString( const std::string &sString ) ;

	virtual int WriteSerializable(const IVistaSerializable &obj);

	/**
	 * Resets the internal accounting on the currently serialized data. This is a quick
	 * operation as it only resets some pointers and ints. No memory is flushed.
	 */
	void ClearBuffer();

	/**
	 * Retrims the size of an internal buffer that was _not_ set by SetBuffer().
	 * Use it to reshape the internal buffer, if you know that you need more or
	 * less memory. This operation can cause a reallocation, so previously serialized
	 * data might be lost.
	 * @param the new size of the internal buffer in bytes.
	 */
	void SetBufferCapacity(int iNewSize);


	/**
	 * Returns the current size in bytes of the internal buffer. Do not confuse
	 * this with the size of the serialized data that is contained in this
	 * buffer (this can be retrieved using GetBufferSize().
	 * @see GetBufferSize()
	 * @return the number of bytes that the internal buffer contains
	 */
	int  GetBufferCapacity() const;

	/**
	 * Tries to do a print out in a hex like format to stdout. Can be useful for
	 * debugging purposes but mostly it is not. Use a memory debugger if you
	 * experience trouble with lots of data. This API is meant for small
	 * data sizes.
	 */
	void DumpCurrentBuffer() const;

	/**
	 * API to adopt an external given buffer. Write attempts to this buffer will
	 * respect iBufferSize as bounds limit, so be sure that this value is correct.
	 * Note that any previously written stuff with -> this <- Serializer will be
	 * forgotten. You can call SetBuffer() with pvBuffer = NULL in order to return
	 * to the internal memory management. In that case, iBufferSize will determine
	 * the new capacity of the internal memory management. iWriteHead is ignored in
	 * the case of passing pvBuffer = NULL, the head is positioned at the begining
	 * of the internal buffer and old content will be overwritten.
	 * Note that once the buffer is full during write requests, new data will be dropped,
	 * no automagic memory resizing is done (in contrast to the use of the internal
	 * management that automatically reserves new room once the old is fully used).
	 * @param pvBuffer a pointer to a non-volatile memory region with a minimum size of iBufferSize
	 * @param iBufferSize size to use while deserializing
	 * @param iWriteHead can be used to pass an offset in the external buffer, where the serializing shall start
	 */
	void SetBuffer( VistaType::byte* pBuffer,
				   int iBufferSize,
				   int iWriteHead = 0);

	/**
	 * Sets the write position to the specified position (in bytes) for the next write operation.
	 * Thus, the next call to any Write*()-Api will not append data to the end of the buffer,
	 * but instead it will rewrite the data at the specified location, so use with care!
	 * An example for a sue-case is writing the size of the buffer as first entry, without knowing it
	 * when writing to the buffer starts. In this case, one can write a dummy as first entry, and then
	 * rewrite it after all data has been written and the size is known.
	 * The Rewrite will only be valid for a SINGLE write operation. If multiple data values
	 * have to be rewritten, the RewritePosition has to be set multiple times.
	 * Passing a negative value will disable a previously ordered override.
	 * @param byte index of the data to be rewritten
	 * @return true if value was successfully set (i.e. if param is negative (for un-setting)
	 *         or in the range of the buffer
	 */
	bool SetBufferRewritePosition( const int nOverridePosition );

protected:
	std::vector<VistaType::byte>	m_vecBuffer;
	int					m_iWriteHead;
	int					m_iOverwritePosition;
	VistaType::byte*	m_pHead;
	unsigned int		m_uiCapacity;
	VistaSerializingToolset::ByteOrderSwapBehavior m_bDoSwap;
	bool				m_bRetrimSize;

	// does it all
	int WriteValue( const VistaType::byte* pcValue, int iLength);
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTABYTEBUFFERSERIALIZER_H


