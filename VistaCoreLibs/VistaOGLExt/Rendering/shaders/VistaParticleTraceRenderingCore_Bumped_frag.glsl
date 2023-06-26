#version 330 compatibility
/*============================================================================*/
/* INCOMING PER-FRAGMENT ATTRIBUTES											  */
/*============================================================================*/
in vec4 var_v4Color;
in vec2 var_v2TexCoord;

in vec3 var_v3LightDir;
in vec3 var_v3ViewDir;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform sampler2D u_texBillboard;
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
	
	vec3 v3Normal = normalize( v4TexCol.xyz * 2.0 - vec3(1.0) );
	
	gl_FragColor.rgb = ComputeLighting(
		var_v4Color.rgb, v3Normal, var_v3LightDir, var_v3ViewDir );

	gl_FragColor.a   = var_v4Color.a * v4TexCol.a;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
