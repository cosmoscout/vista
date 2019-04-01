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

#ifndef __BOB_H
#define __BOB_H

/*============================================================================*/
/* INCLUDES & DEFINES                                                         */
/*============================================================================*/
#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>


#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>



#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaBase/VistaVectorMath.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>

/*============================================================================*/
/* FORWARD DECLARATION                                                        */
/*============================================================================*/
class VistaSystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class Bob
{
public:

	//@TODO. CMD Resolution not implemented!
	enum BOB_COMMANDS
	{
		CMD_RESOLUTION = 0,
		CMD_ZOOM_IN,
		CMD_ZOOM_OUT,
		CMD_TURN_LEFT,
		CMD_TURN_RIGHT,
		CMD_MOVE_LEFT,
		CMD_MOVE_RIGHT,
		CMD_MOVE_UP,
		CMD_MOVE_DOWN,
	};

	// Constructor, Destructor
	//Creates Scene
	Bob(VistaSystem *pSys);
	~Bob();

	// Actually executes the command!
	bool OnCmdAction(BOB_COMMANDS eDirection, float fValue=0.0f);



private:

	//Used to rotate/translate the scene.
	bool Translate(const VistaVector3D &v3Trans);
	bool Rotate(float nDeg);


	// VistaSystem, Scene
	// We need them for our scene.
	VistaSystem *m_pVistaSystem;
	VistaVirtualPlatform *m_pVirtualPlatform;
};

#endif //__BOB_H
