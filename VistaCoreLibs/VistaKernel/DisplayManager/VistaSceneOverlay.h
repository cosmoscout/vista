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


#ifndef _VISTASCENEOVERLAY_H
#define _VISTASCENEOVERLAY_H

#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaBoundingBox;
class VistaDisplayManager;
class VistaViewport;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI IVistaSceneOverlay : public IVistaOpenGLDraw
{
public:
	virtual ~IVistaSceneOverlay();

	// scene overlays do not have a bounding box
	virtual bool GetBoundingBox( VistaBoundingBox &bb );

	virtual bool GetIsEnabled() const = 0;
	virtual void SetIsEnabled( bool bEnabled ) = 0;

	VistaViewport* GetAttachedViewport() const;

protected:
	virtual void UpdateOnViewportChange( int iWidth, int iHeight,
											int iPosX, int iPosY ) = 0;

	IVistaSceneOverlay( VistaDisplayManager* pDisplayManager,
						const std::string& sViewportName = "" );
	IVistaSceneOverlay( VistaViewport* pViewport );

private:
	class ViewportResizeObserver;
	ViewportResizeObserver*		m_pViewportObserver;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASCENEOVERLAY_H

