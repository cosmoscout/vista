/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
varying out vec3 var_v3Normal;
varying out vec4 var_v4RelPos;
varying out vec4 var_v4Color;

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main()
{
	var_v4RelPos = gl_ModelViewMatrix * gl_Vertex;
	var_v3Normal = gl_NormalMatrix * gl_Normal;
	var_v4Color  = gl_Color;
	gl_Position  = gl_ProjectionMatrix * var_v4RelPos;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
