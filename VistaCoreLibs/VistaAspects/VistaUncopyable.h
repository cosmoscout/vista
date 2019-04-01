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


#ifndef _VISTAUNCOPYABLE_H
#define _VISTAUNCOPYABLE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/**
 * Makes a class uncopy-able by making the copy-ctor and assignment-operator private
 * Derived classes will also be un-copyable, however, depending on the compiler, the
 * error message when trying to copy derived classes may be unspecific, so it is
 * advised to also declare inherited classes as uncopyable.
 * Note that the access level changes to private, so take care to re-set it
 */
#define VISTA_UNCOPYABLE( ClassName ) \
private: \
	ClassName( const ClassName& oOther ); \
	ClassName& operator=( const ClassName& oCopy );

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAUNCOPYABLE_H

