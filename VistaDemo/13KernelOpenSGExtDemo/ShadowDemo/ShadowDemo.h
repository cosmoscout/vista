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


#ifndef _SHADOWDEMO_H
#define _SHADOWDEMO_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <string>

#include <VistaKernel/VistaSystem.h>

#include <VistaKernelOpenSGExt/VistaOpenSGShadow.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaGeomNode;
class VistaGeometry;
class VistaDirectionalLight;
class VistaDirectionalLight;
class VistaEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class ShadowDemo
{
public:
	ShadowDemo(int argc = 0, char  *argv[] = NULL);
	virtual ~ShadowDemo();

	bool Run();
	bool CreateScene();

	void SetShadowEnabled( bool b );
	bool GetShadowEnabled();

	void UpdateLightAnimation(VistaEvent *pEvent);

	void SetActiveShadowMode( VistaOpenSGShadow::eShadowMode mode );
	VistaOpenSGShadow::eShadowMode GetActiveShadowMode();

	static std::string GetShadowModeName(VistaOpenSGShadow::eShadowMode mode);

	void ScaleShadowMapResolution( const float nFactor );

private:
	VistaSystem                    mVistaSystem;
	VistaDirectionalLight          *m_pLight;
	VistaOpenSGShadow              *m_pShadow;
	int								m_nMapSize;
};


#endif // _SHADOWDEMO_H
