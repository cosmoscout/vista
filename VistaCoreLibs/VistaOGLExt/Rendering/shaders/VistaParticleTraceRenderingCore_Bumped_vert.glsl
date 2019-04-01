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
uniform float u_fRadius;	   // particle radius

uniform vec2      u_v2RangeParams; // { MinScalar, 1.0/( MaxScalar - MinScalar ) }
uniform sampler1D u_texLookupTable;
uniform vec4      u_v4ParticleColor;

uniform float u_fStartTime;
uniform float u_fLightReductionFactor;

uniform vec4  u_v4LightDir;	   // global direction to the light
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
out vec4 var_v4Color;
out vec2 var_v2TexCoord;

out vec3  var_v3LightDir;
out vec3  var_v3ViewDir;
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	vec3 v3ViewDir = normalize( u_v4ViewerPos.xyz - in_v3Pos );

	vec3 v3Up     = normalize( cross( v3ViewDir, in_v3Tangent ) );
	vec3 v3Offset = normalize( cross( in_v3Tangent, v3Up      ) );

	vec3 v3Vertex = in_v3Pos + u_fRadius*(
			      in_v2Offset.x * v3Offset +
			      in_v2Offset.y * v3Up     );

	gl_Position = gl_ModelViewProjectionMatrix * vec4( v3Vertex, 1.0 );
	
	// provide color value and texture coordinates 
	float fScalar  = ( in_fScalar - u_v2RangeParams.x )*u_v2RangeParams.y;

	var_v4Color      = texture( u_texLookupTable,  fScalar );
	var_v4Color     *= u_v4ParticleColor;
	var_v4Color.rgb *= (1.0f - u_fLightReductionFactor*(u_fStartTime - in_fTime));

	var_v2TexCoord = 0.5 * in_v2Offset.xy + vec2(0.5);
	
	// rotate light direction into local space
	var_v3LightDir.x = dot( u_v4LightDir.xyz,  in_v3Tangent );
	var_v3LightDir.y = dot( u_v4LightDir.xyz,  v3Up        );
	var_v3LightDir.z = dot( u_v4LightDir.xyz,  v3Offset    );

	// calculate half-angle vector in local space
	var_v3ViewDir.x = dot( v3ViewDir.xyz, in_v3Tangent );
	var_v3ViewDir.y = dot( v3ViewDir.xyz, v3Up        );
	var_v3ViewDir.z = dot( v3ViewDir.xyz, v3Offset    );
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
