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


#include "VistaCSVDeSerializer.h"

#include <VistaAspects/VistaSerializable.h>


#include <limits>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaCSVDeSerializer::VistaCSVDeSerializer(char separator)
:m_cSeparator (separator)
{
}

VistaCSVDeSerializer::~VistaCSVDeSerializer()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int VistaCSVDeSerializer::ReadShort16( VistaType::ushort16 &us16Val)
{
	m_streamInput >> us16Val;
	if (m_streamInput.get()!=m_cSeparator)
	{
		if(m_streamInput.eof() == true)
			return sizeof(VistaType::ushort16);
		return -1;
	}
	else
		return sizeof(VistaType::ushort16);
}

int VistaCSVDeSerializer::ReadInt32( VistaType::sint32 &si32Val)
{
	m_streamInput >> si32Val;
	if (m_streamInput.get()!=m_cSeparator)
	{
		if(m_streamInput.eof() == true)
			return sizeof(VistaType::sint32);
		return -1;
	}
	else
		return sizeof(VistaType::sint32);
}

int VistaCSVDeSerializer::ReadInt32( VistaType::uint32 &si32Val)
{
	m_streamInput >> si32Val;
	if (m_streamInput.get()!=m_cSeparator)
	{
		if(m_streamInput.eof() == true)
			return sizeof(VistaType::uint32);
		return -1;
	}
	else
		return sizeof(VistaType::uint32);
}

 int VistaCSVDeSerializer::ReadInt64( VistaType::sint64 &si64Val)
{
	char buffer[4096];
	int iBIdx=0;

	// read from input util separator is found
	while((buffer[iBIdx] = m_streamInput.get()) != 0)
	{
		if(buffer[iBIdx] == m_cSeparator)
		{
			buffer[iBIdx]='\0'; // terminate
			break; // leave loop
		}
		++iBIdx;
	}

#if defined(WIN32)
	si64Val = _atoi64(buffer);
#else
	si64Val = atoi(buffer); // fix me
#endif
	return sizeof(VistaType::sint64);
}

 int VistaCSVDeSerializer::ReadUInt64( VistaType::uint64 &ui64Val)
{
	char buffer[4096];
	int iBIdx=0;

	// read from input util separator is found
	while((buffer[iBIdx] = m_streamInput.get()) != 0)
	{
		if(buffer[iBIdx] == m_cSeparator)
		{
			buffer[iBIdx]='\0'; // terminate
			break; // leave loop
		}
		++iBIdx;
	}

#if defined(WIN32)
	ui64Val = (VistaType::uint64)_atoi64(buffer);
#else
	ui64Val = atoi(buffer); // fix me
#endif
	return sizeof(VistaType::sint64);
}


 int VistaCSVDeSerializer::ReadFloat32( VistaType::float32 &fVal)
{
	m_streamInput >> fVal;
	if (m_streamInput.get()!=m_cSeparator)
	{
		if(m_streamInput.eof() == true)
			return sizeof(VistaType::float32);

		return -1;
	}
	else
		return sizeof(VistaType::float32);

}

 int VistaCSVDeSerializer::ReadFloat64( VistaType::float64 &f64Val)
{
	m_streamInput >> f64Val;
	if (m_streamInput.get()!=m_cSeparator)
	{
	if(m_streamInput.eof() == true)
		return sizeof(VistaType::float64);

		return -1;
	}
	else
		return sizeof(VistaType::float64);

}

 int VistaCSVDeSerializer::ReadDouble( double &dDoubleVal)
{
	m_streamInput >> dDoubleVal;
	if (m_streamInput.get()!=m_cSeparator)
	{
	if(m_streamInput.eof() == true)
		return sizeof(double);

		return -1;
	}
	else
		return sizeof(double);

}

int VistaCSVDeSerializer::ReadString(string &sIn, const int iMaxLen)
{
	if (iMaxLen > 0)
	{
		sIn.resize(iMaxLen);

		// don't forget terminating zero
		char* pString = new char [iMaxLen+1];
		m_streamInput.get (pString, iMaxLen+1, m_cSeparator);
		sIn.assign (pString, m_streamInput.gcount());
		delete [] pString;
	}

	if (m_streamInput.get()!=m_cSeparator)
		return -1;
	else
		return (int)sIn.length();

}

int VistaCSVDeSerializer::ReadDelimitedString(string &sIn, char cDelim)
{
	std::stringbuf buf;
	m_streamInput.get(buf, cDelim);
	sIn.assign(buf.str());

	if (m_streamInput.get()!=cDelim)
		return -1;
	else
		return (int)sIn.length();
}


int VistaCSVDeSerializer::ReadEncodedString( std::string& sString )
{
	VistaType::sint32 nSize;
	int nRet = ReadInt32( nSize );
	if( nRet != sizeof(VistaType::sint32 ) )
		return -1;
	nRet += ReadString( sString, nSize );
	return nRet;
}



int VistaCSVDeSerializer::ReadRawBuffer(void *pBuffer, int iLen)
{
	if (iLen>0)
	{
		// ignore m_cSeparator
		m_streamInput.get ((char*) pBuffer, iLen);
		iLen = (int)m_streamInput.gcount();
	}
	if (m_streamInput.get()!=m_cSeparator)
		return -1;
	else
		return iLen;

}

 int VistaCSVDeSerializer::ReadBool(bool &bVal)
{
	m_streamInput >> bVal;
	if (m_streamInput.get()!=m_cSeparator)
	{
	if(m_streamInput.eof() == true)
		return sizeof(bool);
		return -1;
	}
	else
		return sizeof(bool);

}

 int VistaCSVDeSerializer::ReadSerializable(IVistaSerializable &obj)
{
	return obj.DeSerialize(*this);
}

bool VistaCSVDeSerializer::FillBuffer(const std::string &strBuffer)
{

	m_streamInput.str (strBuffer);
	m_streamInput.seekg(0); // reset read head
	m_streamInput.clear(); // clear state flags
	return true;

}


bool VistaCSVDeSerializer::FillBuffer(char* pBuffer, int iLen)
{
	std::string strBuffer (pBuffer, iLen);
	return FillBuffer (strBuffer);
}

void VistaCSVDeSerializer::ClearBuffer()
{
	m_streamInput.str ("");
	m_streamInput.clear();
	m_streamInput.seekg(0);
}


bool VistaCSVDeSerializer::HasData() const
{
	return !m_streamInput.fail();
}

VistaSerializingToolset::ByteOrderSwapBehavior VistaCSVDeSerializer::GetByteorderSwapFlag() const
{
	return VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES;
}

void VistaCSVDeSerializer::SetByteorderSwapFlag( VistaSerializingToolset::ByteOrderSwapBehavior bDoesIt )
{

}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


