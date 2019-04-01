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


#ifndef _VISTAPROPERTYAWAREABLE_H
#define _VISTAPROPERTYAWAREABLE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaAspectsConfig.h"

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaProperty.h>
#include <VistaAspects/VistaPropertyList.h>


#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/**
 * A template function that can be used to reassign a value only
 * when the value has changed. This can be useful in conjunction with
 * observer/oberveable interfaces when an update should only be propagated
 * once a value has changed.
 * Use like
 * <verbatim>
 *  if(compAndAssignFunc<double>(dold, dNew))
 *   update();
 * </verbatim>
 */
template<class T> int compAndAssignFunc(const T& x, T &y)
{
	if( x != y )
	{
		y = x;
		return 1;
	}
	return 0;
}


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAASPECTSAPI IVistaPropertyAwareable
{
public:
	/**
	* EPropertyStatus encodes the error values of
	* some of the int-returning methods below.
	*
	* PROP_OK				well, everything worked out fine
	* PROP_NOT_FOUND		unable to find the given property
	* PROP_INVALID_VALUE	unable to cast the given prop value to the type needed
	*/
	enum EPropertyStatus
	{
		PROP_NO_CHANGE=0,
		PROP_OK=1,
		PROP_NOT_FOUND,
		PROP_INVALID_VALUE
	};

	virtual ~IVistaPropertyAwareable() {};
	/**
	* Set the property given by refProp.GetNameForNameable() to
	* the value encoded by refProp.GetValue()
	* @param	refProp		Property with key and value to be set
	* @return	int			A status int from EPropertyStatus (NOTE: 0 == success)
	*/
	virtual int SetProperty(const VistaProperty & refProp)    = 0;
	/**
	* Get the property value for the key refProp.GetNameForNameable() and
	* put it to refProp's value field.
	* @param	refProp		Property with key of property to be retrieved
	* @param	refProp		Value field set accordingly iff successful
	* @return	int			A status int from EPropertyStatus (NOTE: 0 == success)
	*/
	virtual int GetProperty(VistaProperty &refProp)          = 0;
	virtual VistaProperty GetPropertyByName(const std::string &sPropName) = 0;
	/**
	* Set a property's value indexing it by it's name.
	* @param sPropName	property's key
	* @param sPropValue	property's value to be set
	* @return bool		true iff successful
	*
	*/
	virtual bool SetPropertyByName(const std::string &sPropName,
								   const std::string &sPropValue) = 0;
	/**
	* Set several properties at once.
	* @param	rPList	PropertyList containing all the properties to be set
	* @return	int		number of properties successfully set
	*/
	virtual int SetPropertiesByList(const VistaPropertyList &rPList) = 0;
	/**
	* Retrieve all object properties at once.
	* @param	rPList	PropertyList containing object's current properties
	* @return	int		number of properties retrieved
	*/
	virtual int GetPropertiesByList(VistaPropertyList &rPList)       = 0;
	/**
	* Retrieve the list of symbols i.e. property keys which are "unterstood" by
	* this object. This should contain all valid key which can be inquired using e.g.
	* the GetProperty(...) method.
	* @param	rStorageList	list with all valid property keys for this object
	* @return	int				number of valid property keys (i.e. rStorageList.size())
	*/
	virtual int GetPropertySymbolList(std::list<std::string> &rStorageList) = 0;


	virtual std::string GetPropertyDescription(const std::string &sPropName) = 0;

protected:
	IVistaPropertyAwareable() {};
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPROPERTYAWARE_H

