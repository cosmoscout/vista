/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2016 RWTH Aachen University               */
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


#ifndef VISTA_GL_POINTS_H
#define VISTA_GL_POINTS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaOGLExtConfig.h"

#include <VistaBase/VistaVector3D.h>

#include <vector>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGLSLShader;
class VistaTexture;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
namespace VistaGLPoints
{
	enum
	{
		POINTS_WITH_HALOS,
		SPHERE_WITH_PHONG_LIGTHING,
		SPHERE_WITH_GOOCH_SHADING,
		CONE_WITH_PHONG_LIGTHING,
		CONE_WITH_GOOCH_SHADING,
		NUM_TYPES
	};

	VISTAOGLEXTAPI void SetPointSize( float fSize );
	VISTAOGLEXTAPI void SetHaloSize( float fHaloSize );

	VISTAOGLEXTAPI void SetDirection( const VistaVector3D& v3Dir, bool bDirInViewCoord = true );

	VISTAOGLEXTAPI bool Enable( int iType = POINTS_WITH_HALOS );
	VISTAOGLEXTAPI bool Disable();

	VISTAOGLEXTAPI bool PushAttrib();
	VISTAOGLEXTAPI bool PopAttrib();

	VISTAOGLEXTAPI int AddCustomPointType(
		VistaGLSLShader* pShader, const std::vector<VistaTexture*>& vecTextures );

	VISTAOGLEXTAPI VistaGLSLShader* GetShader( int iType = POINTS_WITH_HALOS );
	VISTAOGLEXTAPI const std::vector<VistaTexture*>* GetTextures( int iType = POINTS_WITH_HALOS );
};
#endif // Include guard.
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
