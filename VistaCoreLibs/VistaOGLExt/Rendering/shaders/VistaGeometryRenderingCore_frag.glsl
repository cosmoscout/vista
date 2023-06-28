#version 330 compatibility
/*============================================================================*/
/* INCOMING PER-FRAGMENT ATTRIBUTES											  */
/*============================================================================*/
in vec4 var_v4Position;
in vec4 var_v4RelPos;
in vec3 var_v3Normal;
in vec4 var_v4TexCoord;
in vec4 var_v4Scalars;

out vec4 out_v4FagmentColor;

/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform vec4 u_v4Color    = vec4( 1.0, 0.0, 0.0, 1.0 );
uniform int  u_bUseLUT    = 0;
uniform vec2 u_v2LutRange = vec2(0.0, 2.5e-07);

uniform sampler1D u_texLUT;

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
vec4 ShadeFragment(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor);

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main()
{
	float fValue	= ( var_v4Scalars.x - u_v2LutRange.x )
					/ ( u_v2LutRange.y  - u_v2LutRange.x );
				
	vec4 v4Color[2] = vec4[2]( u_v4Color, texture(u_texLUT, fValue) );
	
	out_v4FagmentColor = 
		ShadeFragment( 
			normalize(var_v3Normal), 
			var_v4RelPos.xyz, 
			v4Color[u_bUseLUT]
		);
}

/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/