/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*            Copyright (c) 2016-2019 German Aerospace Center (DLR)           */
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


#ifndef __VISTAVIVEDRIVERCONFIG_H
#define __VISTAVIVEDRIVERCONFIG_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaBase/VistaQuaternion.h>
#include <VistaBase/VistaTransformMatrix.h>


#if defined(WIN32)
#pragma warning (disable: 4786)
#endif


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


// Single marker data (3DOF):
namespace VistaViveConfig
{
	struct VISTA_vive_stick_type{

		float loc[3];
		float rot[9];
		VistaQuaternion orientation;
		VistaTransformMatrix pose;
		
		bool grip_pressed;
		
		bool trackpad_pressed;
		bool trackpad_touched;
		float trackpad_x;
		float trackpad_y;
		
		bool trigger_pressed;
		bool trigger_touched;
		float trigger_x;
		
		bool button_system_pressed;
		bool button_menu_pressed;
		bool button_a_pressed;
	};

	struct VISTA_vive_head_type{

		float loc[3];
		float rot[9];
		VistaQuaternion orientation;
		VistaTransformMatrix pose;
	};
}

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTAVIVEDRIVERCONFIG_H





