#version 330 compatibility
/*============================================================================*/
/* INCOMING PER-VERTEX ATTRIBUTES											  */
/*============================================================================*/
layout(location = 0) in vec4 in_v4Position;
layout(location = 1) in vec3 in_v3Normal;
layout(location = 2) in vec4 in_v4TexCoord;
layout(location = 3) in vec4 in_v4Scalars;

/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
out vec4 var_v4Position;
out vec4 var_v4RelPos;
out vec3 var_v3Normal;
out vec4 var_v4TexCoord;
out vec4 var_v4Scalars;

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	var_v3Normal   = gl_NormalMatrix     * in_v3Normal;
	var_v4TexCoord = gl_TextureMatrix[0]*in_v4TexCoord;
	var_v4Scalars  = in_v4Scalars;
	
	var_v4Position = in_v4Position;
	var_v4RelPos   = gl_ModelViewMatrix  * in_v4Position;
	gl_Position    = gl_ProjectionMatrix * var_v4RelPos;
}

/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
