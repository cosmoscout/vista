#version 150 compatibility								// OpenGL 3.2 to make the 'compatibility' token work
#extension GL_ARB_explicit_attrib_location : enable		// For layout( location = x )

layout( location = 0 ) in vec3 v_position;
layout( location = 1 ) in vec3 v_tex_coords;

out vec3 tex_coords; // Needed by ray generation shader

void main()
{
	tex_coords = v_tex_coords;
		
	gl_Position = gl_ModelViewProjectionMatrix * vec4( v_position, 1.0 );
}
