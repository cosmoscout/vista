#version 330 compatibility
/*============================================================================*/
/* INCOMING PER-VERTEX ATTRIBUTES											  */
/*============================================================================*/
layout(location = 0) in vec3   in_v3Pos;
layout(location = 1) in vec3   in_v3Tangent;
layout(location = 2) in float  in_fScalar;
layout(location = 3) in float  in_fTime;
layout(location = 4) in vec2   in_v2Offset;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform vec4  u_v4ViewerPos;   // viewer position
uniform float u_fRadius = 0.1;	   // particle radius

uniform vec2      u_v2RangeParams; // { MinScalar, 1.0/( MaxScalar - MinScalar ) }
uniform sampler1D u_texLookupTable;
uniform vec4      u_v4ParticleColor;

uniform float u_fStartTime;
uniform float u_fLightReductionFactor;
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
out vec4 var_v4Color;
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	vec3 v3ViewDir = normalize( u_v4ViewerPos.xyz - in_v3Pos );

	gl_Position = gl_ModelViewProjectionMatrix * vec4( in_v3Pos, 1.0 );
	
	// provide color value and texture coordinates 
	float fScalar  = ( in_fScalar - u_v2RangeParams.x )*u_v2RangeParams.y;

	var_v4Color      = texture( u_texLookupTable,  fScalar );
	var_v4Color     *= u_v4ParticleColor;
	var_v4Color.rgb *= (1.0f - u_fLightReductionFactor*(u_fStartTime - in_fTime));
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
