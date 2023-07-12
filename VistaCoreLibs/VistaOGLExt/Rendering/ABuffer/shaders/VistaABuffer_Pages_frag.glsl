/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2011 RWTH Aachen University               */
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
// $Id: VistaGLSLShader.h 24042 2011-11-04 12:16:11Z jh864363 $
#version 400
#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable
#extension GL_NV_shader_buffer_load : enable
#extension GL_EXT_bindable_uniform : enable
#extension GL_NV_shader_buffer_store : enable
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform uint			g_nWidth;
uniform uint			g_nHeight;

coherent uniform int*	g_pPagePointerBuffer;
coherent uniform uint*	g_pSemaphoreBuffer;
coherent uniform uint*	g_pFragCountBuffer;
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
layout(pixel_center_integer) in vec4 gl_FragCoord;

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	ivec2 v2Coords = ivec2(gl_FragCoord.xy);
	
	if(	v2Coords.x >= 0			&& 
		v2Coords.y >= 0			&&
		v2Coords.x < g_nWidth	&&
		v2Coords.y < g_nHeight	)
	{
		unsigned int idx = v2Coords.x + v2Coords.y*g_nWidth;

		g_pPagePointerBuffer[idx] = -1;
		g_pSemaphoreBuffer[idx]   = 0U;
		g_pFragCountBuffer[idx]   = 0U;
	}

	discard;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
