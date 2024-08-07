#version 330 compatibility
/*============================================================================*/
/* INCOMING PER-FRAGMENT ATTRIBUTES											  */
/*============================================================================*/
in vec2 var_v2TexCoord;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform sampler2D u_texBillboard;

uniform vec4  u_v4HaloColor; 
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
void main(void)
{
	gl_FragColor    = u_v4HaloColor;
	gl_FragColor.a *= texture( u_texBillboard, var_v2TexCoord ).a;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
