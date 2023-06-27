#version 140											// OpenGL 3.1 for layout-qualifier
#extension GL_ARB_explicit_attrib_location : enable 	// For layout( location = x )

in vec3 tex_coords;

layout( location = 0 ) out vec4 frag_colors[2];


void main()
{
	// Write to index 0 if front-facing, to 1 otherwise.
	int iTargetIndex = int( !gl_FrontFacing );
	frag_colors[iTargetIndex] = vec4( tex_coords, 1.0 );
	frag_colors[1-iTargetIndex] = vec4( 0.0 );
}
