#version 330 compatibility
/*============================================================================*/
/* INCOMING PER-FRAGMENT ATTRIBUTES											  */
/*============================================================================*/
flat in vec4 var_v4Color;
in vec2  var_v2TexCoord;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform sampler2D u_texBillboard;
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	gl_FragColor = var_v4Color;

	gl_FragColor.a *= texture( u_texBillboard, var_v2TexCoord ).a;

}