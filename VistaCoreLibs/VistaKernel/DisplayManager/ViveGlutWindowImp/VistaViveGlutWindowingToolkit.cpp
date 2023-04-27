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
// $Id: VistaGlutWindowingToolkit.cpp 42600 2014-06-18 19:23:49Z dr165799 $

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaViveGlutWindowingToolkit.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#include "VistaBase/VistaTimeUtils.h"
#include "GL/glut.h"
#include "../VistaViewport.h"

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaViveGlutWindowingToolkit::VistaViveGlutWindowingToolkit()
: VistaGlutWindowingToolkit()
// , m_pData( new Internal() )
, m_pVRSystem(nullptr)
{
	if(!vr::VR_IsHmdPresent()){
		vstr::errp()<<"Error: No Vive HMD present while constructing VistaViveGlutWindowingToolkit!\n";
	}

	vr::EVRInitError eError = vr::VRInitError_None;
	m_pVRSystem = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if(eError!=0){
		vstr::errp()<<"Error while connecting to Vive API in VistaViveGlutWindowingToolkit ctor: "<<vr::VR_GetVRInitErrorAsEnglishDescription(eError)<<std::endl;
	}

	if ( !vr::VRCompositor() )
	{
		vstr::errp()<<"Error: Vive compositor initialization failed in VistaViveGlutWindowingToolkit ctor!\n";
	}
}

VistaViveGlutWindowingToolkit::~VistaViveGlutWindowingToolkit()
{
	vr::VR_Shutdown();
}

bool VistaViveGlutWindowingToolkit::RegisterWindow( VistaWindow* pWindow )
{
	bool bSuccess = VistaGlutWindowingToolkit::RegisterWindow( pWindow );

	uint32_t width, height;
	m_pVRSystem->GetRecommendedRenderTargetSize(&width, &height);
	pWindow->GetWindowProperties()->SetSize( width, height );	

	return bSuccess;
}

bool VistaViveGlutWindowingToolkit::InitWindow( VistaWindow* pWindow )
{
	if(!VistaGlutWindowingToolkit::InitWindow(pWindow)) {
		vstr::errp()<<"Couldn't initialize Window for a Vive application." << std::endl;
		return false;
    }

	BindWindow( pWindow );

	return true;
}
void VistaViveGlutWindowingToolkit::DisplayWindow( const VistaWindow* pWindow )
{
	VistaGlutWindowingToolkit::DisplayWindow( pWindow );
	//vr::VRCompositor()->PostPresentHandoff();
}

vr::IVRSystem* VistaViveGlutWindowingToolkit::GetVRSystem()
{
	return m_pVRSystem;
}



/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


