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


/*============================================================================*/
/* VistaConnection is an abstact class which provides a generic interface    */
/* to a connection interface. Classes of new types of connections shall be    */
/* derived from VistaConnection to provide the same functionality and the    */
/* same interface.                                                            */
/*                                                                            */
/*============================================================================*/

#ifndef _VISTACONNECTION_H
#define _VISTACONNECTION_H

#include <VistaInterProcComm/VistaInterProcCommConfig.h>

// Standard C++ includes
#include <string>
#include <iostream>

// system headers

#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>

class VistaConnection;

VISTAINTERPROCCOMMAPI std::ostream & operator<< (std::ostream &, const VistaConnection &);

/**
 * A connection is an alias for a "stream-like" instance. It is used to serve
 * as an abstract device-like-node which enables the user to "send" and "receive"
 * data without taking care of the actual transport protocol or hardware used.
 * Using this it should be possible to transport information across a serial
 * line as well as over an ip-connection both with the same interface.
 * Note that we are fully aware that these things are inherently very different
 * concepts which have their very own specialties (timing-constraints, package
 * sizes or transport types of their own), but this special knowledge it to
 * be encapsulated by VistaConnection classes in order to make the usage of
 * those streams as easy as possible.
 * Note again that we do use multiple inheritance for Serializer and DeSerializer
 * here in order to mark that the VistaConnection is incoming as well as outgoing
 * stream. We can do so, as the other interfaces are pure abstract and do not
 * interfere in their contexts.<br>
 * VistaConnections have to be Open()'ed in order to send or receive data; in case
 * the open fails, the send and receive methods are forced to fail. The VistaConnection
 * class is, too, responsible to do byte-swapping across different CPU-types (little and
 * big-endian systems), which is often necessary when using ip-services between different
 * host types (e.g. SUN and INTEL platforms). This feature can, though, be turned on and
 * off.
 * Again it is possible to use some strategies for data-retrieval like polling or
 * putting the request into "sleep"-mode, when waiting for data. The choice is up to
 * you. Send and receive tries can be done in "blocking"- and "buffering"-mode.
 * The first one will block the caller of any send and receive call until the data that
 * is to be sent is fully processed, the second one enables buffering strategies
 * which are specific to the implementation or transport layer and are beyond scope of
 * this interface. You can ONLY turn this feature on and off, but do not do any fine
 * tuning.
 */
class VISTAINTERPROCCOMMAPI VistaConnection : public IVistaSerializer, public IVistaDeSerializer
{
public:
	/**
	 * Constructor, sets open=false, swapbyteorder=true, blocking=true, buffering=false, readtimeout=0
	 */
	VistaConnection();

	/**
	 * Virtual destuctor. Needed to destroy objects of derived classes.
	 */
	virtual ~VistaConnection();

	virtual void Debug( std::ostream& oStream ) const;

	/**
	 * Open shall open a connection. Open connections shall not change their state
	 * when they are opened again.
	 *
	 * @return  true   if Open was successful
	 *          false  if Open failed
	 */
	virtual bool Open() = 0;

	/**
	 * Returns the current state of this connection, iff this thing is open or not.
	 * @return true iff this connection is open, false else.
	 */
	virtual bool GetIsOpen() const { return m_bIsOpen; }

	/**
	 * Close shall close the connection.
	 * After a call to close, send and receive methods will fail. It shall be safe to
	 * call close on a closed connection.
	 *
	 * @param bSkipData true: ignore data queued in connection
	 */
	virtual void Close() = 0;


	/**
	 * Receive shall read some information from the connection. Note that a call to this
	 * method can timeout, that means the call can return without |length|-read bytes.
	 * In order to avoid this, call SetBlocking(true) and Receive with a timeout of 0.
	 * Note that then, this call will ONLY return after |length|-bytes are really received
	 * which could "deadlock" the caller, when this does not happen.
	 *
	 * @param length   maximum length of bytes to be received |buffer| <= length
	 * @param  buffer   the received information
	 * @param iTimeout the timeout to wait until at least one byte will be available
	 *
	 * @return  number of received bytes, 0 on timeout without receival of any byte, -1 in case of total error
	 */
	virtual int  Receive( void* pBuffer, const int nLength, int iTimeout = 0 )=0;

	/**
	 * Send shall write some information to the connection
	 *
	 * @param   buffer   the information which shall be sent
	 * @param   length   the length of the information to send
	 *
	 * @return  the number of bytes that were actually sent or -1 on failure
	 */
	virtual int Send( const void* pBuffer, const int nLength )=0;


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
	 * @todo change this: make full virtual and specialize
	 */
	virtual bool HasPendingData() const { return true; }
	virtual unsigned long WaitForIncomingData(int timeout=0) { return 0; }
	virtual unsigned long WaitForSendFinish(int timeout=0) { return 0; }
	virtual unsigned long PendingDataSize() const { return 0; }
	virtual bool GetIsBlocking() const;
	virtual bool GetIsBuffering() const { return m_bIsBuffering; }
	virtual void SetIsBlocking( bool bBlocking ) { m_bIsBlocking = bBlocking; }
	virtual void SetIsBuffering( bool bBuffering ) { m_bIsBuffering = bBuffering; }

	/**
	 * Gets/Sets the read timeout that is used by default with every call to
	 * ReadXYZ - per default, it is set to 0 ( = wait forever )
	 */
	virtual int GetReadTimeout() const;
	virtual bool SetReadTimeout( int nReadTimeout );

	virtual bool Flush() { return false; }

	virtual bool GetIsFine() const;


	virtual HANDLE GetConnectionDescriptor() const;
	virtual HANDLE GetConnectionWaitForDescriptor();
	virtual bool GetWaitForDescriptorEventSelectIsEnabled() const;
	virtual bool SetWaitForDescriptorEventSelectIsEnabled( const bool bSet );

	enum eConState
	{
		CON_NONE = -1,
		CON_IO = 0,
		CON_CLOSE
	};
	virtual eConState DetermineConState() const;

	// ############################################################
	// IMPLEMENTATION : IVistaSerializer / IVistaDeSerializer
	// ############################################################

	virtual int WriteShort16(  VistaType::ushort16 us16Val );
	virtual int WriteInt32( VistaType::sint32 si32Val );
	virtual int WriteInt32( VistaType::uint32 si32Val );
	virtual int WriteInt64( VistaType::sint64 si64Val );
	virtual int WriteUInt64( VistaType::uint64 si64Val );
	virtual int WriteFloat32( VistaType::float32 fVal );
	virtual int WriteFloat64( VistaType::float64 f64Val );
	virtual int WriteDouble( double dValue );

	virtual int WriteRawBuffer( const void *pBuffer, const int iLen );
	virtual int WriteBool( bool bVal );

	virtual int WriteString( const std::string& sValue );
	virtual int WriteDelimitedString( const std::string&, char cDelim = '\0' );
	virtual int WriteEncodedString( const std::string& );

	virtual int WriteSerializable( const IVistaSerializable& );


	virtual int ReadShort16( VistaType::ushort16 &us16Val );
	virtual int ReadInt32( VistaType::sint32 &si32Val );
	virtual int ReadInt32( VistaType::uint32 &si32Val );

	virtual int ReadInt64( VistaType::sint64 &si64Val );
	virtual int ReadUInt64( VistaType::uint64 &si64Val );
	virtual int ReadFloat32( VistaType::float32 &fVal );
	virtual int ReadFloat64( VistaType::float64 &f64Val );
	virtual int ReadDouble( double &dDoubleVal );

	virtual int ReadRawBuffer( void *pBuffer, int iLen );
	virtual int ReadBool( bool &bVal );

	virtual int ReadString( std::string& sTarget, const int iMaxLen );
	virtual int ReadDelimitedString( std::string& sTarget, char cDelim = '\0' );
	virtual int ReadEncodedString( std::string& sTarget );

	virtual int ReadSerializable( IVistaSerializable & );


protected:

	/**
	 * Provides access to the open/close flag for specialized instances.
	 * @param bOpen the new state of this connection (open=true, closed=false)
	 */
	void SetIsOpen(bool bOpen);
private:
	/**
	 * m_bIsOpen indicates if a connection is opened.
	 */
	bool m_bIsOpen;

	/**
	 * indicates whether this connection cares for byte-order (little/big-endianess)
	 */
	VistaSerializingToolset::ByteOrderSwapBehavior  m_bDoesSwapByteOrder;

	/**
	 * indicates whether this connection is in blocking mode
	 */
	bool m_bIsBlocking;

	/**
	 * indicates whether this connections uses a buffering strategy or not.
	 */
	bool m_bIsBuffering;

	int m_nReadTimeout;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_CVISTACONNECTION_H
