#version 330 compatibility
/*============================================================================*/
/* INCOMING PER-FRAGMENT ATTRIBUTES											  */
/*============================================================================*/
flat in vec4 var_v4Color;
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
void main(void)
{
	gl_FragColor = var_v4Color;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
