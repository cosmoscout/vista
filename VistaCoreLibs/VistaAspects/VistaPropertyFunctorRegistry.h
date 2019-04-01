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


#ifndef _VISTAPROPERTYFUNCTORREGISTRY_H
#define _VISTAPROPERTYFUNCTORREGISTRY_H

#include "VistaAspectsConfig.h"

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaReferenceCountable.h"

#include <VistaBase/VistaStreamUtils.h>

#include <map>
#include <list>
#include <set>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaPropertyGetFunctor;
class IVistaPropertySetFunctor;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * @brief singleton structure to holde property-functors, e.g. function object
 *        that access a property of a PropertyAwareable that can be queried
 *        by a symbol / name / string.
 * 
 * The functor registry is used to hold function pointer to property getter/setter
 * functions.
 *
 * A property has
 * <ul>
 *  <li>name</li>
 *  <li>exact class</li>
 * </ul>
 *
 * Different classes can have the same property names.
 * Example:
 * A:prop[R](f) means class A holds a read/get property named 'prop' and is accessible by
 * a functor called 'f'.
 *
 *  - so A:prop[R](f1) and B:prop[R](f2) is possible, properties 'prop' on class A will be resolved
 *    using f1, and properties 'prop' on B will be resolved by using functor f2.
 *  - when C is an A, and A:prop[R](f1) then C:prop[R](f1) is given
 *  - when C is an A, and A:prop[R](f1) and C:prop[R](f2) then f2 will be used to resolve 'prop' on a C
 *
 * @warning access to instances of this class is not thread safe
 */
class VISTAASPECTSAPI VistaPropertyFunctorRegistry : public IVistaReferenceCountable
{
	public:
		/**
		 * @brief returns a pointer to the registry singleton.
		 * 
		 * The first call to this function will create the functor registry.
		 * The functor registry is unloaded when the lib is unloaded.
		 * By then, all still registered functor objects will be destroyed
		 * using the local delete.
		 */
	static VistaPropertyFunctorRegistry *GetSingleton();

	/** 
	 * @brief register a getter functor for a property and class.
	 *
	 * There is no constraint on the symbols of the property name.
	 * Different class types can have properties with the same name,
	 * for that each property is identified by the (name, class) tuple.
	 *
	 * @param sPropertyName the name of the property. 
     * @param sClassType the string that describes the class to which this
	          property belongs.
     * @param pFunctor the functor to call, must not be NULL.
	 * @param bForce force a re-registration in case (name,class) already exists
	 * @return false if (name,class) is already registered and bForce was given as false,
	 *         else true
	 */
	bool RegisterGetter(const std::string &sPropertyName,
						const std::string &sClassType,
						IVistaPropertyGetFunctor *pFunctor,
						bool bForce = false);

	/**
	 * @brief unregister getter functors for a property and class.
	 *
	 * Note that the associated functor is <b>not</b> deleted by unregistering it with this method.
	 * @see GetGetFunctor() to get hold of the pointer before unregistering.
	 *
	 * @param sPropName the name of the property to unregister
	 * @param sClassType the class type for the property to unregister
	 *
	 * @return true for:
	           - properties that were not registered before
			   - properties that were not assigned with class sClassType
			   - properties that were unregistered, so GetGetFunctor(sPropName,sClassType,baseClasses) will return NULL

			   false else.
     *
	 */
	bool UnregisterGetter(const std::string &sPropName,
						  const std::string &sClassType);

	/**
	 * @brief returns a get-functor for property (sPropName,sClassType).
	 *
	 * Getter functors are inherited by class, so the caller has to pass the list
	 * of base-class strings to search for.
	 *
	 * This function uses a list of base-class names to do a least-known search for
	 * the property. 
	 * 
	 * @param sPropName the name of the property to search
	 * @param sClassType the class of the property to look for (exact match)
	 * @param rLibaseCl the class list to search 
	 * @param bUseCache tells to use/populate the cache by this lookup
	 */
	IVistaPropertyGetFunctor *GetGetFunctor(const std::string &sPropName,
										const std::string &sClassType,
										const std::list<std::string> &rLiBaseCl,
										bool bUseCache = true ) const;


	/**
	 * @brief registers a setter functor for property (sPropertyName, sClassType)
	 * 
	 * @param sPropertyName the name of the property. 
     * @param sClassType the string that describes the class to which this
	          property belongs.
     * @param pFunctor the functor to call, must not be NULL.
	 * @param bForce force a re-registration in case (name,class) already exists
	 * @return false if (name,class) is already registered and bForce was given as false,
	 *         else true
	 */
	bool RegisterSetter(const std::string &sPropertyName,
						const std::string &sClassType,
						IVistaPropertySetFunctor *pFunctor,
						bool bForce = false);

	/**
	 * @brief unregister getter functors for a property and class.
	 *
	 * Note that functors associated with the property are not deleted during unregistration.
	 * @see GetSetFunctor() to claim the pointer for a delete if you do not hold it elsewhere before unregistering.
	 *
	 * @param sPropName the name of the property to unregister
	 * @param sClassType the class type for the property to unregister
	 *
	 * @return true for:
	           - properties that were not registered before
			   - properties that were not assigned with class sClassType
			   - properties that were unregistered, so GetGetFunctor(sPropName,sClassType,baseClasses) will return NULL

			   false else.
     *
	 */
	bool UnregisterSetter(const std::string &sPropName,
						  const std::string &sClassType);

	/**
	 * @brief returns a set-functor for property (sPropName,sClassType).
	 *
	 * Setter functors are inherited by class, so the caller has to pass the list
	 * of base-class strings to search for.
	 *
	 * This function uses a list of base-class names to do a least-known search for
	 * the property. 
	 * 
	 * @param sPropName the name of the property to search
	 * @param sClassType the class of the property to look for (exact match)
	 * @param rLibaseCl the class list to search 
	 */
	IVistaPropertySetFunctor *GetSetFunctor(const std::string &sPropName,
										const std::string &sClassType,
										const std::list<std::string> &rLiBaseCl,
										bool bUseCache = true ) const;

	/**
	 * @brief returns the list of symbols for a given base class list that can be used for access.
	 * 
	 * @param setStore out parameter to hold all strings that can be used by instances of
	 *                 any object in one of the rLiBaseCl classes for getting/reading a property.
	 * @param rLiBaseCl the top-to-bottom ordered list of strings that hold the class names.
	 * @return setStore.size()
	 */
	int GetGetterSymbolSet(std::set<std::string> &setStore,
						   const std::list<std::string> &rLiBaseCl) const;

	/**
	 * @brief returns the list of symbols for a given base class list that can be used for access.
	 * 
	 * @param setStore out parameter to hold all strings that can be used by instances of
	 *                 any object in one of the rLiBaseCl classes for setting/writing a property.
	 * @param rLiBaseCl the top-to-bottom ordered list of strings that hold the class names.
	 * @return setStore.size()
	 */
	int GetSetterSymbolSet(std::set<std::string> &setStore,
						   const std::list<std::string> &rLiBaseCl) const;

	/**
	 * @brief gets all symbols known to instances of classes given in rLiBaseCl
	 * 
	 * @param setStore out parameter set to the symbol names to
	 * @param rLiBase the list of base class names to look for
	 * @return setStore.size()
	 */
	int GetSymbolSet(std::set<std::string> &setStore,
					 const std::list<std::string> &rLiBaseCl) const;


	/**
	 * @brief gets all getters (by functor) that <emph>exactly</emph> match class name strExactMatch,
	          i.e. inheritance is not respected
     * @param strExactMatch class name to look for (do not respect base classes)
	 * @return the functor pointer list
	 */
	std::list<IVistaPropertyGetFunctor*> GetGetterByClass(const std::string &strExactMatch) const;

	/**
	 * @brief gets all setters (by functor) that <emph>exactly</emph> match class name strExactMatch,
	          i.e. inheritance is not respected
     * @param strExactMatch class name to look for (do not respect base classes)
	 * @return the functor pointer list
	 */
	std::list<IVistaPropertySetFunctor*> GetSetterByClass(const std::string &strExactMatch) const;

	/**
	 * @brief reflects talkative state of this registry.
	 * debugging api, registration / unregistration / lookup failure will be reported on vstr::warnp()
	 * when registry is set to be talkative.
	 */
	bool GetTalkativeFlag() const;

	/**
	 * @brief sets the talkative state of this registry.
	 * 
	 * @param bTalkative new state (true=talkative / false=silent)
	 * debugging api, registration / unregistration / lookup failure will be reported on vstr::warnp()
	 * when registry is set to be talkative.
	 */
	void SetTalkativeFlag(bool bTalkative);

	/**
	 * @brief debugging api, dumps registry to oStream
	 * 
	 * @param oStream output stream to use for dumping.
	 */
	void Print( std::ostream& oStream = vstr::out() ) const;

	protected:
	private:
		VistaPropertyFunctorRegistry();
		~VistaPropertyFunctorRegistry();


	static VistaPropertyFunctorRegistry *m_pSingleton;

	typedef std::pair<std::string, std::string> STRPAIR;
	typedef std::multimap<std::string, std::set<std::string> > STSTMAP;

	typedef std::map<STRPAIR, IVistaPropertyGetFunctor*> GETFMAP;
	typedef std::map<STRPAIR, IVistaPropertySetFunctor*> SETFMAP;

	// used to model property inheritance: prop-name to class type map
	STSTMAP m_mpGProps; /**< the gprop-name to class map */
	STSTMAP m_mpSProps; /**< the gprop-name to class map */

	        GETFMAP m_mpGetters;
	mutable GETFMAP m_mpGetterCache;
	// used to hold setters, and a fast-lookup structure for setters (subsequent access)
	        SETFMAP m_mpSetters;
	mutable SETFMAP m_mpSetterCache;

	bool m_bTalkative;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPROPERTYFUNCTORREGISTRY_H

