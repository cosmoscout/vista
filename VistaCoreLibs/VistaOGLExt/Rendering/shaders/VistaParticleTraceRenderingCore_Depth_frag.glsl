#version 330 compatibility
/*============================================================================*/
/* INCOMING PER-FRAGMENT ATTRIBUTES											  */
/*============================================================================*/
in vec4 var_v4Color;
in vec2 var_v2TexCoord;

in vec3 var_v3LightDir;
in vec3 var_v3ViewDir;

in vec3 var_v3RelPos;
in float var_fDepthReplaceFactor;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform sampler2D     u_texBillboard;
uniform sampler2DRect u_texDepth;

uniform vec4  u_v4ViewerPos;   // viewer position
uniform float u_fRadius;	   // particle radius
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
vec3 ComputeLighting( vec3 v3Color, vec3 v3Normal, vec3 v3LightDir, vec3 v3ViewDir );
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	vec4 v4TexCol = texture( u_texBillboard, var_v2TexCoord );

	float fDepth = texture( u_texDepth, gl_FragCoord.xy ).r;

	if( fDepth < gl_FragCoord.z )
		discard;
	
	vec3 v3Normal = normalize( v4TexCol.xyz * 2.0 - vec3(1.0) );
	
	gl_FragColor.rgb = ComputeLighting(
		var_v4Color.rgb, v3Normal, var_v3LightDir, var_v3ViewDir );

	gl_FragColor.a   = var_v4Color.a * v4TexCol.a;

	// compute coreckt deapth value;
	float fDist = length( var_v3RelPos );
	fDist = ( fDist - v4TexCol.z * u_fRadius * var_fDepthReplaceFactor )/fDist;

	vec4 v4Pos = vec4( u_v4ViewerPos.xyz - var_v3RelPos*fDist, 1 );

	float z = dot( gl_ModelViewProjectionMatrixTranspose[2], v4Pos );
	float w = dot( gl_ModelViewProjectionMatrixTranspose[3], v4Pos );

	gl_FragDepth = 0.5*(z/w) + 0.5;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
