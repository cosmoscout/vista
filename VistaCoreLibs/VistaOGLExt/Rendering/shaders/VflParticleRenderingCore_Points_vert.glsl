#version 330 compatibility
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform sampler2D u_texParticleData;
uniform ivec2     u_v2ParticleDataSize;

uniform vec2      u_v2RangeParams; // { MinScalar, 1.0/( MaxScalar - MinScalar ) }
uniform sampler1D u_texLookupTable;
uniform vec4      u_v4ParticleColor;
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
flat out vec4 var_v4Color;
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	// read particle Data
	vec2 v2TexCoord = vec2( 
		(mod( gl_InstanceID,  u_v2ParticleDataSize.x ) + 0.5 ) / u_v2ParticleDataSize.x, 
		(	( gl_InstanceID / u_v2ParticleDataSize.x ) + 0.5 ) / u_v2ParticleDataSize.y);

	vec4 v4ParticleData = texture( u_texParticleData, v2TexCoord );

	// compute final vertex position
	gl_Position = gl_ModelViewProjectionMatrix * vec4( v4ParticleData.xyz, 1.0 );

	// provide color value and texture coordinates 
	float fScalar  = ( v4ParticleData.w - u_v2RangeParams.x )*u_v2RangeParams.y;
	var_v4Color    = texture( u_texLookupTable,  fScalar ) * u_v4ParticleColor;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
