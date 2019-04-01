uniform sampler2D texture_lefteye;
uniform sampler2D texture_righteye;

void main()
{		
	vec4 v3LeftColor = texture2D( texture_lefteye, gl_TexCoord[0].st );
	vec4 v3RightColor = texture2D( texture_righteye, gl_TexCoord[0].st );
	float nLeftAccum = ( v3LeftColor.r + v3LeftColor.g + v3LeftColor.b ) / 3.0;
	float nRightAccum = ( v3RightColor.r + v3RightColor.g + v3RightColor.b ) / 3.0;
	gl_FragColor = vec4( nLeftAccum, nRightAccum, nRightAccum, 1.0 );
}
