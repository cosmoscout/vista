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


#ifndef VISTA_GL_LINE_H
#define VISTA_GL_LINE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaOGLExtConfig.h"

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGLSLShader;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
namespace VistaGLLine
{
	enum ELineTyps{
		VISTA_GL_LINES = 0,
		VISTA_GL_LINE_STRIP
	};

	enum EShader{
		SHADER_HALO_LINES = 0,
		SHADER_TUBELETS,
		SHADER_ADJACENCY_HALO_LINES,
		SHADER_ADJACENCY_TUBELETS,
		SHADER_CYLINDER,
		SHADER_ARROWS,
		SHADER_TRUNCATED_CONES,
		NUM_SHADES
	};

	/**
	 * Sets the line radius ( default = 0.01 )
	 */
	VISTAOGLEXTAPI void SetLineWidth(float fLineWith);


	/**
	 * Specifies whether the shader does lighting computation.
	 * This is set to true by default
	 */
	VISTAOGLEXTAPI void SetEnableLighting(bool b);

	/**
	 * Sets the size of the line Halo in percent.
	 * Only used by SHADER_HALO_LINES or SHADER_ADJACENCY_HALO_LINES.
	 * The Halo size has no influence on the line radius
	 */
	VISTAOGLEXTAPI void SetHaloSize(float fHaloSize);

	/**
	 * Only used by SHADER_ARROWS.
	 * Sets the radius of the arrowhead.
	 */

	VISTAOGLEXTAPI void SetArrowheadRadius(float fArrowheadRadius);
	/**
	 * Only used by SHADER_ARROWS.
	 * Sets the length of the arrowhead.
	 * NOTE: the length of the arrowhead should always be smaller than the length of the line
	 */
	VISTAOGLEXTAPI void SetArrowheadLength(float fArrowheadLength);
	
	/**
	 * should be used in stat of glBegin() when using VistaGLLine.
	 * You have to call VistaGLLine::Enable() before calling this.
	 * This function will call glBegin(GL_LINES)/glBegin(GL_LINE_STRIP)
	 * if the current bind shader doesn't need adjacency informations, otherwise
	 * glBegin(GL_LINES_ADJACENCY)/glBegin(GL_LINE_STRIP_ADJACENCYNE_STRIP)
	 * is called;
	 *
	 * @param iLineTyp should be VISTA_GL_LINES or VISTA_GL_LINE_STRIP
	 * 
	 */
	VISTAOGLEXTAPI bool Begin(int iLineTyp = VISTA_GL_LINES );
	/**
	 * should be used in stat of glEnd() when using VistaGLLine
	 */
	VISTAOGLEXTAPI bool End();

	/**
	 * this function binds the shader, that is used to draw GL_LINES.
	 */
	VISTAOGLEXTAPI bool Enable(int iShader = SHADER_HALO_LINES);

	/**
	 * disables the current bind shader.
	 */
	VISTAOGLEXTAPI bool Disable();

	/**
	 * Add a new shader, that can be used instead of a default shader.
	 * You should remove the shader you have added by calling
	 * VistaGLLine::RemoveLineShader before you delete it;
	 *
	 * @return the shader handle that can be use at VistaGLLine::Begin or -1 if something faild;
	 */
	VISTAOGLEXTAPI int AddLineShader(VistaGLSLShader* pShader, bool bNeedAdjacencyInformations = false);

	/**
	 * If you have added an extra Shader you should remove it before you delete it
	 */
	VISTAOGLEXTAPI bool RemoveLineShader(VistaGLSLShader* pShader);


	VISTAOGLEXTAPI bool GetDoesShaderNeedAdjacencyInformations(int iShader);
};
#endif // Include guard.
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
