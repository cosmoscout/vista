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


#ifndef _VISTACSVDESERIALIZER_H
#define _VISTACSVDESERIALIZER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaAspects/VistaDeSerializer.h>

#include <sstream>
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

/**
 * VistaCSVDeSerializer
 *
 *  DeSerializer class for "character separated values" buffers. You can choose a separator character
 * ( ';' is standard ) and the serializer will read values from an input string stream
 *  separated by these separator characters.
 *
 *  There could be another implementation with a "marking" character, to mark occurrences of m_cSeparator
 *  in values. In the current implementation, m_cSeparator must not occur in any value !!!
 *
 */

class VISTAINTERPROCCOMMAPI VistaCSVDeSerializer : public IVistaDeSerializer
{
	private:

	std::istringstream m_streamInput;

	char               m_cSeparator;

	protected:
	public:

	// Remark:
	// m_cSeparator must not occur in any value !!!
	// otherwise, the DeSerializer will have problems separating values (apparently)
	VistaCSVDeSerializer(char separator = ';');
	virtual ~VistaCSVDeSerializer();

	virtual VistaSerializingToolset::ByteOrderSwapBehavior GetByteorderSwapFlag() const;
	virtual void SetByteorderSwapFlag( VistaSerializingToolset::ByteOrderSwapBehavior bDoesIt );

	virtual int ReadShort16( VistaType::ushort16 &us16Val);

	virtual int ReadInt32( VistaType::sint32 &si32Val);
	virtual int ReadInt32( VistaType::uint32 &si32Val);

	virtual int ReadInt64( VistaType::sint64 &si64Val);
	virtual int ReadUInt64( VistaType::uint64 &ui64Val);

	virtual int ReadFloat32( VistaType::float32 &fVal);
	virtual int ReadFloat64( VistaType::float64 &f64Val);

	virtual int ReadDouble( double &dDoubleVal);

	// this will read RAW, ignoring all separators, be sure the length is right  !!!
	virtual int ReadRawBuffer(void *pBuffer, int iLen);

	virtual int ReadBool(bool &bVal) ;

	virtual int ReadString(std::string &sIn, const int iMaxLen);
	virtual int ReadDelimitedString(std::string &, char cDelim = '\0');
	virtual int ReadEncodedString(std::string &);

	virtual int ReadSerializable(IVistaSerializable &obj) ;


	bool FillBuffer(const std::string &strBuffer);
	bool FillBuffer(char* pBuffer, int iLen);

	void ClearBuffer();
	bool HasData() const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VceDESERIALIZER_H
