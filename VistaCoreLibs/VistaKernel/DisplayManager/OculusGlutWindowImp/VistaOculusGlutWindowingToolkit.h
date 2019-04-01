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
// $Id: VistaGlutWindowingToolkit.h 42018 2014-05-16 12:11:52Z dr165799 $

#ifndef _VISTAOCULUSGLUTWINDOWINGTOOLKIT_H
#define _VISTAOCULUSGLUTWINDOWINGTOOLKIT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/DisplayManager/GlutWindowImp/VistaGlutWindowingToolkit.h>
#include <OVR.h>
#include <OVR_CAPI_GL.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
union ovrGLConfig;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Oculus Window extensions around the Glut windowing toolkit
 */
class VISTAKERNELAPI VistaOculusGlutWindowingToolkit : public VistaGlutWindowingToolkit
{
public:
	VistaOculusGlutWindowingToolkit();
	~VistaOculusGlutWindowingToolkit();

	virtual void DisplayWindow( const VistaWindow* pWindow );

	virtual bool RegisterWindow( VistaWindow* pWindow );
	virtual bool UnregisterWindow( VistaWindow* pWindow );
	
	ovrHmd GetHmdForWindow( VistaWindow* pWindow );


	virtual bool InitWindow( VistaWindow* pWindow );
	bool FillOculusRenderConfigForWindow( VistaWindow* pWindow, ovrGLConfig& oOculusConfig );

private:
	class Internal;
	Internal* m_pData;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
#endif // _VISTAOCULUSGLUTWINDOWINGTOOLKIT_H
