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


#ifndef _VISTAFADEOUTOVERLAY_H
#define _VISTAFADEOUTOVERLAY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaBase/VistaColor.h>
#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaViewport;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Overlay that simply fills the whole viewport with a single color (e.g. for fadeouts)
 */
class VISTAKERNELAPI VistaColorOverlay : public IVistaSceneOverlay
{
public:
	VistaColorOverlay( VistaDisplayManager* pDisplayManager,
						const std::string& sViewportName = "" );
	VistaColorOverlay( VistaViewport* pViewport );
	~VistaColorOverlay();

	virtual bool GetIsEnabled() const;

	virtual void SetIsEnabled( bool bEnabled );

	virtual void UpdateOnViewportChange( int iWidth, int iHeight, int iPosX, int iPosY );

	virtual bool Do();
	VistaColor GetColor() const;
	void SetColor( const VistaColor& oColor );
	/**
	 * G/SetOpacity retrieves/modifies only the alpha component of the color
	 */
	float GetOpacity() const;
	void SetOpacity( const float nOpacity );
private:
	bool m_bEnabled;
	VistaColor m_oColor;
};

#endif // _VISTAFADEOUTOVERLAY_H

