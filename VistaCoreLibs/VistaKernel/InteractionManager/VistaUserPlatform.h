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


#ifndef _VISTAUSERPLATFORM_H
#define _VISTAUSERPLATFORM_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaTransformNode;
class VistaSceneGraph;
class VistaVirtualPlatformAdapter;
class VistaVirtualPlatform;
class VistaVector3D;
class VistaDisplaySystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * The VistaUserPlatform creates a representation of the camera (or
 * the user) in the scene graph.
 *
 * For the given VistaDisplaySystem, one scenegraph node is created
 * below the root node, representing the virtual platform (i.e. the
 * display system's local coordinate system). Below that, a second
 * node is added representing the user's position relative to the
 * virtual platform.
 *
 * Changes to both the virtual platform and the user's position are
 * observed, and the transformations of the scenegraph nodes updated
 * accordingly. 
 */
class VISTAKERNELAPI VistaUserPlatform
{
public:
	VistaUserPlatform(VistaSceneGraph *pSG,
					   VistaDisplaySystem *);
	virtual ~VistaUserPlatform();


	VistaVirtualPlatformAdapter *GetPlatformTrans() const;
	VistaTransformNode          *GetPlatformNode() const;
	VistaTransformNode          *GetPlatformUserNode() const;

	VistaVirtualPlatform *GetPlatform() const;

	VistaVector3D GetUserViewPosition() const;
protected:
private:
	class UserPlatformObserver;
	class UserObserver;

	UserPlatformObserver        *m_pPlatObs;
	UserObserver                *m_pUserObs;
	VistaVirtualPlatformAdapter *m_pPlatTrans;

	VistaSceneGraph            *m_pSG;
	VistaTransformNode *m_pPlatformNode,
						*m_pUserNode;
	VistaDisplaySystem *m_pDispSys;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAUSERPLATFORM_H


