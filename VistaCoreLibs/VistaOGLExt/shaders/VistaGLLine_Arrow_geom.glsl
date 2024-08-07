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
// $Id: VistaGLLine_Cylinder_GS.glsl 32874 2012-11-22 13:32:20Z jh864363 $
#version 330 compatibility

layout(lines) in;
layout(triangle_strip, max_vertices = 16) out;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform float u_fLineRadius = 0.01f;
uniform float u_fConeRadius = 0.02f;
uniform float u_fConeHeight = 0.04f;

/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
in vec4 var_v4PrimaryColor[];
in vec4 var_v4SecondaryColor[];

out vec3 var_v3Position;
flat out vec4	var_v4Color1;
flat out vec4	var_v4Color2;
flat out vec3	var_v3Point1;
flat out vec3	var_v3Point2;
flat out vec3	var_3LineDir;
flat out float	var_fLineLength;

/*============================================================================*/
/* GLOBAL VARIABLES	AND FUNKTIONS											  */
/*============================================================================*/
vec3 v3Z = vec3( 0.0, 0.0, 1.0 );

void CreateVertex( vec3 v3Pos )
{
	var_v3Position = v3Pos;
	gl_Position = gl_ProjectionMatrix*vec4( var_v3Position, 1 );
	EmitVertex();
}

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
// The following triangle strips are created by this geometry shader:
//
//        3------------ ... ------------7
//      / |                           / |
//    /   |                         /   |
//  2-----+------ ... ------------6     |
//  |  P1 |                       |  P2 |
//  |     1------------ ... ------+-----4
//  |   /                         |   /
//  | /                           | /
//  0------------ ... ------------4
//
// triangle_strip 1
//
//        3------------ ... ------------7
//      / |                           / |
//    /   |                         /   |
//  2     |                       6     |
//  |  P1 |                       |  P2 |
//  |     1------------ ... ------+-----4
//  |   /                         |   /
//  | /                           | /
//  0                             4
//
// triangle_strip 2
//
//        3------------ ... ------------7
//      /                             /
//    /                             /
//  2------------ ... ------------6
//  |                             |
//  |     1------------ ... ------+-----4
//  |   /                         |   /
//  | /                           | /
//  0------------ ... ------------4
//
void main ()
{	
	vec3 v3Dir = normalize( gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz );

	vec3 v3X = normalize( cross( v3Dir, v3Z ) );
	vec3 v3Y = normalize( cross( v3Dir, v3X ) );

	// pass values to fragment shader
	var_v4Color1 = var_v4PrimaryColor[0];
	var_v4Color2 = var_v4PrimaryColor[1];
	var_v3Point1 = gl_in[0].gl_Position.xyz;
	var_v3Point2 = gl_in[1].gl_Position.xyz;
	var_3LineDir	= normalize( var_v3Point2 - var_v3Point1 );
	var_fLineLength = length( var_v3Point2 - var_v3Point1 );

	vec3 v4Points[8] = vec3[8](
			gl_in[0].gl_Position.xyz + u_fConeRadius*( -v3X -v3Y ),
			gl_in[0].gl_Position.xyz + u_fConeRadius*( +v3X -v3Y ),
			gl_in[0].gl_Position.xyz + u_fConeRadius*( -v3X +v3Y ),
			gl_in[0].gl_Position.xyz + u_fConeRadius*( +v3X +v3Y ),
			gl_in[1].gl_Position.xyz + u_fConeRadius*( -v3X -v3Y ),
			gl_in[1].gl_Position.xyz + u_fConeRadius*( +v3X -v3Y ),
			gl_in[1].gl_Position.xyz + u_fConeRadius*( -v3X +v3Y ),
			gl_in[1].gl_Position.xyz + u_fConeRadius*( +v3X +v3Y )
		);

	CreateVertex( v4Points[0] );
	CreateVertex( v4Points[2] );
	CreateVertex( v4Points[1] );
	CreateVertex( v4Points[3] );
	CreateVertex( v4Points[5] );
	CreateVertex( v4Points[7] );
	CreateVertex( v4Points[4] );
	CreateVertex( v4Points[6] );

	EndPrimitive();

	CreateVertex( v4Points[7] );
	CreateVertex( v4Points[3] );
	CreateVertex( v4Points[6] );
	CreateVertex( v4Points[2] );
	CreateVertex( v4Points[4] );
	CreateVertex( v4Points[0] );
	CreateVertex( v4Points[5] );
	CreateVertex( v4Points[1] );

	EndPrimitive();
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
