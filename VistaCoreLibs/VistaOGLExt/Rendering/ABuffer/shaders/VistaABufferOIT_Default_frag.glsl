
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
in vec3 var_v3Normal;
in vec4 var_v4RelPos;
in vec4 var_v4Color;

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
void StoreFragment( vec4 v4Color );
vec4 ShadeFragment(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor);
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	// compute fragment color
	vec4 v4FragColor = ShadeFragment( 
			normalize( var_v3Normal ), 
			var_v4RelPos.xyz, 
			var_v4Color 
		);

	// write fragment into the A-Buffer
	StoreFragment( v4FragColor );

	// Fragment is already stored in the A-Buffer, so it souldn't be written
	// to the Frame-Buffer.
	discard;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/