#version 140											// OpenGL 3.1 for layout-qualifier
#extension GL_ARB_explicit_attrib_location : enable 	// For layout( location = x )

uniform sampler2DRect u_ray_exit;
layout( location = 0 ) out vec4 frag_color;

uniform vec3 tex_coords_for_viewer = vec3( 0.0 );

vec4 TraverseVolume( vec3 ray_entry_pt, vec3 ray_exit_pt );

// @TODO Change fragment depth to near plane as we currently use the depth of
// 		 the exit points. Not changing this might lead to issues when clipping
//		 against the prior-to-raycasting depth buffer.
void main()
{
	// Determine interval that needs to be traversed
	vec3 ray_exit_pt = clamp( texture( u_ray_exit, gl_FragCoord.xy ).rgb, 0.0, 1.0 );
	
	// Perform traversal and write out final color
	frag_color = TraverseVolume( tex_coords_for_viewer, ray_exit_pt );
}
