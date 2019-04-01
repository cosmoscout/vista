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


#include <cstring>
#include <cstdio>

#include <string>
#include <iostream>

#include "VistaByteBufferDeSerializer.h"

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaAspects/VistaSerializable.h>


using namespace std;


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaByteBufferDeSerializer::VistaByteBufferDeSerializer()
: m_bDoSwap( VistaSerializingToolset::GetDefaultPlatformSwapBehavior() ),
  m_pExternalBuffer(0), m_bDeleteAfterUsage(false)
{
}

VistaByteBufferDeSerializer::~VistaByteBufferDeSerializer()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaByteBufferDeSerializer::DoRead( VistaType::byte* pBuf, int iSize, bool bSwap )
{
	unsigned int iSwapSize = (unsigned int)iSize;

	if( !m_pExternalBuffer )
	{
		if( m_vecBuffer.size() < (unsigned int)iSize )
		{
			vstr::errp() << "VistaByteBufferDeSerializer::DoRead() -- "
					<< "Could only read [" << m_vecBuffer.size() << "] bytes - Requested: ["
					<< iSize << "]" << std::endl;
			iSize = (int)m_vecBuffer.size();
		}
		//read data from internal buffer
		for(int i=0; i < iSize; ++i)
		{
			pBuf[i] = m_vecBuffer[0];
			m_vecBuffer.pop_front();
		}
	}
	else
	{
		int nRemainingBytes = m_iExternalSize - m_iCurrentBufferPos;
		if( nRemainingBytes < iSize )
		{
			vstr::errp() << "VistaByteBufferDeSerializer::DoRead() -- "
					<< "Could only read [" << nRemainingBytes << "] bytes - Requested: ["
					<< iSize << "]" << std::endl;
			iSize = nRemainingBytes;
		}
		//read data from externally set buffer
		memcpy( pBuf, &(m_pExternalBuffer[m_iCurrentBufferPos]), iSize*sizeof(char) );
		m_iCurrentBufferPos += iSize;
	}

	if( bSwap && GetByteorderSwapFlag() )
		VistaSerializingToolset::Swap( (void*)pBuf, iSwapSize );
	return ( (int)iSwapSize==iSize );  // we indicate success iff we had enough bytes to read
}


VistaSerializingToolset::ByteOrderSwapBehavior  VistaByteBufferDeSerializer::GetByteorderSwapFlag() const
{
	return m_bDoSwap;
}

void VistaByteBufferDeSerializer::SetByteorderSwapFlag( VistaSerializingToolset::ByteOrderSwapBehavior  bDoesIt )
{
	m_bDoSwap = bDoesIt;
}

int VistaByteBufferDeSerializer::ReadShort16( VistaType::ushort16 &us16Val)
{
	if( !DoRead( reinterpret_cast<VistaType::byte*>( &us16Val ), sizeof(us16Val) ) )
		return -1;
	return sizeof(VistaType::ushort16);
}

int VistaByteBufferDeSerializer::ReadInt32( VistaType::sint32 &si32Val)
{
	if( !DoRead( reinterpret_cast<VistaType::byte*>( &si32Val ), sizeof(si32Val) ) )
		return -1;
	return sizeof(VistaType::sint32);
}

int VistaByteBufferDeSerializer::ReadInt32( VistaType::uint32 &si32Val)
{
	if( !DoRead( reinterpret_cast<VistaType::byte*>( &si32Val ), sizeof(VistaType::uint32) ) )
		return -1;
	return sizeof(VistaType::sint32);
}

 int VistaByteBufferDeSerializer::ReadInt64( VistaType::sint64 &si64Val)
{
	if( !DoRead( reinterpret_cast<VistaType::byte*>( &si64Val ), sizeof(si64Val) ) )
		return -1;
	return sizeof(si64Val);
}

int VistaByteBufferDeSerializer::ReadUInt64( VistaType::uint64 &ui64Val)
{
	if(!DoRead( reinterpret_cast<VistaType::byte*>( &ui64Val ), sizeof(ui64Val) ) )
		return -1;
	return sizeof(ui64Val);
}

 int VistaByteBufferDeSerializer::ReadFloat32( VistaType::float32 &fVal)
{
	if( !DoRead( reinterpret_cast<VistaType::byte*>( &fVal ), sizeof(fVal) ) )
		return -1;
	return sizeof(fVal);

}

 int VistaByteBufferDeSerializer::ReadFloat64( VistaType::float64 &f64Val)
{
	if(!DoRead( reinterpret_cast<VistaType::byte*>( &f64Val ), sizeof(f64Val) ) )
		return -1;
	return sizeof(f64Val);

}

 int VistaByteBufferDeSerializer::ReadDouble( double &dDoubleVal)
{
	if(!DoRead( reinterpret_cast<VistaType::byte*>( &dDoubleVal ), sizeof(dDoubleVal) ) )
		return -1;
	return sizeof(dDoubleVal);

}

 int VistaByteBufferDeSerializer::ReadString(string &sIn, const int iMaxLen)
{
	sIn.resize(iMaxLen);
	if(!DoRead( reinterpret_cast<VistaType::byte*>( &sIn[0] ), iMaxLen, false) )
		return -1;
	return iMaxLen;
}

 int VistaByteBufferDeSerializer::ReadDelimitedString( string &sString, char cDelim )
 {
	 sString.erase();

	 char pcTmp[2];
	 pcTmp[0] = 0x00;
	 pcTmp[1] = 0x00;

	 int iLength = 1;
	 int iLen = 0; /**< measure length */
	 for(;;)
	 {
		 int iRead=0;
		 if((iRead=ReadRawBuffer((void*)&pcTmp[0], iLength))==iLength)
		 {
			 if(pcTmp[0] == cDelim)
			 {
				 break; // leave loop
			 }
			 else
			 {
				 sString.append(string(&pcTmp[0]));
				 ++iLen;
			 }
		 }
		 else
		 {
			 vstr::errp() << "VistaByteBufferDeSerializer::ReadDelimitedString() -- "
				 << "Should read: " << iLength << ", but read: " << iRead << std::endl;
			 break;
		 }
	 }

	 return iLen;
 }

 
int VistaByteBufferDeSerializer::ReadEncodedString( std::string& sString )
{
	VistaType::sint32 nSize;
	int nRet = ReadInt32( nSize );
	if( nRet != sizeof( VistaType::sint32) )
		return -1;
	if( nSize == 0 )
	{
		sString.clear();
		return nRet;
	}
	nRet += ReadString( sString, nSize );
	return nRet;
}


int VistaByteBufferDeSerializer::ReadRawBuffer(void *pBuffer, int iLen)
{
	/** @todo check me */
	if( !DoRead( reinterpret_cast<VistaType::byte*>( pBuffer ), iLen, false ) )
		return -1;
	return iLen;

}

 int VistaByteBufferDeSerializer::ReadBool( bool &bVal )
{
	if( !DoRead(reinterpret_cast<VistaType::byte*>( &bVal ), sizeof(bVal) ) )
		return -1;
	return sizeof(bVal);

}

 int VistaByteBufferDeSerializer::ReadSerializable( IVistaSerializable &obj )
{
	return obj.DeSerialize(*this);
}

bool VistaByteBufferDeSerializer::FillBuffer( const VistaType::byte* pBuff, int iLength )
{
	ClearBuffer();
	for(int i=0; i < iLength; ++i)
	{
		m_vecBuffer.push_back( pBuff[i] ); /** @todo speed this up */
	}
	return true;
}

bool VistaByteBufferDeSerializer::SetBuffer( const VistaType::byte* pBuff, int iLength, bool bDeleteAfterUse )
{
	ClearBuffer();
	m_pExternalBuffer = pBuff;
	m_iExternalSize = iLength;
	m_iCurrentBufferPos = 0;
	m_bDeleteAfterUsage = bDeleteAfterUse;
	return true;
}

void VistaByteBufferDeSerializer::ClearBuffer()
{
	m_vecBuffer.clear();
	if(m_bDeleteAfterUsage)
		delete[] m_pExternalBuffer;
	m_iCurrentBufferPos = -1;
	m_bDeleteAfterUsage = false;
	m_pExternalBuffer = NULL;
}

unsigned int VistaByteBufferDeSerializer::GetTailSize() const
{
	if(!m_pExternalBuffer)
	{
		// using internal dequeu, tail size = number
		// of elements in queue
		return (int)m_vecBuffer.size();
	}
	else
	{
		// external buffer, rest to read is
		// iSize - curPos
		return m_iExternalSize - m_iCurrentBufferPos;
	}
}

const VistaType::byte* VistaByteBufferDeSerializer::GetBuffer() const
{
	if(m_pExternalBuffer)
		return m_pExternalBuffer;
	return &m_vecBuffer[0];
}

const VistaType::byte* VistaByteBufferDeSerializer::GetReadHead() const
{
	if(m_pExternalBuffer)
		return &m_pExternalBuffer[m_iCurrentBufferPos];
	return &m_vecBuffer[m_iCurrentBufferPos];
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


