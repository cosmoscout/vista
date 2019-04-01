#version 120

uniform sampler2D texture;
uniform vec2 v2TexScale = vec2( 1.0, 1.0 );
uniform vec2 v2LensCenter;
uniform vec4 v4DistortionParams = vec4( 1.0, 0.22, 0.24, 0.0 );

void main()
{
	// transform texcoords to lense-entric and [-1,1]
	vec2 v2TexCoords = gl_TexCoord[0].st * 2.0 - 1.0;
	v2TexCoords -= v2LensCenter;

	float nSqDist = v2TexCoords.s * v2TexCoords.s + v2TexCoords.t * v2TexCoords.t;
	float nDistortion = v4DistortionParams.x
						+ v4DistortionParams.y * nSqDist
						+ v4DistortionParams.z * nSqDist * nSqDist
						+ v4DistortionParams.w * nSqDist * nSqDist * nSqDist;

						// warp the coords
	vec2 v2WarpedCoords = v2TexScale * nDistortion * v2TexCoords;
	// transform back from lense-centric [-1,1] to [0,1]
	v2WarpedCoords += v2LensCenter;
	v2WarpedCoords = ( v2WarpedCoords + 1.0 ) / 2.0;
	
	if( v2WarpedCoords.s < 0.0 || v2WarpedCoords.s > 1.0 || v2WarpedCoords.t < 0.0 || v2WarpedCoords.t > 1.0 )
	{
		gl_FragColor = vec4( 0.0 );
	}
	else
	{
		gl_FragColor = texture2D( texture, v2WarpedCoords );
	}

	/*
	vec2 v2WarpedCoords = v2TexCoords + v2LensCenter;
	v2WarpedCoords = ( v2WarpedCoords + 1.0 ) / 2.0;
	vec2 v2DistToMid = gl_TexCoord[0].st - vec2( 0.5, 0.5 );
	float v2SqDistToMid = v2DistToMid.s * v2DistToMid.s + v2DistToMid.t * v2DistToMid.t;
	
	if( nSqDist < 0.0001 )
		gl_FragColor = vec4( 1.0 );
	else if( v2SqDistToMid < 0.0001 )
		gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
	else
		gl_FragColor = vec4( 1, 0.0, 0.0, 1.0 );
	*/
}
