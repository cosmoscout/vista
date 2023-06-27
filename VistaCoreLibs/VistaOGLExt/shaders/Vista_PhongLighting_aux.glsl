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
// $Id: Vista_PhongLighting_aux.glsl 47412 2015-01-30 09:51:46Z ps790413 $
/*============================================================================*/
/* SHADER FUNKTION															  */
/*============================================================================*/
vec4 ShadeFragment(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor)
{
	vec3 v3Ambient  = vec3(0.0);
	vec3 v3Diffuse   = vec3(0.0);
	vec3 v3Specular = vec3(0.0);
	for(int i = 0; i<gl_MaxLights; ++i)
	{
		vec3 v3LightDir;
		if( gl_LightSource[i].position.w == 0 )
		{
			v3LightDir = normalize(gl_LightSource[i].position.xyz);
		}
		else
		{
			v3LightDir = normalize(gl_LightSource[i].position.xyz-v3Position);
		}

		float fNDotL    = dot( v3LightDir, v3Normal );
		vec3  v3Reflect = reflect( -v3LightDir, v3Normal );
		float fRDotP    = dot( v3Reflect, normalize(-v3Position) );
		float fSpecular = pow( max( fRDotP, 0.0 ), 32.0 );

		v3Ambient += gl_LightSource[i].ambient.rgb;
		v3Diffuse  += gl_LightSource[i].diffuse.rgb  * max( fNDotL, 0 );
		v3Specular+= gl_LightSource[i].specular.rgb * fSpecular;

	}
	
	vec4 v4Color;
	v4Color.rgb  = v4MatColor.rgb * (v3Ambient + v3Diffuse);
	v4Color.rgb += v3Specular;
	v4Color.a    = v4MatColor.a;

	return v4Color;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
