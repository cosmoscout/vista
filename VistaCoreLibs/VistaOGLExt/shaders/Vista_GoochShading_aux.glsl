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
// $Id: VistaGLLine_PhongLighting_Ext.glsl 24316 2011-11-24 13:42:15Z jh864363 $
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform float	u_fAlpha = 0.25;
uniform float	u_fBeta	 = 0.5;

uniform vec3	u_v3LightDir	= normalize( vec3( -0.5, -0.5, -0.5 ) );

/*============================================================================*/
/* GLOBAL VARIABLES			    											  */
/*============================================================================*/
float g_fb = 1.0-u_fAlpha;
float g_fy = 1.0-u_fBeta;

/*============================================================================*/
/* FUNCTION IMPLEMENTATION													  */
/*============================================================================*/
vec4 ShadeFragment(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor)
{
	vec3 v3CoolColor	= u_fAlpha * v4MatColor.rgb + vec3(  0.0,  0.0, g_fb );
	vec3 v3WarmColor	= u_fBeta  * v4MatColor.rgb + vec3( g_fy, g_fy,  0.0 );	

	float	fNDotL		= dot( u_v3LightDir, v3Normal );
	vec3	v3Reflect 	= reflect( u_v3LightDir, v3Normal );
	float	fRDotP		= dot( v3Reflect, normalize(-v3Position));
	float	fSpecular	= pow( max( fRDotP, 0.0 ), 32.0 );

	vec4 v4Color;
	v4Color.rgb  = mix( v3WarmColor, v3CoolColor, ((1.0+fNDotL)/2.0) );
	v4Color.rgb += 0.5 * fSpecular;
	v4Color.a	 = v4MatColor.a;

	return v4Color;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
