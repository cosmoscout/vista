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
// $Id: VistaGLLine_Tubelets_frag.glsl 47469 2015-02-03 15:58:17Z jh864363 $
#version 330 compatibility
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform float u_fLineRadius;
uniform int   u_bComputeLighting = 1;

/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
in vec3 var_v3Position;
in vec2 var_v2TexCoord;
in vec3 var_v3LocalXAxis;
in vec3 var_v3LocalYAxis;
in vec4 var_v4Color1;

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

vec4 ShadeFragment(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor);
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	float x = 2.0*var_v2TexCoord.x - 1.0;
	float y = 2.0*var_v2TexCoord.y - 1.0;

	float f = x*x+y*y;
	if(f>1.0)
		discard;

	float z = sqrt(1.0-f);

	vec3 v3Dir = normalize( var_v3Position    );
	vec3 v3X   = normalize( var_v3LocalXAxis  );
	vec3 v3Y   = normalize( var_v3LocalYAxis  );
	vec3 v3Z   = normalize( cross( v3Y, v3X ) );

	//Compute fragment depth

	f = -dot( v3Dir, v3Z );

	if(f<0.0001)
		f=0.0001;
	f = u_fLineRadius*z / f;

 	vec3 v3Normal = normalize( x*v3X + y*v3Y + z*v3Z );

	//compute fragmenth color
	vec4 v4Pos = vec4( var_v3Position, 1 );
	v4Pos.xyz -= f*v3Dir;

	if (u_bComputeLighting>0)
		gl_FragColor = ShadeFragment(v3Normal, v4Pos.xyz, var_v4Color1);
	else
		gl_FragColor = var_v4Color1;

	//compute fragmenth depth
	v4Pos = gl_ProjectionMatrix * v4Pos;
	gl_FragDepth = (v4Pos.z / v4Pos.w + 1.0)* 0.5;

}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/