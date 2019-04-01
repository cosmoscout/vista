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
// $Id: VistaGLLine_Cylinder_GS.glsl 30539 2012-06-11 13:21:29Z jh864363 $
#version 330 compatibility

layout(lines) in;
layout(triangle_strip, max_vertices = 16) out;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform float u_fLineRadius;

/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
in vec4  var_v4PrimaryColor[];
in vec4  var_v4SecondaryColor[];
in float var_fScale[];

out vec3 var_v3Position;
flat out vec3  var_v3Point1;
flat out vec3  var_v3Point2;
flat out vec4  var_v4Color1;
flat out vec4  var_v4Color2;
flat out float var_fRadius1;
flat out float var_fRadius2;

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
//        3-------+-------- ... --------+-------7
//      / |     / |                   / |     / |
//    /   |   /   |                 /   |   /   |
//  2-----+-+-----+-- ... --------+-----+-6     |
//  |     | |  P1 |               |  P2 | |     |
//  |     1-+-----+-------- ... --------+-+-----5
//  |   /   |   /                 |   /   |   /
//  | /     | /                   | /     | /
//  0-------+-------- ... --------+-------4
//
// triangle_strip 1
//
//        3---------------- ... ----------------7
//      / |                                   / |
//    /   |                                 /   |
//  2     |                               6     |
//  |     |                               |     |
//  |     1---------------- ... ----------+-----5
//  |   /                                 |   /
//  | /                                   | /
//  0                                     4
//
// triangle_strip 2
//
//        3---------------- ... ----------------7
//      /                                     /
//    /                                     /
//  2---------------- ... ----------------6
//  |                                     |
//  |     1---------------- ... ----------+-----5
//  |   /                                 |   /
//  | /                                   | /
//  0---------------- ... ----------------4
//
void main ()
{	
	vec3 v3Dir = normalize( gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz );
	
	vec3 v3X = normalize( cross( v3Dir, v3Z ) );
	vec3 v3Y = normalize( cross( v3Dir, v3X ) );

	// pass values to fragment shader
	var_v3Point1 = gl_in[0].gl_Position.xyz;
	var_v3Point2 = gl_in[1].gl_Position.xyz;
	var_v4Color1 = var_v4PrimaryColor[0];
	var_v4Color2 = var_v4PrimaryColor[1];
	var_fRadius1 = u_fLineRadius*var_fScale[0];
	var_fRadius2 = u_fLineRadius*var_fScale[1]; 
	
	float fRadius = max( var_fRadius1, var_fRadius2 );

	vec3 v4Points[8] = vec3[8](
			gl_in[0].gl_Position.xyz + fRadius*( -v3X -v3Y -v3Dir ),
			gl_in[0].gl_Position.xyz + fRadius*( +v3X -v3Y -v3Dir ),
			gl_in[0].gl_Position.xyz + fRadius*( -v3X +v3Y -v3Dir ),
			gl_in[0].gl_Position.xyz + fRadius*( +v3X +v3Y -v3Dir ),
			gl_in[1].gl_Position.xyz + fRadius*( -v3X -v3Y +v3Dir ),
			gl_in[1].gl_Position.xyz + fRadius*( +v3X -v3Y +v3Dir ),
			gl_in[1].gl_Position.xyz + fRadius*( -v3X +v3Y +v3Dir ),
			gl_in[1].gl_Position.xyz + fRadius*( +v3X +v3Y +v3Dir )
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
