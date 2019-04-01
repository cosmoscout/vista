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


#ifndef _VISTASERIALIZABLE_H
#define _VISTASERIALIZABLE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaAspectsConfig.h"

#include <string>

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
 * Basic interface for the active object serialization of classes.
 * This usually means to redefine all the methods that are in this
 * interface
 */
class VISTAASPECTSAPI IVistaSerializable
{
public:
	virtual ~IVistaSerializable();

	/**
	 * Think of this as "SAVE"
	 */
	virtual int Serialize(IVistaSerializer &) const = 0;

	/**
	 * Think of this as "LOAD"
	 */
	virtual int DeSerialize(IVistaDeSerializer &) = 0;


	/**
	 * should return a very unique identifier that can be
	 * used, e.g., to prepend a stream and let receivers decide
	 * about what is to come. We usually take the class name
	 * AS-IS, as this is usually, at least for a coherent package,
	 * unique (you are free to use namespaces in this std::string, as
	 * well.
	 * @return the unique identifier, can be per instance, or per class
	 */
	virtual std::string GetSignature() const = 0;

protected:
	IVistaSerializable();
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASERIALIZABLE

