#version 140											// OpenGL 3.1 for layout-qualifier
#extension GL_ARB_explicit_attrib_location : enable 	// For layout( location = x )

uniform sampler2DRect u_ray_entry;
uniform sampler2DRect u_ray_exit;

layout( location = 0 ) out vec4 frag_color;

vec4 TraverseVolume( vec3 ray_entry_pt, vec3 ray_exit_pt );


void main()
{
	// Determine interval that needs to be traversed
	vec3 ray_entry_pt = clamp( texture( u_ray_entry, gl_FragCoord.xy ).rgb, 0.0, 1.0 );
	vec3 ray_exit_pt = clamp( texture( u_ray_exit, gl_FragCoord.xy ).rgb, 0.0, 1.0 );

	// Perform traversal and write out final color
	frag_color = TraverseVolume( ray_entry_pt, ray_exit_pt );
}
