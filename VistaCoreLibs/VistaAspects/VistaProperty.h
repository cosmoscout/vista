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


#ifndef _VISTAPROPERTY_H
#define _VISTAPROPERTY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaAspectsConfig.h"

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaNameable.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaPropertyList;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A property is a mapping from std::string -> std::string|PropertyList. As such, it can
 * describe a value as a (human readable) std::string or a mapping, resembling
 * a tree like structure with associative lookup. The VistaProperty() interface
 * is meant for slow interaction (e.g. humans) or comfort. It is not optimized for
 * speed or precision. As such it works with conversion, copying and unstrict type
 * rules. This makes it comfortable, but maybe error prone or hard to parse.<br>
 *
 * Use as follows.
 * <verbatim>
 *  VistaProperty p;
 *  p.SetNameForNameable("testname");
 *  p.SetValue("testvalue");
 *
 *  VistaProperty o("testdouble");
 *  o.SetValue( VistaConversion::ToString( 3.14 ) );
 *  o.SetPropertyType(VistaProperty::PROPT_DOUBLE);
 * </verbatim>
 * <br>
 * Instead of working with properties in such a low level way, use PropertyLists or
 * the IVistaPropertyAwareable() interface, if you have a property awareable at hand.
 * @see PropertyList()
 */
class VISTAASPECTSAPI VistaProperty : public IVistaNameable
{
public:

	enum ePropType
	{
		PROPT_NIL = 0,
		PROPT_STRING,   /**< default */
		PROPT_INT,      /**< captures int as a std::string */
		PROPT_BOOL,     /**< captures true/false/on/off */
		PROPT_DOUBLE,   /**< captures a real as a std::string */
		PROPT_LIST,     /**< is a list of things, possibly all of the same type */
		PROPT_ID,       /**< is a std::string'ed adress pointer */
		PROPT_PROPERTYLIST  /**< is a tree in itself */
	};

	/**
	 * Creates an unamed, no-value property
	 * @see SetNameForNameable()
	 * @see SetValue()
	 */
	VistaProperty();

	/**
	 * Creates a no-value property with name sName
	 * @see SetNameForNameable()
	 * @see SetValue()
	 * @param sName the name for this property
	 */
	VistaProperty( const std::string& sName );

	VistaProperty( const std::string& sName,
				   const std::string& sValue,
				   ePropType eType = PROPT_STRING,
				   ePropType eListType = PROPT_NIL );

	/**
	 * Copies name and value. In case oOther is a PropertyList
	 * property, the sublist is <emph>copied</emph>. This can
	 * be costly.
	 */
	VistaProperty(const VistaProperty &oOther);

	/**
	 * Destructor. Deletes any subproperty PropertyList.
	 * As we are working with copies here, this should
	 * not be a problem.
	 */
	virtual ~VistaProperty();


	/**
	 * @return the value of this property in human-readable format.
	 */
	std::string GetValue() const;
	const std::string& GetValueConstRef() const;

	/**
	 * Returns the value of this property in <emph>copy</emph>.
	 * Use like PropertyList p(oProperty.GetPropertyListValue()); during
	 * construction.
	 * @return a copy of the internal PropertyList value.
	 */
	VistaPropertyList  GetPropertyListValue() const;

	/**
	 * Returns a <emph>reference</emph> to the internal PropertyList value.
	 * This can be used to change it via PropertyList calls. If no PropertyList
	 * is currently present, one will be created on the fly.
	 * Note, too, that we provide this a const and non-const.
	 * @return a reference to the internal PropertyList to be changed
	 * @see GetPropertyListConstRef()
	 */
	VistaPropertyList &GetPropertyListRef();

	/**
	 *	Note that this
	 * method can throw an exception, in case the property is not
	 * of type PROPT_PROPERTYLIST or the internal PropertyList pointer is not
	 * created for whatever reason. Although we could create on on the
	 * fly, the exception reveals an error in the access logic a bit better.
	 * @return a const reference to the internal PropertyList to be changed
	 * @see GetPropertyListRef()
	 */
	const VistaPropertyList &GetPropertyListConstRef() const;


	/**
	 * Sets the value as a std::string type. "1.23" will be of type PROPT_STRING.
	 * There is no intention to guess the type of a value, the user has to
	 * switch to the correct type using SetPropertyType(). The only effect
	 * on the type is that a PROPT_NIL property switches to PROPT_STRING.
	 * @see SetPropertyType()
	 */
	void   SetValue(const std::string &sValue);

	/**
	 * Sets a PropertyList value. The oOther will be <emph>copied</emph> to the
	 * internal storage of this property. The type of this property will be
	 * set to PROPT_PROPERTYLIST. GetValue() still returns the empty std::string.
	 *
	 * @param oOther the PropertyList to be copied and serve as a value.
	 */
	void   SetPropertyListValue(const VistaPropertyList &oOther);


	/**
	 * Returns human-readable strings for the ePropType enumeration.
	 * This can be useful for debugging purposes.
	 * @see ePropType()
	 * @see GetPropertyType()
	 * @return a human readable std::string for the ePropType enumeration
	 */
	static std::string GetPropTypeName(VistaProperty::ePropType eTypeId);

	/**
	 * Returns an enumeration value for a given int. This is needed
	 * during the serialization process and does something like a
	 * safe-case.
	 * @return a proper ePropType for iPropType or PROPT_NIL if iPropType is out of range.
	 */
	static VistaProperty::ePropType GetPropTypeEnum( int iPropType );


	/**
	 * @return the type of this property.
	 * @see ePropType()
	 */
	ePropType  GetPropertyType() const;

	ePropType  GetPropertyListSubType() const;

	/**
	 * Sets the property type for this property. In case one switches
	 * from or to PROPT_PROPERTYLIST, the internal PropertyList will be deleted
	 * or created.
	 * @param eType the new type.
	 */
	void SetPropertyType(ePropType eType);

	void SetPropertyListSubType(ePropType eType);

	/**
	 * Checks, whether this property has a type or not.
	 * Fast way of writing if(GetPropertyType() == VistaProperty::PROPT_NIL).
	 * @return true iff GetPropertyType() == VistaProperty::PROPT_NIL
	 */
	bool GetIsNilProperty() const;

	// ######################################################
	// OPERATORS
	// ######################################################
	bool operator==(const std::string &) const;
	bool operator==(const VistaPropertyList &) const;
	bool operator==(const VistaProperty &) const;

	VistaProperty &operator=(const VistaProperty &);


	// ########################################################################
	// IVistaNameable-Interface
	// ########################################################################
	virtual std::string GetNameForNameable() const;
	virtual void   SetNameForNameable(const std::string &sNewName);

protected:
private:
	std::string			m_sName;
	std::string			m_sValue;
	ePropType			m_ePropType;
	ePropType			m_eListSubType;
	VistaPropertyList*	m_pSubProps;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPROPERTYAWARE_H

