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


/*============================================================================*/
/*                                                                            */
/* This Demo will create a box and a sphere (textured), both located in       */
/* another sphere.                                                            */
/* Two ways of creating a sphere are demonstrated:                            */
/* oGeometryFactory.CreateSphere() and oGeometryFactory.CreateFromPropertyList*/
/*                                                                            */
/*============================================================================*/

#ifndef _TEXTDEMOAPPL_H
#define _TEXTDEMOAPPL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaSimpleTextOverlay;
class IVistaTextEntity;
class Vista3DTextOverlay;
class VistaSystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class TextDemoAppl
{
public:
	TextDemoAppl();
	virtual ~TextDemoAppl();

	bool Init( int argc, char* argv[] );
	void Run();

private:
	VistaSystem*	m_pVistaSystem = nullptr;

	VistaSimpleTextOverlay		*m_pSimpleTextOverlay = nullptr;
	IVistaTextEntity 			*m_pTextEntity1 = nullptr;
	IVistaTextEntity			*m_pTextEntity2 = nullptr;
	IVistaTextEntity			*m_pTextEntity3 = nullptr;
	Vista3DTextOverlay			*m_pOverlay3D = nullptr;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _GEOMETRYDEMOAPPL_H

