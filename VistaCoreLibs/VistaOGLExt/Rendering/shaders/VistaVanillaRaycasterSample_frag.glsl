#version 130 // OpenGL 3.0 for global function texture(...)

uniform sampler1D u_lookup_sampler;
uniform sampler3D u_volume_sampler;

uniform vec2 u_lut_range;


vec4 DetermineSampleColor( vec3 sample_pos )
{
	// Retrieve volume value, re-normalize it, and color it using the LUT
	float volume_value = texture( u_volume_sampler, sample_pos ).r;
	float normalized_value = ( volume_value - u_lut_range.x ) / ( u_lut_range.y - u_lut_range.x );
	return texture( u_lookup_sampler, normalized_value );
}
