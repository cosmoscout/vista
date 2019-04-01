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


#ifndef _VISTACSVSERIALIZER_H
#define _VISTACSVSERIALIZER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaAspects/VistaSerializer.h>

#include <sstream>

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
 * VistaCSVSerializer
 *
 *  Serializer class for "character separated values" buffers. You can choose a separator character
 * ( ';' is standard ) and the serializer will write values to an output string stream
 *  separated by these separator characters.
 *
 *  There could be another implementation with a "marking" character, to mark occurrences of m_cSeparator
 *  in values. In the current implementation, m_cSeparator must not occur in any value !!!
 */
class VISTAINTERPROCCOMMAPI VistaCSVSerializer : public IVistaSerializer
{
private:

	std::ostringstream m_streamOutput;

	char               m_cSeparator;
protected:
public:
	//! @remark m_cSeparator must not occur in any value !!!
	VistaCSVSerializer(char separator = ';');

	virtual ~VistaCSVSerializer();

	const char* GetBuffer() const;
	void GetBufferString(std::string & strBuffer) const;
	int   GetBufferSize() const;

	virtual int WriteShort16(  VistaType::ushort16 us16Val);
	virtual int WriteInt32(  VistaType::sint32 si32Val) ;
	virtual int WriteInt32(  VistaType::uint32 si32Val) ;
	virtual int WriteInt64(  VistaType::sint64 si64Val) ;
	virtual int WriteUInt64(  VistaType::uint64 si64Val) ;
	virtual int WriteFloat32(  VistaType::float32 fVal);
	virtual int WriteFloat64(  VistaType::float64 f64Val);
	virtual int WriteDouble(  double dVal) ;
	virtual int WriteRawBuffer( const void *pBuffer, const int iLen) ;
	virtual int WriteBool( bool bVal) ;

	virtual int WriteString( const std::string &sString) ;	
	virtual int WriteDelimitedString( const std::string &sString, char cDelim = '\0');
	virtual int WriteEncodedString( const std::string& sString );

	virtual int WriteSerializable(const IVistaSerializable &obj);

	void ClearBuffer();
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACSVSERIALIZER_H


