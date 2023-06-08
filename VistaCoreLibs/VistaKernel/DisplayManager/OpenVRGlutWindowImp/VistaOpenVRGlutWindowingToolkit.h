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
// $Id: VistaGlutWindowingToolkit.h 42018 2014-05-16 12:11:52Z dr165799 $

#ifndef _VISTAOPENVRGLUTWINDOWINGTOOLKIT_H
#define _VISTAOPENVRGLUTWINDOWINGTOOLKIT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/DisplayManager/GlutWindowImp/VistaGlutWindowingToolkit.h>
    
#include <openvr/openvr.h>
#include <openvr/openvr_capi.h>

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
 * OpenVR Window extensions around the Glut windowing toolkit
 */
class VISTAKERNELAPI VistaOpenVRGlutWindowingToolkit : public VistaGlutWindowingToolkit
{
public:
	VistaOpenVRGlutWindowingToolkit();
	~VistaOpenVRGlutWindowingToolkit();

	virtual void DisplayWindow( const VistaWindow* pWindow );

	virtual bool RegisterWindow( VistaWindow* pWindow );
	
	vr::IVRSystem* GetVRSystem();

	virtual bool InitWindow( VistaWindow* pWindow );
private:

	vr::IVRSystem* m_pVRSystem;
	// class Internal;
	// Internal* m_pData;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
#endif // _VISTAOCULUSGLUTWINDOWINGTOOLKIT_H
