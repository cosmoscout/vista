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


#ifndef _VISTAREFLECTIONABLE_H
#define _VISTAREFLECTIONABLE_H

#include "VistaAspectsConfig.h"

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaObserveable.h"
#include "VistaPropertyAwareable.h"
#include "VistaNameable.h"
#include "VistaAspectsUtils.h"

#include "VistaPropertyFunctor.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaPropertyFunctorRegistry;


/*============================================================================*/
/* LOCAL DECLARATIONS                                                         */
/*============================================================================*/

#define REFL_INLINEIMP(classname, superclass) \
	public:\
	std::string GetReflectionableType() const \
	{\
		return #classname;\
	}\
	protected: \
	int AddToBaseTypeList(std::list<std::string> &rBtList) const \
	{ \
		int nRet = superclass::AddToBaseTypeList(rBtList); \
		rBtList.push_back(#classname);\
		return ++nRet; \
	}

#define REFL_DECLARE \
	public: \
	std::string GetReflectionableType() const; \
	protected: \
	int AddToBaseTypeList(std::list<std::string> &rBtList) const;

#define REFL_IMPLEMENT(classname, superclass) \
	std::string classname::GetReflectionableType() const \
	{\
		return #classname;\
	}\
	\
	int classname::AddToBaseTypeList(std::list<std::string> &rBtList) const \
	{ \
		int nRet = superclass::AddToBaseTypeList(rBtList); \
		rBtList.push_back(#classname);\
		return ++nRet; \
	}

#define REFL_IMPLEMENT_FULL(classname, superclass) \
	namespace { std::string SsReflectionName( #classname ); } \
	std::string classname::GetReflectionableType() const \
	{\
		return #classname;\
	}\
	\
	int classname::AddToBaseTypeList(std::list<std::string> &rBtList) const \
	{ \
		int nRet = superclass::AddToBaseTypeList(rBtList); \
		rBtList.push_back(#classname);\
		return ++nRet; \
	}


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A type implementing IVistaReflectionable is PropertyAwareable,
 * Observeable and Nameable in the ViSTA sense.
 */
class VISTAASPECTSAPI IVistaReflectionable : public IVistaPropertyAwareable,
							 public IVistaObserveable,
							 public IVistaNameable

{
public:
	virtual ~IVistaReflectionable();

	/**
	 * Override this method in subclasses. This is an absolute
	 * must. Use the macros REFL_INLINEIMP() in order to
	 * do so automatically. This method is called by GetBaseTypeList()
	 * The type name should not change during the application run.
	 * @return the application-wide unique type identifier for
	 *         the reflectionable specialization.
	 */
	virtual std::string GetReflectionableType() const;

	//#################################################
	// NAMEABLE-INTERFACE
	//#################################################

	std::string GetNameForNameable() const;
	void   SetNameForNameable(const std::string &sName);


	/**
	 * Wrapper method for SetNameForNameable() (returns a bool
	 * instead of void)
	 * @return true iff sName changed the previous name
	 */
	bool   SetNameProp(const std::string &sName);

	//#################################################
	// PROPERTYAWAREABLE-INTERFACE
	//#################################################
	/**
	 * slightly changed behavior for reflectionables than
	 * for PropertyAwareables for SetProperty().
	 * @param oProp a reference to a property,
			  with a name set (oProp.GetName() != empty_string)
	 * @return PROPT_OK when the set functor
			   for the property returned true.
			   SetFunctors indicate true when
			   - the syntax was correct
			   - the new value replaced the old value
				 (meaning: was really a new value)
	 * @see IVistaPropertyGetFunctor()
	 */
	virtual int SetProperty(const VistaProperty &oProp);
	virtual int GetProperty(VistaProperty &);
	virtual VistaProperty GetPropertyByName(const std::string &sPropName);
	virtual bool SetPropertyByName(const std::string &sPropName,
								   const std::string &sPropValue);

	/**
	 * @return the number of changed properties from the PropertyList
			   in total.
	 */
	virtual int SetPropertiesByList(const VistaPropertyList &);
	virtual int GetPropertiesByList(VistaPropertyList &);

	virtual int GetPropertySymbolList(std::list<std::string> &rStorageList);
	virtual std::string GetPropertyDescription(const std::string &sPropName);

	/**
	 * Alias to GetBaseTypes(), but returns the list of parent
	 * type names on the stack (slightly more expensive, but convenient).
	 * @return a list of type names for the base types of this
			   IVistaReflectionable. The names are ordered to
			   indicate the inheritance chain from most abstract
			   to most specific.
	 * @see GetBaseTypes()
	 */
	std::list<std::string> GetBaseTypeList() const;

	/**
	 * Determines the base type names for this IVistaReflectionable
	 * as a sorted list from most abstract to most specific.
	 * The list is built up on the first call to this method and
	 * cached, so type names must not change during the application
	 * run. As the type names are defined by the subclass of the
	 * the reflectionable, it is important to keep them unique (hey,
	 * these are type names ;) and readable.
	 * @return the number of strings in rList
	 * @param rList a list to store the type names in
	 */
	int GetBaseTypes(std::list<std::string> &rList) const;

	enum
	{
		MSG_SETCHANGE = IVistaObserveable::MSG_LAST, /**< sent after SetPropsByList */
		MSG_NAMECHANGE,                              /**< sent upon SetNameForNameable() call */
		MSG_LAST
	};

	IVistaReflectionable(const IVistaReflectionable &);
	IVistaReflectionable &operator=(const IVistaReflectionable &);
protected:
	IVistaReflectionable();

	/**
	 *  This method <b>must</b> be overloaded by subclasses in order
	 * to build a property hierarchy of base-type strings (i.e. strings
	 * that encode a unique class identifier). This Method will add its
	 * base type std::string as vanilla std::string to the rBtList.
	 * @return the number of entries in the list
	 * @param rBtList the list storage to keep track of base types
	 */
	virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;
private:
	void ValidateBaseTypeList();

	std::string            m_sName;
	std::list<std::string> m_liBaseTypeList;
};

namespace VistaReflectionableUtil
{
	VISTAASPECTSAPI void ReleaseProperties();
}


template<class T> bool compAssignAndNotify(const T x, T &y, IVistaReflectionable &oObj, int nMsg)
{
	if( x != y )
	{
		y = x;
		oObj.Notify(nMsg);
		return true;
	}
	return false;
}


/**
 * TVistaPropertyGet implements a templated getter functor for a
 * IVistaReflectionable object. It should save you the job of writing an
 * own functor in most of the cases.
 * The meaning of the template arguments is as follows:
 *
 * R       return type of getter method
 * C	   reflectionable for which to use/register the GetFunctor
 *
 */
template<class R, class C, VistaProperty::ePropType nPropType = VistaProperty::PROPT_STRING>
class TVistaPropertyGet : public IVistaPropertyGetFunctor
{
public:
	typedef R (C::*GetterFn)() const;

	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfGetter	pointer to the reflectionable's get method to be used
	* @param	sDescription Descriptive string
	*/
	TVistaPropertyGet( const std::string &sPropName,
						const std::string &sClassType,
						GetterFn pfGetter,
						const std::string &sDescription = "" )
	: IVistaPropertyGetFunctor( sPropName, sClassType, sDescription )
	, m_pfGetter( pfGetter )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		R rTmp = ( static_cast<const C&>(rObj).*m_pfGetter )();
		rProp.SetValue( VistaConversion::ToString<R>( rTmp ) );
		rProp.SetPropertyType(nPropType);
		return !rProp.GetIsNilProperty();
	}

protected:
	GetterFn		m_pfGetter;
};


/**
 * TVistaPropertyConvertAndGet implements a templated getter functor for a
 * IVistaReflectionable object. It should save you the job of writing an
 * own functor in most of the cases. This Version allows providing
 * a custim to-string conversion function
 * The meaning of the template arguments is as follows:
 *
 * R       return type of getter method
 * ConvArg Argument of conversion function
 * C	   reflectionable for which to use/register the GetFunctor
 *
 */
template<class R, class ConvArg, class C, VistaProperty::ePropType nPropType = VistaProperty::PROPT_STRING>
class TVistaPropertyConvertAndGet : public IVistaPropertyGetFunctor
{
public:
	typedef R (C::*GetterFn)() const;
	typedef std::string (*ConvertFn)( ConvArg );

	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfGetter	pointer to the reflectionable's get method to be used
	* @param	pfConert	pointer to ToString Conversion functions
	* @param	sDescription Descriptive string
	*/
	TVistaPropertyConvertAndGet( const std::string &sPropName,
						const std::string &sClassType,
						GetterFn pfGetter,
						ConvertFn pfConvert,
						const std::string &sDescription = "" )
	: IVistaPropertyGetFunctor( sPropName, sClassType, sDescription )
	, m_pfGetter( pfGetter )
	, m_pfConvert( pfConvert )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		R rTmp = ( static_cast<const C&>(rObj).*m_pfGetter )();
		rProp.SetValue( (*m_pfConvert)( rTmp ) );
		rProp.SetPropertyType(nPropType);
		return !rProp.GetIsNilProperty();
	}

protected:
	GetterFn		m_pfGetter;
	ConvertFn		m_pfConvert;
};


/**
 * TVistaPropertySet implements a templated SetFunctor for a
 * IVistaReflectionable object. It should save you the job of writing an
 * own functor in most of the cases.
 * The meaning of the template arguments is as follows:
 *
 * - Arg    input type to reflectionable's setter method
 * - R      Actual type of the function (Arg withput qualifiers)
 * - C	    reflectionable for which to use/register the GetFunctor
 *
 * map functions like<br>
 * bool C::SetMyFoo( const CMySpecialClass & );<br>
 * with<br>
 * TVistaPropertyGet<const CMySpecialClass &, CMySpecialClass, C>(
 *               "MYFOO", SsReflectionName, &C::SetMyFoo )<br>
 */
template <class Arg, class R, class C>
class TVistaPropertySet : public IVistaPropertySetFunctor
{
	typedef bool (C::*SetterFunc)( Arg );
public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfSetter	pointer to the reflectionable's set method to be used
	*/
	TVistaPropertySet(const std::string &sPropName,
					const std::string &sClassType,
					SetterFunc pfSetter,
					const std::string &sDescription = "" )
	: IVistaPropertySetFunctor(sPropName, sClassType, sDescription)
	, m_pfSetter( pfSetter )
	{
	}
	virtual bool operator()( IVistaPropertyAwareable &rObj, const VistaProperty &rProp )
	{

		R oTmp;
		if( VistaConversion::FromString<R>( rProp.GetValue(), oTmp ) == false )
			return false;
		return (static_cast<C&>(rObj).*m_pfSetter)(oTmp);
	}


protected:
	SetterFunc		m_pfSetter;
};


/**
 * TVistaPropertySet implements a templated SetFunctor for a
 * IVistaReflectionable object. It should save you the job of writing an
 * own functor in most of the cases.
 * The meaning of the template arguments is as follows:
 *
 * - Arg    input type to reflectionable's setter method
 * - CRes   return type of the Conversion Method
 * - C	    reflectionable for which to use/register the GetFunctor
 
 */
template <class Arg, class CRes, class C>
class TVistaPropertyConvertAndSet : public IVistaPropertySetFunctor
{
	typedef bool (C::*SetterFunc)( Arg );
	typedef CRes (*ConvertFunc)( const std::string& );
public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfSetter	pointer to the reflectionable's set method to be used
	* @param	pfConvert	pointer to the rFromSting conversion function
	*/
	TVistaPropertyConvertAndSet( const std::string &sPropName,
					const std::string &sClassType,
					SetterFunc pfSetter,
					ConvertFunc pfConvert,
					const std::string &sDescription = "" )
	: IVistaPropertySetFunctor(sPropName, sClassType, sDescription)
	, m_pfSetter( pfSetter )
	, m_pfConvert( pfConvert )
	{
	}
	virtual bool operator()( IVistaPropertyAwareable &rObj, const VistaProperty &rProp )
	{

		CRes oTmp = (*m_pfConvert)(	rProp.GetValue() );
		return (static_cast<C&>(rObj).*m_pfSetter)(oTmp);
	}


protected:
	SetterFunc		m_pfSetter;
	ConvertFunc		m_pfConvert;
};


template<class C>
class TVistaPropertyListGet : public IVistaPropertyGetFunctor
{
	typedef VistaPropertyList (C::*GetterFn)() const;
public:
	TVistaPropertyListGet( const std::string &sPropName,
							const std::string &sClassName,
							GetterFn pfGetter,
							const std::string &sDescription = "" )
	: IVistaPropertyGetFunctor(sPropName, sClassName, sDescription)
	, m_pfGetter( pfGetter )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		VistaPropertyList rProps = (static_cast<const C&>(rObj).*m_pfGetter)();
		rProp.SetPropertyListValue(rProps);
		rProp.SetPropertyType(VistaProperty::PROPT_PROPERTYLIST);
		return true;
	}

protected:
	GetterFn	m_pfGetter;
};

template<class C> class TVistaPropertyListSet : public IVistaPropertySetFunctor
{
public:
	typedef bool (C::*SetterFunc)(const VistaPropertyList &);

	TVistaPropertyListSet(const std::string &sName,
							const std::string &sClassName,
							SetterFunc pfSetter,
							const std::string &sDescription = "")
	: IVistaPropertySetFunctor(sName, sClassName, sDescription)
	, m_pfSetter( pfSetter )
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		return (static_cast<C&>(rObj).*m_pfSetter)( rProp.GetPropertyListConstRef() );
	}

protected:
	SetterFunc		m_pfSetter;
};

template< class ClassType, class VariableType, VistaProperty::ePropType nPropType = VistaProperty::PROPT_STRING >
class TVistaPublicVariablePropertyGet : public IVistaPropertyGetFunctor
{
public:
	typedef VariableType (ClassType::*MemberVariable);
	TVistaPublicVariablePropertyGet( const std::string& sPropName,
							const std::string& sClassType,
							MemberVariable pfMember,
							const std::string &sDescription = "" )
	: IVistaPropertyGetFunctor( sPropName, sClassType, sDescription )
	, m_pfMember( pfMember )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		std::string sValue;
		VistaConversion::ToString< VariableType >( static_cast< const ClassType& >( rObj ).*m_pfMember, sValue );
		rProp.SetValue( sValue );
		rProp.SetPropertyType( nPropType );
		return true;
	}

protected:
	MemberVariable m_pfMember;
};
template< class ClassType, class VariableType, class ConvertArg, VistaProperty::ePropType nPropType = VistaProperty::PROPT_STRING >
class TVistaPublicVariablePropertyConvertAndGet : public IVistaPropertyGetFunctor
{
public:
	typedef VariableType (ClassType::*MemberVariable);
	typedef std::string (*ConvertFunc)( ConvertArg );

	TVistaPublicVariablePropertyConvertAndGet( const std::string& sPropName,
							const std::string& sClassType,
							MemberVariable pfMember,
							ConvertFunc pfConvert,
							const std::string &sDescription = "" )
	: IVistaPropertyGetFunctor( sPropName, sClassType, sDescription )
	, m_pfMember( pfMember )
	, m_pfConvert( pfConvert )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		std::string sValue = (*m_pfConvert)( static_cast< const ClassType& >( rObj ).*m_pfMember );
		rProp.SetValue( sValue );
		rProp.SetPropertyType( nPropType );
		return true;
	}

protected:
	MemberVariable m_pfMember;
	ConvertFunc m_pfConvert;
};
template < class ClassType, class VariableType >
class TVistaPublicVariablePropertySet : public IVistaPropertySetFunctor
{
public:
	typedef VariableType (ClassType::*MemberVariable);

	TVistaPublicVariablePropertySet(const std::string& sPropName,
									const std::string& sClassType,
									MemberVariable pfMember,
									const std::string &sDescription = "")
	: IVistaPropertySetFunctor(sPropName, sClassType, sDescription)
	, m_pfMember( pfMember )
	{
	}

	virtual bool operator()( IVistaPropertyAwareable& rObj, const VistaProperty& rProp )
	{
		return VistaConversion::FromString< VariableType >( rProp.GetValue(), static_cast< ClassType& >( rObj ).*m_pfMember );
	}

protected:
	MemberVariable m_pfMember;
};
template < class ClassType, class VariableType, class ConvertArg = const std::string& >
class TVistaPublicVariablePropertyConvertAndSet : public IVistaPropertySetFunctor
{
public:
	typedef VariableType (ClassType::*MemberVariable);
	typedef VariableType (*ConvertFunc)( ConvertArg );

	TVistaPublicVariablePropertyConvertAndSet(const std::string& sPropName,
									const std::string& sClassType,
									MemberVariable pfMember,
									ConvertFunc pfConvert,
									const std::string &sDescription = "")
	: IVistaPropertySetFunctor(sPropName, sClassType, sDescription)
	, m_pfMember( pfMember )
	, m_pfConvert( pfConvert )
	{
	}

	virtual bool operator()( IVistaPropertyAwareable& rObj, const VistaProperty& rProp )
	{
		static_cast< ClassType& >( rObj ).*m_pfMember = (*m_pfConvert)( rProp.GetValue() );
		return true;
	}

protected:
	MemberVariable m_pfMember;
	ConvertFunc m_pfConvert;
};

template<class C> class TVistaPublicStringPropertyGet : public IVistaPropertyGetFunctor
{
public:
	typedef std::string (C::*StringMember);
	TVistaPublicStringPropertyGet(const std::string &sPropName,
							const std::string &sClassType,
							StringMember pfMember,
							const std::string &sDescription = "")
	: IVistaPropertyGetFunctor(sPropName, sClassType, sDescription)
	, m_pfMember( pfMember )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		rProp.SetValue( static_cast<const C&>(rObj).*m_pfMember );
		rProp.SetPropertyType( VistaProperty::PROPT_STRING );
		return !rProp.GetIsNilProperty();
	}

protected:
	StringMember m_pfMember;
};


template <class C> class TVistaPublicStringPropertySet : public IVistaPropertySetFunctor
{
public:
	typedef std::string (C::*StringMember);
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfMember	pointer to the reflectionable's set method to be used
	*/
	TVistaPublicStringPropertySet(const std::string &sPropName,
									const std::string &sClassType,
									StringMember pfMember,
									const std::string &sDescription = "")
	: IVistaPropertySetFunctor(sPropName, sClassType, sDescription)
	, m_pfMember( pfMember )
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		static_cast<C&>(rObj).*m_pfMember = rProp.GetValue();
		return true;
	}

protected:
	StringMember m_pfMember;
};


/**
 * C - base type
 * A - ref type
 * usage, e.g. : bool Get(float &, float &) const;
 * -> TVistaProperty2RefGet<float, TYPE>("name", "type", &type::Get)
 */
template<class A, class C, VistaProperty::ePropType nPropSubType = VistaProperty::PROPT_DOUBLE>
class TVistaProperty2RefGet : public IVistaPropertyGetFunctor
{
public:
	typedef bool (C::*GetterFunc)( A&, A& ) const;

	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfGetter	pointer to the reflectionable's get method to be used
	*/
	TVistaProperty2RefGet(const std::string &sPropName,
							const std::string &sClassType,
							GetterFunc pfGetter,
							const std::string &sDescription = "" )
	: IVistaPropertyGetFunctor(sPropName, sClassType, sDescription)
	, m_pfGetter( pfGetter )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		A aArray[2];
		if( (static_cast<const C&>(rObj).*m_pfGetter)( aArray[0], aArray[1] ) == false )
		{
			return false;
		}
	
		std::string sText =	VistaConversion::ArrayToString<2, A>( aArray );
		rProp.SetValue( sText );
		rProp.SetPropertyType( VistaProperty::PROPT_LIST );
		rProp.SetPropertyListSubType( nPropSubType );		
		return true;
	}

protected:
	GetterFunc	m_pfGetter;
};


template <class Arg, class C>
class TVistaProperty2ValSet : public IVistaPropertySetFunctor
{
public:
	typedef bool (C::*SetterFunc)( Arg, Arg );
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfSetter	pointer to the reflectionable's set method to be used
	*/
	TVistaProperty2ValSet( const std::string &sPropName,
						const std::string &sClassType,
						SetterFunc pfSetter,
						const std::string &sDescription = "" )
	: IVistaPropertySetFunctor( sPropName, sClassType, sDescription )
	, m_pfSetter( pfSetter )
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		Arg aData[2];
		if( VistaConversion::ArrayFromString<2, Arg>( rProp.GetValue(), aData ) )
		{
			return ( static_cast<C&>(rObj).*m_pfSetter)( aData[0], aData[1] );
		}
		return false;
	}

protected:
	SetterFunc	m_pfSetter;
};

/**
 * C - base type
 * A - ref type
 * usage, e.g. : bool Get(float &, float &) const;
 * -> TVistaProperty2RefGet<float, TYPE>("name", "type", &type::Get)
 */
template<class A, class C, VistaProperty::ePropType nPropSubType = VistaProperty::PROPT_DOUBLE>
class TVistaProperty3RefGet : public IVistaPropertyGetFunctor
{
public:
	typedef bool (C::*GetterFunc)( A&, A&, A& ) const;
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfGetter	pointer to the reflectionable's get method to be used
	*/
	TVistaProperty3RefGet(const std::string &sPropName,
							const std::string &sClassType,
							GetterFunc pfGetter,
							const std::string &sDescription = "" )
	: IVistaPropertyGetFunctor(sPropName, sClassType, sDescription)
	, m_pfGetter( pfGetter )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		A aArray[3];
		if( (static_cast<const C&>(rObj).*m_pfGetter)( 
						aArray[0], aArray[1], aArray[2] ) == false )
		{
			return false;
		}
	
		std::string sText =	VistaConversion::ArrayToString<3, A>( aArray );
		rProp.SetValue( sText );
		rProp.SetPropertyType( VistaProperty::PROPT_LIST );
		rProp.SetPropertyListSubType( nPropSubType );		
		return true;
	}

protected:
	GetterFunc	m_pfGetter;
};


template <class Arg, class C>
class TVistaProperty3ValSet : public IVistaPropertySetFunctor
{
public:
	typedef bool (C::*SetterFunc)( Arg, Arg, Arg );
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfSetter	pointer to the reflectionable's set method to be used
	*/
	TVistaProperty3ValSet( const std::string &sPropName,
						const std::string &sClassType,
						SetterFunc pfSetter,
						const std::string &sDescription = "" )
	: IVistaPropertySetFunctor(sPropName, sClassType, sDescription)
	, m_pfSetter( pfSetter )
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		Arg aData[3];
		if( VistaConversion::ArrayFromString<3, Arg>( rProp.GetValue(), aData ) )
		{
			return ( static_cast<C&>(rObj).*m_pfSetter)( 
									aData[0], aData[1], aData[2] );
		}
		return false;
	}

protected:
	SetterFunc	m_pfSetter;
};

/**
 * C - base type
 * A - ref type
 * usage, e.g. : bool Get(float &, float &) const;
 * -> TVistaProperty2RefGet<float, TYPE>("name", "type", &type::Get)
 */
template<class A, class C, VistaProperty::ePropType nPropSubType = VistaProperty::PROPT_DOUBLE>
class TVistaProperty4RefGet : public IVistaPropertyGetFunctor
{
public:
	typedef bool (C::*GetterFunc)( A&, A&, A&, A& ) const;
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfGetter	pointer to the reflectionable's get method to be used
	*/
	TVistaProperty4RefGet( const std::string &sPropName,
							const std::string &sClassType,
							GetterFunc pfGetter,
							const std::string &sDescription = "" )
	: IVistaPropertyGetFunctor(sPropName, sClassType, sDescription)
	, m_pfGetter( pfGetter )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		A aArray[4];
		if( (static_cast<const C&>(rObj).*m_pfGetter)( 
					aArray[0], aArray[1], aArray[2], aArray[3] ) == false )
		{
			return false;
		}
	
		std::string sText =	VistaConversion::ArrayToString<4, A>( aArray );
		rProp.SetValue( sText );
		rProp.SetPropertyType( VistaProperty::PROPT_LIST );
		rProp.SetPropertyListSubType( nPropSubType );		
		return true;
	}

protected:
	GetterFunc	m_pfGetter;
};


template <class Arg, class C>
class TVistaProperty4ValSet : public IVistaPropertySetFunctor
{
public:
	typedef bool (C::*SetterFunc)( Arg, Arg, Arg, Arg );
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfSetter	pointer to the reflectionable's set method to be used
	*/
	TVistaProperty4ValSet( const std::string &sPropName,
						const std::string &sClassType,
						SetterFunc pfSetter,
						const std::string &sDescription = "" )
	: IVistaPropertySetFunctor(sPropName, sClassType, sDescription)
	, m_pfSetter( pfSetter )
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		Arg aData[4];
		if( VistaConversion::ArrayFromString<4, Arg>( rProp.GetValue(), aData ) )
		{
			return ( static_cast<C&>(rObj).*m_pfSetter)( 
									aData[0], aData[1], aData[2], aData[3] );
		}
		return false;
	}

protected:
	SetterFunc	m_pfSetter;
};

/**
 * VistaGetArrayPropertyFunctor retrieves a named property which contains n floats
 * given in array format i.e. the getter has the signature
 * void GetFloats(float f[n]) const;
 * This can be used to retrieve e.g. a coordinate vector, a rotation, or a rgb tuple.
 *
 * CRefl		Reflectionable type
 * CArrayType	type of array elements
 * DIM			Number of floats to retrieve
 *
 */
template<class CRefl, class CArrayType, int DIM=3,
			VistaProperty::ePropType nSubType = VistaProperty::PROPT_DOUBLE>
class TVistaPropertyArrayGet : public IVistaPropertyGetFunctor
{
public:
	typedef void ( CRefl::*GetterFunc )( CArrayType* ) const;
	/**
	 * @param strPropertyName		name of the property to get
	 * @param strClassName			name of the reflectionable type
	 * @param pfGetter				pointer to the reflectionable's getter method
	 */
	TVistaPropertyArrayGet(	const std::string& strPropertyName,
									const std::string& strClassType,
									GetterFunc pfGetter,
									const std::string &sDescription = "" )
	: IVistaPropertyGetFunctor(strPropertyName, strClassType, sDescription)
	, m_pfGetter( pfGetter )
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable& rRefl, VistaProperty &rProp) const
	{
		CArrayType aData[DIM];
		(static_cast<const CRefl&>(rRefl).*m_pfGetter)(aData);
		std::string sValue = VistaConversion::ArrayToString<DIM, CArrayType>( aData );
		rProp.SetValue( sValue );
		rProp.SetPropertyType( VistaProperty::PROPT_LIST );
		rProp.SetPropertyListSubType( nSubType );
		return !rProp.GetIsNilProperty();
	}
protected:
	GetterFunc	m_pfGetter;
};


/**
 * VistaSetArrayPropertyFunctor writes a named property of n floats
 * given in array format i.e. the setter has the signature
 * void SetFloats(float f[n]);
 * This can be used to write e.g. a coordinate vector, a rotation, or a rgb tuple.
 *
 * CRefl		Reflectionable type
 * CArrayType	type of array elements
 * DIM			Number of floats to retrieve
 *
 */
template<class CRefl, class CArrayType, int DIM=3>
class TVistaPropertyArraySet : public IVistaPropertySetFunctor
{
public:
	typedef bool ( CRefl::*SetterFuncConst )( const CArrayType* );
	typedef bool ( CRefl::*SetterFunc )( CArrayType* );
	/**
	 * @param strPropertyName		name of the property to set
	 * @param strClassName			name of the reflectionable type
	 * @param setter				pointer to the reflectionable's setter method
	 */
	TVistaPropertyArraySet(	const std::string& strPropertyName,
									const std::string& strClassName,
									SetterFunc pfSetter,
									const std::string &sDescription = "" )
	: IVistaPropertySetFunctor(strPropertyName, strClassName, sDescription)
	, m_pfSetterFunc( pfSetter )
	, m_pfConstSetterFunc( NULL )
	{
	}

	/**
	 * @param strPropertyName		name of the property to set
	 * @param strClassName			name of the reflectionable type
	 * @param setter				pointer to the reflectionable's setter method
	 */
	TVistaPropertyArraySet(	const std::string& strPropertyName,
									const std::string& strClassName,
									SetterFuncConst pfSetter,
									const std::string &sDescription = "" )
	: IVistaPropertySetFunctor(strPropertyName, strClassName, sDescription)
	, m_pfSetterFunc( NULL )
	, m_pfConstSetterFunc( pfSetter )
	{
	}

	/**
	 * set property rProp to the given reflectionable rRefl
	 */
	virtual bool operator()(IVistaPropertyAwareable& rRefl, const VistaProperty &rProp)
	{
		CArrayType aData[DIM];
		if( VistaConversion::ArrayFromString<DIM, CArrayType>( rProp.GetValue(), aData  ) == false )
			return false;
		if( m_pfConstSetterFunc )
			return (static_cast<CRefl&>(rRefl).*m_pfConstSetterFunc)( aData );
		else
			return (static_cast<CRefl&>(rRefl).*m_pfSetterFunc)( aData );
	}
protected:
	SetterFuncConst		m_pfConstSetterFunc;
	SetterFunc			m_pfSetterFunc;
};


/**
 * Helper function to allow creating the most common PropertyG/Sets with (partial) automatic
 * template argument deduction, making them easier to write and read.
 * None or only the first template parameter has to be provided, the remaining ones are derived from
 * the parameters.
 */
namespace Vista
{
	// requires R to be explicitely specified
	template< class R, class C, class Arg >
	IVistaPropertySetFunctor* CreateVistaPropertySet( const std::string &sPropName,
											const std::string &sClassType,
											bool ( C::*pfSetter )( Arg ),
											const std::string &sDescription = "" )
	{
		return new TVistaPropertySet< Arg, R, C >( sPropName, sClassType, pfSetter, sDescription );
	}
	
	// requires no explicitely specified template args
	template< class C, class CRes, class Arg >
	IVistaPropertySetFunctor* CreateVistaPropertyConvertAndSet( const std::string &sPropName,
					const std::string &sClassType,
					bool ( C::*pfSetter )( Arg ),
					CRes ( *pfConvert )( const std::string& ),
					const std::string &sDescription = "" )
	{
		return new TVistaPropertyConvertAndSet< Arg, CRes, C >( sPropName,
									sClassType, pfSetter, pfConvert, sDescription );
	}

	// requires nPropType to be explicitely specified
	template< VistaProperty::ePropType nPropType, class C, class R >
	IVistaPropertyGetFunctor* CreateVistaPropertyGet( const std::string &sPropName,
											const std::string &sClassType,
											R ( C::*pfGetter )() const,
											const std::string &sDescription = "" )
	{
		return new TVistaPropertyGet< R, C, nPropType >( sPropName, sClassType, pfGetter, sDescription );
	}

	// requires nPropType to be explicitely specified
	template< VistaProperty::ePropType nPropType, class C, class R, class ConvArg >
	IVistaPropertyGetFunctor* CreateVistaPropertyConvertAndGet( const std::string &sPropName,
					const std::string &sClassType,
					R ( C::*pfGetter )() const,
					std::string ( *pfConvert )( ConvArg ),
					const std::string &sDescription = "" )
	{
		return new TVistaPropertyConvertAndGet< R, ConvArg, C, nPropType >( sPropName,
									sClassType, pfGetter, pfConvert, sDescription );
	}

}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAREFLECTIONABLE_H

