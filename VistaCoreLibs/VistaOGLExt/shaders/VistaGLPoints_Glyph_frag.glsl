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
// $Id: VistaGLLine_Tubelets_frag.glsl 33944 2013-02-18 16:15:38Z jh864363 $
#version 420 compatibility
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform float u_fPointSize;

layout(binding=0) uniform sampler2D u_texNormal;
layout(binding=1) uniform sampler2D u_texDepth;
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
	float fDepth = texture( u_texDepth, var_v2TexCoord ).a;
	vec3 v3Dir = normalize( var_v3Position    );
	vec4 v4Pos = vec4( var_v3Position, 1 );
	v4Pos.xyz -= fDepth*u_fPointSize*v3Dir;

	//compute fragmenth color
	vec4  v4Normal = texture( u_texNormal, var_v2TexCoord );

	vec3 v3Normal = normalize( 2*v4Normal.xyz - vec3(1) );
	
	vec3 v3X   = normalize( var_v3LocalXAxis  );
	vec3 v3Y   = normalize( var_v3LocalYAxis  );
	vec3 v3Z   = normalize( cross( v3X, v3Y ) );

 	v3Normal = normalize( v3Normal.x*v3X + v3Normal.y*v3Y + v3Normal.z*v3Z );
	
	gl_FragColor   = ShadeFragment(v3Normal, v4Pos.xyz, var_v4Color1);
	gl_FragColor.a = v4Normal.a;

	//compute fragmenth depth
	v4Pos = gl_ProjectionMatrix * v4Pos;
	gl_FragDepth = (v4Pos.z / v4Pos.w + 1.0)* 0.5;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/