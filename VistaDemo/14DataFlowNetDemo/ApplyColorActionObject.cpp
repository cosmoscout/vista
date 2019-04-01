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

#include "ApplyColorActionObject.h" 

#include <VistaKernel/GraphicsManager/VistaGeometry.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/**
 * Don't forget this line! It defines the inheritance relationship for
 * the reflectionable interface. Additionally, it provides the SsReflectionableName
 */
REFL_IMPLEMENT_FULL( ApplyColorActionObject, IVdfnActionObject );


/**
 * As for the SetPositionActionObject, we define a SetFunctor, but now,
 * we also create a GetFunctor for the GetColor method.
 */
namespace
{
	IVistaPropertySetFunctor *SaSetter[] =
	{
		new TActionSetFunctor<
				ApplyColorActionObject,		// class of the ActionObject
				VistaVector3D,				// data type of the setter
				const VistaVector3D&>(		// actual parameter for the setter
						&ApplyColorActionObject::SetColor, // pointer to the function
						NULL,				// Converter from string - we don't need this
						"color",			// name of the functor - this will also
											// be the name of the inport in the dfn net
						SsReflectionName,	// Name of the class, defined by REFL_IMPLEMENT_FULL
						"sets color" ),		// help text
		NULL
	};

	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new TActionGetFunctor<
				ApplyColorActionObject,		// class of the ActionObject
				VistaVector3D>(				// return type of the function
						&ApplyColorActionObject::GetColor, // pointer to the function
						NULL,				// Converter from string - we don't need this
						"color",			// name of the functor - this will also
											// be the name of the inport in the dfn net
						SsReflectionName,	// Name of the class, defined by REFL_IMPLEMENT_FULL
						"returns color" ),	// help text
		NULL
	};
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
ApplyColorActionObject::ApplyColorActionObject( VistaGeometry* pTargetGeometry )
: IVdfnActionObject()
, m_pGeometry( pTargetGeometry )
{
}

ApplyColorActionObject::~ApplyColorActionObject()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool ApplyColorActionObject::SetColor( const VistaVector3D& v3Color )
{
	if( v3Color == m_v3Color )
		return true;

	m_v3Color = v3Color;
	m_pGeometry->SetColor( VistaColor( v3Color[0], v3Color[1], v3Color[2] ) );
	/**
	 * we have changed m_v3Color, and thus the return value of GetColor.
	 * Thus, we have to inform any actionnode for this actionobject
	 * That they should update their outports accordingly.
	 * Therefore, we simply call a Notify()
	 */
	Notify();

	return true;
}

VistaVector3D ApplyColorActionObject::GetColor() const
{
	return m_v3Color;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "ApplyColorActionObject.cpp"                                   */
/*============================================================================*/

