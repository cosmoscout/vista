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



#ifndef _VISTAPROPERTYLIST_H
#define _VISTAPROPERTYLIST_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaAspectsConfig.h"

#include <VistaAspects/VistaProperty.h>
#include <VistaAspects/VistaNameable.h>
#include <VistaAspects/VistaReferenceCountable.h>
#include <VistaAspects/VistaConversion.h>

#include <VistaBase/VistaVectorMath.h>
#include <VistaBase/VistaStreamUtils.h>

#include <map>
#include <string>
#include <list>
#include <vector>
#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSerializer;
class IVistaDeSerializer;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A PropertyList is-a map<string, VistaProperty>, as such feel free
 * to use all map-methods as you are used to.
 * A PropertyList does not maintain an internal
 * state, all the functions are "macros" is you like with additional
 * checking, if needed.
 */
class VISTAASPECTSAPI VistaPropertyList : public std::map<std::string,
									VistaProperty,
									VistaAspectsComparisonStuff::StringLessObject>
{
public:
	explicit VistaPropertyList( const bool bCaseSensitive = false );
	VistaPropertyList( const VistaPropertyList& oCopy );
	virtual ~VistaPropertyList();

	bool GetIsCaseSensitive() const;
	/**
	 * Converts the PropertyList from case-sensitivity to -insensitivity, or vice versa
	 * @param bConvertSubPropLists - if true, Sub-PorpertyLists of Properties
	 *        will be converted too
	 * @param bFailOnNameClash - if set to true, the conversion will abort and return false
	 *        if two entries in the current list would have identical keys after the conversion,
	 *        which is not allowed in maps. If false is returned, the state is not changed at all.
	 *        If bFailOnNameClash is false, a name clash will just result in a warning output.
	 */
	bool SetIsCaseSensitive( const bool bSet,
							const bool bConvertSubPropLists = true,
							const bool bFailOnNameClash = false );
						
	// GETTERS

	bool HasProperty( const std::string& sPropName ) const;
	bool HasSubList( const std::string& sPropName ) const;

	/**
	 * returns property list of a property
	 * throws exception - to get ref and add new prop, use operator[]
	 * @throws VistaExceptionBase if property does not exist
	 */
	VistaProperty GetPropertyCopy( const std::string& sPropName ) const;
	VistaProperty& GetPropertyRef( const std::string& sPropName );
	const VistaProperty& GetPropertyConstRef( const std::string& sPropName ) const;

	/**
	 * non-intrusive getter - like [], but doesn't create a new entry
	 * if none exists yet, and instead returns a new property
	 */
	VistaProperty operator() ( const std::string &sName ) const;

	/**
	 * Returns the type of the property with the specified name, or PROPT_NIL if it doesnt exist
	 */
	VistaProperty::ePropType GetPropertyType( const std::string& sPropName ) const;

	/**
	 * returns property list of a property
	 * @throws VistaExceptionBase if property does not exist, or is no propertylist prop
	 */
	VistaPropertyList GetSubListCopy( const std::string& sPropName ) const;
	VistaPropertyList& GetSubListRef( const std::string& sPropName );
	const VistaPropertyList& GetSubListConstRef( const std::string& sPropName ) const;


	template<class T>
	bool GetValue( const std::string& sPropName, T& oTarget ) const;
	template<class T>
	T GetValue( const std::string& sPropName, 
				VistaConversion::eConversionErrorHandling nOnError = VistaConversion::ON_ERROR_PRINT_WARNING ) const;

	template<class T>
	T GetValueOrDefault( const std::string& sPropName ) const;
	template<class T>
	T GetValueOrDefault( const std::string& sPropName, const T& oDefault ) const;
	template<class T>
	bool GetValueOrDefault( const std::string& sPropName, T& oTarget, const T& oDefault ) const;	

	template<int ArraySize, class T>
	bool GetValueAsArray( const std::string& sPropName, T aTarget[] ) const;
	
	template<class T>
	bool GetValueInSubList( const std::string& sPropName, const std::string& sSubList, T& oTarget ) const;
	template<class T>
	T GetValueInSubList( const std::string& sPropName, const std::string& sSubList,
				VistaConversion::eConversionErrorHandling nOnError = VistaConversion::ON_ERROR_PRINT_WARNING  ) const;

	template<class T>
	bool GetValueInSubListOrDefault( const std::string& sPropName, const std::string& sSubList,  T& oTarget, const T& oDefault ) const;
	template<class T>
	T GetValueInSubListOrDefault( const std::string& sPropName, const std::string& sSubList, const T& oDefault ) const;
	template<class T>
	T GetValueInSubListOrDefault( const std::string& sPropName, const std::string& sSubList ) const;

	

	// SETTER
	void SetProperty( const VistaProperty& oProperty );

	template<typename T>
	void SetValue( const std::string& sPropName, const T& oValue );

	template<int N, typename T>
	void SetValueFromArray( const std::string& sPropName, const T oValue[] );

	/**
	 * Sets a property in a sublist to the desired value
	 * fails if a property sSubList already exists, but is not of type PROPERTYLIST
	 */
	template<class T>
	bool SetValueInSubList( const std::string& sPropName, const std::string& sSubList, const T& oValue );

	void SetPropertyListValue( const std::string &sPropName, const VistaPropertyList& oList );

	/**
	 * @param sPropName the name of the property to remove
	 * @return true when a property with name sPropName() was removed, false if such a property
	 *         does not exist.
	 */
	bool RemoveProperty( const std::string &sPropName );


	// UTILITY
	static int DeSerializePropertyList( IVistaDeSerializer& rDeSer,
								   VistaPropertyList &rPropertyList,
								   std::string &sName);

	static int SerializePropertyList( IVistaSerializer& rSer,
								 const VistaPropertyList &rPropertyList,
								 const std::string &sPropertyListName);


	/**
	 * create a PropertyList that contains elements from oMaster,
	 * overwritten from oMergeIn. PropertyList elements will be recursively
	 * merged in. Elements that are only in oMergeIn will be in oMaster
	 * after this call. If oMaster has a PropertyList element named X and
	 * oMergeIn has the same PropertyList element named X, the list will
	 * be recursively copied, else, the value of oMergeIn will replace the
	 * one in oMaster. That means, if oMaster has a property Y which is
	 * non-PropertyList, and oMergeIn has a property named Y which is PropertyList,
	 * the returned PropertyList will after this call have a PropertyList property
	 * called Y which is an exact copy of oMergeIn.
	 */
	static VistaPropertyList MergePropertyLists( const VistaPropertyList& oMaster,
											 const VistaPropertyList& oMergeIn);

	/**
	 * merge THIS PropertyList with ioMergeIn.
	 */
	bool MergeWith( const VistaPropertyList& oMergeIn );

	
	/**
	 * Prints this PropertyList to out. Ignore iDepth, this is used
	 * for the recursive traversal during printouts.
	 * @param iIndentation indents a line with iIndentation number of whitespaces
	 *        resulting in more readable output.
	 */
	void Print( std::ostream& oOutStream = vstr::out(), int iIndentation = 0 ) const;

private:
	template<typename T>
	static void SetPropertyTypeForType( VistaProperty& oProp );
private:
	void SetStringValueTyped( const std::string &sPropName,
									const std::string &sValue,
									VistaProperty::ePropType eType,
									VistaProperty::ePropType eListType );	
	bool m_bIsCaseSensitive;
};

IVistaSerializer& operator<< ( IVistaSerializer& oSer, const VistaPropertyList& oList );

IVistaDeSerializer& operator>> ( IVistaDeSerializer& oDeSer, VistaPropertyList& oList );


/////////////////////////////////////////
//////  INLINE IMPLEMENTATIONS //////////
/////////////////////////////////////////

// outstream output
inline std::ostream& operator<<( std::ostream& oStream, const VistaPropertyList& oList )
{
	oList.Print( oStream );
	return oStream;
}

// template implementation
// default is string
template<typename T>
inline void VistaPropertyList::SetPropertyTypeForType( VistaProperty& oProp )
{
	oProp.SetPropertyType(  VistaProperty::PROPT_STRING );
}

// integer types
template<>
inline void VistaPropertyList::SetPropertyTypeForType<short>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_INT );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<unsigned short>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_INT );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<int>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_INT );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<unsigned int>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_INT );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<long int>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_INT );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<unsigned long int>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_INT );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<long long int>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_INT );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<unsigned long long int>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_INT );
}

// float types
template<>
inline void VistaPropertyList::SetPropertyTypeForType<float>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_DOUBLE );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<double>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_DOUBLE );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<long double>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_DOUBLE );
}

// bool
template<>
inline void VistaPropertyList::SetPropertyTypeForType<bool>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_BOOL );
}

// chars - we regard them as strings (specialization just for clarity)
template<>
inline void VistaPropertyList::SetPropertyTypeForType<char>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_STRING );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<signed char>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_STRING );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<unsigned char>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_STRING );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<wchar_t>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_STRING );
}

// math types
template<>
inline void VistaPropertyList::SetPropertyTypeForType<VistaVector3D>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_LIST );
	oProp.SetPropertyListSubType( VistaProperty::PROPT_DOUBLE );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<VistaQuaternion>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_LIST );
	oProp.SetPropertyListSubType( VistaProperty::PROPT_DOUBLE );
}
template<>
inline void VistaPropertyList::SetPropertyTypeForType<VistaTransformMatrix>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_LIST );
	oProp.SetPropertyListSubType( VistaProperty::PROPT_DOUBLE );
}

// proplist
template<>
inline void VistaPropertyList::SetPropertyTypeForType<VistaPropertyList>( VistaProperty& oProp )
{
	oProp.SetPropertyType( VistaProperty::PROPT_PROPERTYLIST );
}






template<class T>
inline bool VistaPropertyList::GetValue( const std::string& sPropName, T& oTarget ) const
{
	const_iterator itEntry = find( sPropName );
	if( itEntry == end() 
		|| (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_NIL
		|| (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST )
		return false;
	return VistaConversion::FromString( (*itEntry).second.GetValueConstRef(), oTarget );
}

template<int ArraySize, class T>
bool VistaPropertyList::GetValueAsArray( const std::string& sPropName, T aTarget[] ) const
{
	const_iterator itEntry = find( sPropName );
	if( itEntry == end() 
		|| (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_NIL
		|| (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST )
		return false;
	return VistaConversion::ArrayFromString<ArraySize>( (*itEntry).second.GetValueConstRef(), aTarget );
}
template<class T>
inline T VistaPropertyList::GetValue( const std::string& sPropName, 
									VistaConversion::eConversionErrorHandling nOnError ) const
{
	T oValue = VistaConversion::CreateInitializedDefaultObject<T>();
	if( GetValue<T>( sPropName, oValue ) == false )
	{
		if( nOnError & VistaConversion::ON_ERROR_PRINT_WARNING )
		{
			vstr::warnp() << "VistaPropertyList::GetValue<" << VistaConversion::GetTypeName<T>()
						<< ">( " << sPropName << " ) failed - ";
			const_iterator itEntry = find( sPropName );
			if( itEntry == end() )
			{
				vstr::warn() << "Property does not exist" << std::endl;
			}
			else if( (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_NIL )
			{
				vstr::warn() << "Property is NIL" << std::endl;
			}
			else if( (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST )
			{
				vstr::warn() << "Property is a Property List" << std::endl;
			}
			else
			{
				vstr::warn() << " Property value ["
						<< (*itEntry).second.GetValueConstRef() 
						<< "] could not be converted " << std::endl;
			}
		}
		if( nOnError & VistaConversion::ON_ERROR_THROW_EXCEPTION )
			VISTA_THROW( "Conversion failed!", -1 );
	}
	return oValue;
}

template<class T>
inline bool VistaPropertyList::GetValueOrDefault( const std::string& sPropName,
												 T& oTarget, const T& oDefault ) const
{
	if( GetValue( sPropName, oTarget ) )
		return true;

	oTarget = oDefault;
	return false;
}

template<class T>
inline T VistaPropertyList::GetValueOrDefault( const std::string& sPropName,
											  const T& oDefault ) const
{
	T oValue;
	if( GetValue( sPropName, oValue ) )
		return oValue;
	return oDefault;
}

template<class T>
inline T VistaPropertyList::GetValueOrDefault( const std::string& sPropName ) const
{
	T oValue;
	if( GetValue( sPropName, oValue ) )
		return oValue;
	return VistaConversion::CreateInitializedDefaultObject<T>();
}

template<class T>
inline bool VistaPropertyList::GetValueInSubList( const std::string& sPropName,
												 const std::string& sSubList,
												 T& oTarget ) const
{
	const_iterator itEntry = find( sSubList );
	if( itEntry == end() 
		|| (*itEntry).second.GetPropertyType() != VistaProperty::PROPT_PROPERTYLIST )
		return false;
	return (*itEntry).second.GetPropertyListConstRef().GetValue<T>( sPropName, oTarget );
}

template<class T>
inline T VistaPropertyList::GetValueInSubList( const std::string& sPropName, const std::string& sSubList,
											VistaConversion::eConversionErrorHandling nOnError  ) const
{
	const_iterator itEntry = find( sSubList );
	if( itEntry == end() || (*itEntry).second.GetPropertyType() != VistaProperty::PROPT_PROPERTYLIST )
	{
		if( nOnError & VistaConversion::ON_ERROR_PRINT_WARNING )
		{
			vstr::warnp() << "VistaPropertyList::GetValueInSubList<"
						<< VistaConversion::GetTypeName<T>()
						<< ">( " << sPropName << " ) failed - ";
			if( itEntry == end() )
			{
				vstr::warn() << "Prop does not exist" << std::endl;
			}
			else // if( (*itEntry).second.GetPropertyType() != VistaProperty::PROPT_NIL )
			{
				vstr::warn() << "Prop is no PropertyList" << std::endl;
			}
		}
		if( nOnError & VistaConversion::ON_ERROR_THROW_EXCEPTION )
			VISTA_THROW( "Conversion failed!", -1 );

		return T();
	}

	return (*itEntry).second.GetPropertyListConstRef().GetValue<T>( sPropName, nOnError );	
}

template<class T>
inline T VistaPropertyList::GetValueInSubListOrDefault( const std::string& sPropName,
														const std::string& sSubList,
														const T& oDefault ) const
{
	T oValue;
	if( GetValueInSubList( sPropName, sSubList, oValue ) )
		return oValue;
	return oDefault;
}

template<class T>
inline T VistaPropertyList::GetValueInSubListOrDefault( const std::string& sPropName,
														const std::string& sSubList ) const
{
	T oValue;
	if( GetValueInSubList( sPropName, sSubList, oValue ) )
		return oValue;
	return VistaConversion::CreateInitializedDefaultObject<T>();
}

template<class T>
inline bool VistaPropertyList::GetValueInSubListOrDefault( const std::string& sPropName,
														const std::string& sSubList,
														T& oTarget,
														const T& oDefault ) const
{
	if( GetValueInSubList( sPropName, sSubList, oTarget ) )
		return true;
	oTarget = oDefault;
	return false;
}

// specializations for PropertyLists
template<>
inline bool VistaPropertyList::GetValue( const std::string& sPropName,
													VistaPropertyList& oTarget ) const
{
	const_iterator itEntry = find( sPropName );
	if( itEntry == end() 
		|| (*itEntry).second.GetPropertyType() != VistaProperty::PROPT_PROPERTYLIST )
		return false;
	oTarget = (*itEntry).second.GetPropertyListConstRef();
	return true;
}
template<>
inline void VistaPropertyList::SetValue( const std::string& sPropName,
											const VistaPropertyList& oSource )
{
	VistaProperty& oProp = (*this)[sPropName];
	oProp.SetNameForNameable( sPropName );
	oProp.SetPropertyListValue( oSource );
	// Type already set by call above
	// oProp.SetPropertyType( VistaProperty::PROPT_PROPERTYLIST );
}

template<typename T>
inline void VistaPropertyList::SetValue( const std::string& sPropName, const T& oValue )
{
	std::string sValue;
	VistaConversion::ToString( oValue, sValue );
	VistaProperty& oProp = (*this)[sPropName];
	oProp.SetNameForNameable( sPropName );
	oProp.SetValue( sValue );
	SetPropertyTypeForType<T>( oProp );
}

template<class T>
inline bool VistaPropertyList::SetValueInSubList( const std::string& sPropName,
												 const std::string& sSubList,
												 const T& oValue )
{
	VistaProperty& oProp = (*this)[sSubList];
	oProp.SetNameForNameable( sSubList );
	if( oProp.GetPropertyType() == VistaProperty::PROPT_NIL )
		oProp.SetPropertyType( VistaProperty::PROPT_PROPERTYLIST );
	else if( oProp.GetPropertyType() != VistaProperty::PROPT_PROPERTYLIST )
		return false;
	oProp.GetPropertyListRef().SetValue<T>( sPropName, oValue );
	return true;
}

template<int N, typename T>
inline void VistaPropertyList::SetValueFromArray( const std::string& sPropName,
												 const T oValue[] )
{
	std::string sValue;
	VistaConversion::ArrayToString<N, T>( oValue, sValue );
	VistaProperty& oProp = (*this)[sPropName];
	oProp.SetNameForNameable( sPropName );
	oProp.SetValue( sValue );
	// tricking a little
	SetPropertyTypeForType<T>( oProp );
	if( oProp.GetPropertyType() != VistaProperty::PROPT_LIST )
		oProp.SetPropertyListSubType( oProp.GetPropertyType() );
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPROPERTYAWARE_H

