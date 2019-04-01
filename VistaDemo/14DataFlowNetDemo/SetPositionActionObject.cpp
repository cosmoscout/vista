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

#include "SetPositionActionObject.h" 

#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/**
 * Don't forget this line! It defines the inheritance relationship for
 * the reflectionable interface. Additionally, it provides the SsReflectionableName
 */
REFL_IMPLEMENT_FULL( SetPositionActionObject, IVdfnActionObject );


/**
 * Now, we define the functors to get and set the colors.
 * Those functors tell the reflectionable interface what getter/setter exist,
 * what data type they work on, and how they are named.
 * Since this is an ActionObject for the DFN, we utilize the special templated
 * TAction[G/S]etFunctor classes to create these functors.
 * When creating them, they are automatically registered with the FunctorRegistry,
 * thus you don't have to care about their deletion!
 * These functors are used by the DFN to determine which in- and outports the
 * ActionNodes for an ActionObject shall have.
 * While we do not require the GetFunctor for GetPosition in our application,
 * its still here for demo reasons.
 */
namespace
{
	IVistaPropertySetFunctor *SaSetter[] =
	{
		new TActionSetFunctor<
				SetPositionActionObject,		// class of the ActionObject
				VistaVector3D,				// data type of the setter
				const VistaVector3D&>(		// actual parameter for the setter
						&SetPositionActionObject::SetPosition, // pointer to the function
						NULL,				// Converter from string - we don't need this
						"position",			// name of the functor - this will also
											// be the name of the inport in the dfn net
						SsReflectionName,	// Name of the class, defined by REFL_IMPLEMENT_FULL
						"set position" ),	// help text
		NULL
	};

	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new TActionGetFunctor<
				SetPositionActionObject,		// class of the ActionObject
				VistaVector3D>(				// return type of the function
						&SetPositionActionObject::GetPosition, // pointer to the function
						NULL,				// Converter from string - we don't need this
						"position",			// name of the functor - this will also
											// be the name of the inport in the dfn net
						SsReflectionName,	// Name of the class, defined by REFL_IMPLEMENT_FULL
						"return position" ),// help text
		NULL
	};
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
SetPositionActionObject::SetPositionActionObject( VistaTransformNode* pTransformNode )
: IVdfnActionObject()
, m_pTransformNode( pTransformNode )
{
}

SetPositionActionObject::~SetPositionActionObject()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool SetPositionActionObject::SetPosition( const VistaVector3D& v3Position )
{
	m_pTransformNode->SetTranslation( v3Position );
	return true;
}

VistaVector3D SetPositionActionObject::GetPosition() const
{
	VistaVector3D v3Position;
	m_pTransformNode->GetTranslation( v3Position );
	return v3Position;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "SetPositionActionObject.cpp"                                  */
/*============================================================================*/

