#version 330 compatibility
/*============================================================================*/
/* INCOMING PER-VERTEX ATTRIBUTES											  */
/*============================================================================*/
layout(location = 0) in vec4  in_v2Offset;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform sampler2D u_texParticleData;
uniform sampler2D u_texParticleRadii;

uniform ivec2     u_v2ParticleDataSize;

uniform vec4  u_v4ViewerPos;   // viewer position
uniform float u_fRadius;	   // particle radius

uniform vec2      u_v2RangeParams; // { MinScalar, 1.0/( MaxScalar - MinScalar ) }
uniform sampler1D u_texLookupTable;
uniform vec4      u_v4ParticleColor;

uniform vec4  u_v4LightDir;	   // global direction to the light
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
flat out vec4 var_v4Color;
out vec2 var_v2TexCoord;

flat out vec3  var_v3LightDir;

out vec3 var_v3RelPos;
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
	float fRadius = u_fRadius*texture( u_texParticleRadii, v2TexCoord ).r;

	// compute local axis
	vec3 v3ViewDir = normalize( u_v4ViewerPos.xyz - v4ParticleData.xyz );
	vec3 v3Right   = normalize( cross( vec3(0.0, 1.0, 0.0), v3ViewDir ) );
	vec3 v3Up      = normalize( cross( v3ViewDir, v3Right ) );
		
	// compute final vertex position
	vec3 v3Vertex = v4ParticleData.xyz
			      + fRadius * in_v2Offset.x * v3Right
			      + fRadius * in_v2Offset.y * v3Up;

	gl_Position = gl_ModelViewProjectionMatrix * vec4( v3Vertex, 1.0 );

	// provide color value and texture coordinates 
	float fScalar  = ( v4ParticleData.w - u_v2RangeParams.x )*u_v2RangeParams.y;
	var_v4Color    = texture( u_texLookupTable,  fScalar ) * u_v4ParticleColor;
	var_v2TexCoord = 0.5 * in_v2Offset.xy + vec2(0.5);
	
	// rotate light direction into local space
	var_v3LightDir.x = dot( u_v4LightDir.xyz, v3Right   );
	var_v3LightDir.y = dot( u_v4LightDir.xyz, v3Up      );
	var_v3LightDir.z = dot( u_v4LightDir.xyz, v3ViewDir );

	var_v3RelPos = u_v4ViewerPos.xyz - v4ParticleData.xyz;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
