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


#include <VistaDataFlowNet/VdfnObjectRegistry.h>
#include <VistaDataFlowNet/VdfnReadWorkspaceNode.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>

#include <VistaAspects/VistaTransformable.h>

#include <VistaBase/VistaVectorMath.h>

#include "PhantomWorkspaceHandler.h"

PhantomWorkspaceHandler::PhantomWorkspaceHandler( VdfnObjectRegistry *pDFNRegistry, IVistaTransformable * pTransformable )
: m_pDFNRegistry( pDFNRegistry )
, m_pTransformable( pTransformable )
{
}

PhantomWorkspaceHandler::~PhantomWorkspaceHandler()
{
}

void PhantomWorkspaceHandler::HandleEvent(VistaEvent *pEvent)
{
	/**
	 * Transform the transformable like the cam moves,
	 * this simulates hanging the transformable under
	 * the cam in the scenegraph
	 */
	IVistaTransformable *pCam = m_pDFNRegistry->GetObjectTransform("CAM:MAIN");
	if( pCam )
	{
		float v[16];
		pCam->GetTransform(v);

		m_pTransformable->SetTransform(v);
	}
}
