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

uniform float u_fHaloSize; 
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
out vec2 var_v2TexCoord;
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	vec3 v3ViewDir = normalize( u_v4ViewerPos.xyz - in_v3Pos );

	vec3 v3Up     = normalize( cross( v3ViewDir, in_v3Tangent ) );
	vec3 v3Offset = normalize( cross( in_v3Tangent, v3Up      ) );

	vec3 v3Vertex = in_v3Pos + u_fHaloSize*u_fRadius*(
			      in_v2Offset.x * v3Offset +
			      in_v2Offset.y * v3Up );

	// move halos away from viewer to avoid z-fighting
	v3Vertex += 0.01*u_fHaloSize*normalize( v3Vertex - u_v4ViewerPos.xyz );

	gl_Position = gl_ModelViewProjectionMatrix * vec4( v3Vertex, 1.0 );
	
	var_v2TexCoord = 0.5 * in_v2Offset.xy + vec2(0.5);
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
