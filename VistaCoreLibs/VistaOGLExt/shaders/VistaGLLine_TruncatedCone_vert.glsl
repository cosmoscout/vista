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
// $Id: VistaGLLine_VS.glsl 24403 2011-12-06 14:50:20Z jh864363 $
#version 330 compatibility
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
out vec4 var_v4PrimaryColor;
out vec4 var_v4SecondaryColor;
out float var_fScale;

/*============================================================================*/
/* VERTEX ATTRIBUTES														  */
/*============================================================================*/
in vec4 gl_Vertex;
in vec4 gl_Color;
in vec4 gl_SecondaryColor;

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	var_fScale = gl_Vertex.w;
	
	var_v4PrimaryColor   = gl_Color;
	var_v4SecondaryColor = gl_SecondaryColor;
	gl_Position          = gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1 );
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
