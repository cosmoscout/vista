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
// $Id: VistaGLLine_HaloLine_frag.glsl 33944 2013-02-18 16:15:38Z jh864363 $
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform sampler1D texSamplerTMU0;
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
in vec2 var_v2TexCoord;
in vec4 var_v4Color1;
in vec4 var_v4Color2;
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	float x = 2.0*var_v2TexCoord.x - 1.0;
	float y = 2.0*var_v2TexCoord.y - 1.0;

	float fRadius = sqrt( x*x + y*y );

	if(fRadius>1.0) discard;
	float fAlpha = texture( texSamplerTMU0, fRadius ).a;

	gl_FragColor = mix( var_v4Color1, var_v4Color2, fAlpha );
	//gl_FragColor = vec4(fAlpha, 0, 0, 1);
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
