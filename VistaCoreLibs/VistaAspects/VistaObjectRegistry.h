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


#ifndef _VISTAOBJECTREGISTRY_H
#define _VISTAOBJECTREGISTRY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <string>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaAspectsConfig.h"


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNameable;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaObjectRegistry can be used to uniquely save and retrieve objects
 * of base type IVistaNameable. The main purpose of this registry mechanism is
 * to uniquely identify objects in a clustered environment. For this to work,
 * objects have to be registered in exactly the same order on all participating
 * machines!
 */
class VISTAASPECTSAPI VistaObjectRegistry
{
public:
	VistaObjectRegistry();
	virtual ~VistaObjectRegistry();
	/**
	* save the given nameable into the registry
	*
	* @param    IVistaNameable*     nameable object to save
	* @return   int                 identifying key generated upon registration
	*/
	unsigned int RegisterNameable(IVistaNameable*);
	/**
	* unregister specified nameable
	*
	* @param  int	            registry key of nameable object to be unregistered
	* @return IVistaNameable*   pointer to unregistered nameable (NULL iff not found)
	*/
	IVistaNameable *UnregisterNameable(int );
	/**
	* unregister specified nameable
	*
	* @param  std::string            name of nameable object that should be unregistered
	* @return IVistaNameable*   pointer to unregistered nameable (NULL iff not found)
	*/
	IVistaNameable *UnregisterNameable(const std::string&);
	/**
	* unregister specified nameable
	*
	* @param            pointer to nameable object that should be unregistered
	* @return bool      true iff found and unregistered
	*/
	bool UnregisterNameable(IVistaNameable*);
	/**
	* retrieve nameable object given the registration key
	*
	* @param  int	            registry key of nameable object to be retrieved
	* @return IVistaNameable*   pointer to seeked nameable object (NULL iff not found)
	*/
	IVistaNameable *RetrieveNameable(int ) const ;
	/**
	* retrieve nameable object given object's name
	* NOTE: Implemented as a linear search on the registry map, thus this might become
	* expensive.
	*
	* @param  std::string            name of nameable object that should be retrieved
	* @return IVistaNameable*   pointer to seeked nameable object (NULL iff not found)
	*/
	IVistaNameable *RetrieveNameable(const std::string&) const;

	bool HasNameable( const std::string &strName ) const;


	/**
	* Clear the entire registry.
	* All registry information will be LOST!
	* No deletion of registerered instances will occur.
	*/
	void ClearRegistry();

	// ######################################################################
	// ITERATOR API
	// @group iterator api
	// @{
	typedef std::vector<IVistaNameable*>::const_iterator const_iterator;
	typedef std::vector<IVistaNameable*>::iterator iterator;
	typedef std::vector<IVistaNameable*>::reverse_iterator reverse_iterator;
	typedef std::vector<IVistaNameable*>::value_type value_type;
	typedef std::vector<IVistaNameable*>::size_type size_type;

	const_iterator begin() const;
	const_iterator end() const;

	iterator begin();
	iterator end();

	reverse_iterator rbegin();
	reverse_iterator rend();

	size_type count( const std::string &sName ) const;
	// @}
protected:
	int FindNameable(const std::string& strName) const;
private:
	VistaObjectRegistry( const VistaObjectRegistry & ) {}
	VistaObjectRegistry &operator()( const VistaObjectRegistry & ) { return *this; }
	VistaObjectRegistry &operator=( const VistaObjectRegistry & ) { return *this; }

	/**
	* map the registry keys to nameable objects.
	*/
	std::vector<IVistaNameable*> m_vecObjectRegistry;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOBJECTREGISTRY_H

