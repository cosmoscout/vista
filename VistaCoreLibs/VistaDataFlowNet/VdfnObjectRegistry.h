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


#ifndef _VDFNOBJECTREGISTRY_H
#define _VDFNOBJECTREGISTRY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include <map>
#include <string>

#include <VistaAspects/VistaObjectRegistry.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaTransformable;
class IVistaNameable;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * the object registry is a core part of the DataFlowNet reflective interface.
 * There can be more than one, but this is not advised. The object registry
 * offers to map two types of objects
 * - IVistaTransformable instances, that can be retrieved using the
     GetObjectTransform() API
 * - IVistaNameable instances, that can be retrieved using the
     GetObject() API
 *
 * Note that the VdfnObjectRegistry is merely a hack, the selection of the
 * two base classes merely resembles the desire to get a hand at objects for
 * the purpose of transformation. At some time, you need their name, at another
 * time you need their transform properties. Which is used when can only be
 * determined by inspecting the code. As a consequence, any object that is
 * used for a specific purpose has to be registered separately. For example,
 * an object that is a transformable and a nameable has to be registered twice
 * in order to be found (using set object pointing both to the same object).
 *
 * Feel free to come up with a better concept.
 * All pointers stored in the registry must outlive the registry or be
 * unregistered before deleting them using RemObject().
 */
class VISTADFNAPI VdfnObjectRegistry : public VistaObjectRegistry
{
public:
	VdfnObjectRegistry();

	/**
	 * does <i>not</i> release memory on any instance that was registered.
	 */
	virtual ~VdfnObjectRegistry();

	/**
	 * @return the transformable pointer to an object named strName or NULL
	 */
	IVistaTransformable *GetObjectTransform( const std::string &strName ) const;

	/**
	 * @return the nameable pointer to an object named strName or NULL
	 */
	IVistaNameable      *GetObject( const std::string &strName ) const;

	template<class T>
	T *GetObject( const std::string &strName ) const
	{
		return dynamic_cast<T*>(GetObject(strName));
	}

	/**
	 * claim possibly both: the nameable interface and the transformable interface,
	 * if it was registered.
	 * @param strName the name to look up
	 * @param pObj a reference to a pointer to store the nameable interface to
	 * @param pTrans a  reference to a pointer to store the transformable interface to
	 * @return true if any of the two was found using strName
	 */
	bool GetObject( const std::string &strName,
					IVistaNameable *&pObj,
					IVistaTransformable *&pTrans ) const;

	/**
	 * sets the nameable and transformable interface to match the name strName.
	 * The method checks whether strName was already registered beforehand.
	 * If nameable has an empty name, its name will be set to strName
	 * @return false when strName already points to an object, true else
	 * @param nameable
	 * @param strName the name to register in this registry
	 */
	bool SetObject( const std::string &strName,
					IVistaNameable *nameable,
					IVistaTransformable *transformable );

	/**
	 * clear the bindings for strName
	 * @return false if no binding existed the could be cleared.
	 */
	bool RemObject( const std::string &strName );

	bool HasObject( const std::string &strName ) const;


protected:
private:
	VdfnObjectRegistry( const VdfnObjectRegistry & ) {}
	VdfnObjectRegistry &operator()( const VdfnObjectRegistry & ); // unimplemented
	VdfnObjectRegistry &operator=( const VdfnObjectRegistry & ); // unimplemented

	typedef std::map<std::string, IVistaTransformable*> OBJMAP;

	OBJMAP m_mpObjects;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNOBJECTREGISTRY_H

