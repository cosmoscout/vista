#version 330 compatibility
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform vec4 u_v4LightingCoeffs;	// ambient coeff, diffuse coeff, spec coeff, spec power
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
vec3 ComputeLighting( vec3 v3Color, vec3 v3Normal, vec3 v3LightDir, vec3 v3ViewDir )
{
	float fNDotL    = dot( normalize(v3LightDir), v3Normal );
	vec3  v3Reflect = reflect( -v3LightDir, v3Normal );
	float fRDotP    = dot( normalize(v3Reflect), normalize(v3ViewDir) );
	float fSpecular = pow( max( fRDotP, 0.0 ), u_v4LightingCoeffs.w );

	float fSpec    = fSpecular * u_v4LightingCoeffs.z;
	float fDiffAmb = fNDotL * u_v4LightingCoeffs.y + u_v4LightingCoeffs.x;

	return v3Color * fDiffAmb + vec3( fSpec );
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
