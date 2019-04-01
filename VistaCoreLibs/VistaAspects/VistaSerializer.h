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


#ifndef _VISTASERIALIZER_H
#define _VISTASERIALIZER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaAspectsConfig.h"

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaSerializingToolset.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSerializable;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This class serves as a simple base class for home-grown serialization methods.
 * Serializing means something like "preserving an object's state into a kind of
 * serial representation e.g. stream or similiar". The basic idea behind it is
 * that an object usually is composed of base-types (int, doubles etc.) or
 * other objects that are composed of basetypes at some level and so on. So usually
 * what you want to do is to send an object in a stream to a different machine
 * and assemble another instance with the same attribute values on this machine.
 * In our teminology all objects that are to be serialized have to be  of the base
 * type "IVistaSerializable" which does define the methods "Serialize" and "DeSerialize"
 * which do what is depicted above. They use the interface of a serializer or
 * deserialzer in order to streamline their basetypes or hand the serializer/desirializer
 * to their complex aggregates. Note that this mechanism sounds simple, but has its
 * very own advantages and disadvantages.<br>
 * This interface supports the idea of writing variables that are assigned to names
 * in order to enable a non-linear retrieval on the peer sides of this serializer.
 * So all methods are available either in the form of simple methods or with the
 * prefix "Name" to the method name (e.g. WriteInt32() and WriteInt32Name()). The first
 * one forces that any peer has to know the correct order of the incoming data, while
 * the other one allows that incoming data is stored in tables and retrieved later
 * on. Note that this is currently only the sketch of an idea and might not be appropriate
 * at all.
 */
class VISTAASPECTSAPI IVistaSerializer
{
private:
protected:

	/**
	 * Constructor. Note that serializers itself do not get constructed. This is
	 * only an interface.
	 */
	IVistaSerializer() {}
public:

	/**
	 * Destructor. Note that Serializers usually do not get deconstructed themselves
	 * This is only an interface.
	 */
	virtual ~IVistaSerializer() {}

	virtual VistaSerializingToolset::ByteOrderSwapBehavior GetByteorderSwapFlag() const = 0;
	virtual void SetByteorderSwapFlag( VistaSerializingToolset::ByteOrderSwapBehavior bDoesIt ) = 0;

	//! @deprecated call method with proper flag (see above)
	void SetByteorderSwapFlag( bool bDoesIt );


	/**
	 * This method is to write an 16bit short integer.
	 * @param us16val the value to be serialized
	 * @return the number of bytes serialized (should be 2) or -1 on failure
	 */
	virtual int WriteShort16(  VistaType::ushort16 us16Val) = 0;

	/**
	 * This method is to write an 32bit-integer.
	 * @param si32val the value to be serialized
	 * @return the number of bytes serialized (should be 4) or -1 on failure
	 */
	virtual int WriteInt32(  VistaType::sint32 si32Val) = 0;

	/**
	 * This method is to write an unsigned 32bit-integer.
	 * @param si32val the value to be serialized
	 * @return the number of bytes serialized (should be 4) or -1 on failure
	 */
	virtual int WriteInt32(  VistaType::uint32 ui32Val) = 0;

	/**
	 * This method is to write an 64bit-integer.
	 * @param si64val the value to be serialized
	 * @return the number of bytes serialized (should be 8) or -1 on failure
	 */
	virtual int WriteInt64(  VistaType::sint64 si64Val) = 0;

	/**
	 * This method is to write an 64bit-integer.
	 * @param ui64val the value to be serialized
	 * @return the number of bytes serialized (should be 8) or -1 on failure
	 */
	virtual int WriteUInt64(  VistaType::uint64 ui64Val) = 0;

	/**
	 * This method is to write an 32bit-float.
	 * @param fVal the value to be serialized
	 * @return the number of bytes serialized (should be 4) or -1 on failure
	 */
	virtual int WriteFloat32(  VistaType::float32 fVal) = 0;

	/**
	 * This method is to write an 64bit-float. Note that 64bit floats are equal to
	 * doubles in some cases.
	 * @param f64Val the value to be serialized
	 * @return the number of bytes serialized (should be 8) or -1 on failure
	 */
	virtual int WriteFloat64(  VistaType::float64 f64Val) = 0;

	/**
	 * This method is to write a double.
	 * @param dVal the value to be serialized
	 * @return the number of bytes serialized (should be sizeof(double)) or -1 on failure
	 */
	virtual int WriteDouble(  double dVal ) = 0;

	/**
	 * This method is to write a std::string, without forcing its encoding for the
	 * transport layer. Some implementations may chose to send the length as 32bit-int
	 * first prepended by the std::string itself, some may totally ignore the length or chose
	 * a very different approach
	 * @param sString the std::string to be written
	 * @return the number of bytes written or -1 on failure
	 */
	virtual int WriteString( const std::string &sString) = 0;

	/**
	 * This method is to write a std::string, without forcing its encoding for the
	 * transport layer.
	 * @param sString the std::string to be written
	 * @return the number of bytes written or -1 on failure
	 */
	virtual int WriteDelimitedString( const std::string &sString, char cDelim = '\0') = 0;

	/**
	 * This method is to write a std::string including a size encoding.
	 * @param sString the std::string to be written
	 * @return the number of bytes written or -1 on failure
	 */
	virtual int WriteEncodedString( const std::string &sString ) = 0;

	/**
	 * This method is to write a binary byte field of length iLen, without forcing its encoding for the
	 * transport layer. Some implementations may chose to send the length as 32bit-int
	 * first prepended by the byte field itself, some may totally ignore the length or chose
	 * a very different approach
	 * @param pBuffer the buffer to be written
	 * @param iLen the number of byte to be written beginning at pBuffer
	 * @return the number of bytes written or -1 on failure
	 */
	virtual int WriteRawBuffer( const void *pBuffer, const int iLen) = 0;

	/**
	 * This method is to write a bool value, without enforcing anything about the structure
	 * of the bool (bools may be 1-bit values on some systems).
	 * @param bVal the bool value to be written
	 * @return the number of bytes used for serializing or -1 on failure
	 */
	virtual int WriteBool( bool bVal) = 0;

	/**
	 * This method is called on the root-object of an serializable-object-graph that is to
	 * be serialized. The effect of calling this method should resolve in the one-by-one traversal
	 * of the object-graph, where every node has the chance to serialize itself with the
	 * help of the serializer given as an argument.
	 * @param rSerializable the root-object to serialize.
	 * @return -1 on failure, some other value > 0 else (semantics depend on the strategy of the serializer used)
	 */
	virtual int WriteSerializable(const IVistaSerializable &rSerializable) = 0;


	/**
	 * Convenience operator, calls WriteShort16
	 * @param val16 the value to be serialized
	 * @return *this
	 */
	virtual IVistaSerializer &operator<< (  VistaType::ushort16 nVal16 );

	/**
	 * Convenience operator, calls WriteInt32
	 * @param VistaType::val32 the value to be serialized
	 * @return *this
	 */
	virtual IVistaSerializer &operator<< (  VistaType::sint32 nVal32 );

	/**
	 * Convenience operator, calls WriteInt32
	 * @param VistaType::val32 the value to be serialized
	 * @return *this
	 */
	virtual IVistaSerializer &operator<< (  VistaType::uint32 nVal64 );

	/**
	 * Convenience operator, calls WriteInt32
	 * @param VistaType::val32 the value to be serialized
	 * @return *this
	 */
	virtual IVistaSerializer &operator<< (  VistaType::uint64 nVal64 );
	/**
	 * Convenience operator, calls WriteInt64
	 * @param VistaType::val64 the value to be serialized
	 * @return *this
	 */
	virtual IVistaSerializer &operator<< (  VistaType::sint64 nVal64 );

	/**
	 * Convenience operator, calls WriteFloat32
	 * @param fVal32 the value to be serailized
	 * @return *this
	 */
	virtual IVistaSerializer &operator<< (  VistaType::float32 fVal32 );

	/**
	 * Convenience operator, calls WriteFloat64
	 * @param fVal64 the value to be serialized
	 * @return *this
	 */
	virtual IVistaSerializer &operator<< (  VistaType::float64  fVal64 );

	/**
	 * Convenience operator, calls WriteString
	 * @param sVal the value to be serialized
	 * @return *this
	 */
	virtual IVistaSerializer &operator<< (  const std::string &sVal );

	/**
	 * Convenience operator, calls WriteBool
	 * @param bVal the value to be serialized
	 * @return *this
	 */
	virtual IVistaSerializer &operator<< (  bool bVal );

	virtual IVistaSerializer &operator<< ( VistaType::byte  );
};

#endif //_VISTASYSTEM_H
