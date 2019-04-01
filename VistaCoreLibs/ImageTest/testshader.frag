uniform sampler2D texture;

void main()
{		
	vec4 v3TexColor = texture2D( texture, gl_TexCoord[0].st );
	gl_FragColor = vec4( 1.0 - v3TexColor.r, 1.0 - v3TexColor.g, 1.0 - v3TexColor.b, 1.0 );
}
