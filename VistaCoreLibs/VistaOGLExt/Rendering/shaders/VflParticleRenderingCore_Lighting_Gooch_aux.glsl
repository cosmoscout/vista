#version 330 compatibility
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform vec4 u_v4LightingCoeffs;	// ambient coeff, diffuse coeff, spec coeff, spec power

uniform float	u_fAlpha = 0.25;
uniform float	u_fBeta	 = 0.5;
/*============================================================================*/
/* GLOBAL VARIABLES			    											  */
/*============================================================================*/
float g_fb = 1.0-u_fAlpha;
float g_fy = 1.0-u_fBeta;
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
vec3 ComputeLighting( vec3 v3Color, vec3 v3Normal, vec3 v3LightDir, vec3 v3ViewDir )
{
	vec3 v3CoolColor = u_fAlpha * v3Color + vec3(  0.0,  0.0, g_fb );
	vec3 v3WarmColor = u_fBeta  * v3Color + vec3( g_fy, g_fy,  0.0 );

	float fNDotL    = dot( normalize(v3LightDir), v3Normal );
	vec3  v3Reflect = reflect( -v3LightDir, v3Normal );
	float fRDotP    = dot( normalize(v3Reflect), normalize(v3ViewDir) );
	float fSpecular = pow( max( fRDotP, 0.0 ), u_v4LightingCoeffs.w );

	return mix( v3WarmColor, v3CoolColor, ((1.0-fNDotL)/2.0) ) 
		   + vec3( u_v4LightingCoeffs.z * fSpecular );
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
