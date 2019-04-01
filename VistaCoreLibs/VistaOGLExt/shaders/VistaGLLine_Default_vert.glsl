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
// $Id: VistaGLLine_Default_vert.glsl 47412 2015-01-30 09:51:46Z ps790413 $
#version 330 compatibility
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
out vec4 var_v4PrimaryColor;
out vec4 var_v4SecondaryColor;

/*============================================================================*/
/* VERTEX ATTRIBUTES														  */
/*============================================================================*/

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	var_v4PrimaryColor   = gl_Color;
	var_v4SecondaryColor = gl_SecondaryColor;
	gl_Position          = gl_ModelViewMatrix*gl_Vertex;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
