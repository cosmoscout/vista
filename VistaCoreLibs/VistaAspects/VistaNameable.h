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


#ifndef _VISTANAMEABLE_H
#define _VISTANAMEABLE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaAspectsConfig.h"

//#include <VistaBase/VistaBaseTypes.h>

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
 * The IVistaNameable-Interface.
 */
class VISTAASPECTSAPI IVistaNameable
{
public:
	virtual ~IVistaNameable();

	/**
	 * @todo set method names to "Name()", so it reads like nmbl.Name()
	 */ 
	virtual std::string GetNameForNameable() const = 0;
	virtual void SetNameForNameable(const std::string &sNewName) = 0;


	/**
	 * @todo rename to GetId(), return VistaType::uint64 or native (size_t)
	 * or remove from this interface...
	 */
	void* GetNameableId() const;
protected:
	IVistaNameable();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANAMEABLE_H


