#version 150 compatibility	// OpenGL 3.2 to make the 'compatibility' token work, 
							// 'compatibility' for gl_LightSource & gl_ModelViewProjectionMatrix

#extension GL_ARB_conservative_depth : enable // For depth_unchanged
							
// Indicates to the GL that we won't change the fragment depth, increasing the chances of pipeline optimizations.
layout( depth_unchanged ) out float gl_FragDepth;

uniform sampler3D	u_gradient_sampler; // Set to texture used to determine the lighting gradient

uniform int		u_gradient_channel; // Only used for non-alpha-classifed gradient lighting
uniform float 	u_step_size;
uniform float 	u_alpha_saturation_limit;
uniform float 	u_alpha_compensation;


// Defined by user in separate shader file
vec4 DetermineSampleColor( vec3 sample_pos );


#ifdef PERFORM_DEPTH_TEST

// Bindings 0 and 1 are used for entry and exit points textures
uniform sampler2DRect	u_depth_sampler;

bool IsSampleNotOccluded( vec3 sample_pos )
{
	vec4 sample_pos_proj = gl_ModelViewProjectionMatrix * vec4( sample_pos, 1.0 );

	// Fragments, i.e. sample positions close to the near plane might yield
	// slightly negative z and w values at the same time. This will lead to
	// problems during dehomogenization. To avoid this, we set the sample depth
	// to a fixed-zero for thos fragments.
	float sample_depth = 0.0;
	if( sample_pos_proj.w > 0.0 )
		sample_depth = ( sample_pos_proj.z / sample_pos_proj.w * 0.5 + 0.5 );		
	
	return ( sample_depth < texture( u_depth_sampler, gl_FragCoord.xy ).r );
}

#endif // PERFORM_DEPTH_TEST


#ifdef PERFORM_GRADIENT_LIGHTING

#ifdef USE_ALPHA_CLASSIFIED_GRADIENT

vec3 ComputeGradient( vec3 sample_pos, float current_value, vec3 offsets )
{
	vec3 offset_sample_pos = sample_pos + vec3( offsets.x, 0.0, 0.0 );
	float X = DetermineSampleColor( offset_sample_pos ).a;
	
	offset_sample_pos = sample_pos + vec3( 0.0, offsets.y, 0.0 );
	float Y = DetermineSampleColor( offset_sample_pos ).a;
		
	offset_sample_pos = sample_pos + vec3( 0.0, 0.0, offsets.z );
	float Z = DetermineSampleColor( offset_sample_pos ).a;
	
	return vec3( X - current_value, Y - current_value, Z - current_value );
}

#else // Normal, non-classified gradient

vec3 ComputeGradient( vec3 sample_pos, float current_value, vec3 offsets )
{
	vec3 offset_sample_pos = sample_pos + vec3( offsets.x, 0.0, 0.0 );
	float X = texture( u_gradient_sampler, offset_sample_pos )[u_gradient_channel];
	
	offset_sample_pos = sample_pos + vec3( 0.0, offsets.y, 0.0 );
	float Y = texture( u_gradient_sampler, offset_sample_pos )[u_gradient_channel];
		
	offset_sample_pos = sample_pos + vec3( 0.0, 0.0, offsets.z );
	float Z = texture( u_gradient_sampler, offset_sample_pos )[u_gradient_channel];
	
	return vec3( X - current_value, Y - current_value, Z - current_value );
}

#endif

#endif // PERFORM_GRADIENT_LIGHTING

#ifdef PERFORM_RAY_JITTERING

uniform sampler2DRect u_jitter_sampler;
uniform int	u_jitter_texture_size;

#endif // PERFORM_RAY_JITTERING


vec4 TraverseVolume( vec3 ray_entry_pt, vec3 ray_exit_pt )
{
	// Traversal variables
	vec3 ray_dir = normalize( ray_exit_pt - ray_entry_pt );
	vec3 curr_sample_pos = ray_entry_pt;
	vec4 color = vec4( 0.0 );
	
#ifdef PERFORM_RAY_JITTERING
	float ray_offset = texture( u_jitter_sampler, mod( gl_FragCoord.xy, u_jitter_texture_size ) ).r;
	curr_sample_pos += ray_offset * ray_dir * u_step_size;
#endif // PERFORM_RAY_JITTERING
	
#ifdef PERFORM_GRADIENT_LIGHTING
	vec3 offsets = vec3( 0.0 );
	ivec3 tex_sizes = textureSize( u_gradient_sampler, 0 );
	offsets = vec3(
		1.0 / float( tex_sizes.x ),
		1.0 / float( tex_sizes.y ),
		1.0 / float( tex_sizes.z )
	);
#endif // PERFORM_GRADIENT_LIGHTING
	
	int iCnt = 0;
	
	// Ray traversal loop
	while( 	   all( greaterThanEqual( curr_sample_pos, vec3( 0.0 ) ) )
			&& all( lessThanEqual( curr_sample_pos, vec3( 1.0 ) ) )
			&& color.a <= u_alpha_saturation_limit
#ifdef PERFORM_DEPTH_TEST
			&& IsSampleNotOccluded( curr_sample_pos )
#endif // PERFORM_DEPTH_TEST
			)
	{
		// Determine the color of the sample through a user define function
		vec4 curr_color = DetermineSampleColor( curr_sample_pos );
		
#ifdef PERFORM_GRADIENT_LIGHTING
		// Add some gradient-based lighting
		vec3 lighted_color = vec3( 0.0 );
		
		// Diffuse
#ifdef USE_ALPHA_CLASSIFIED_GRADIENT
		vec3 normal = normalize( gl_NormalMatrix * ComputeGradient( curr_sample_pos, curr_color.a, offsets ) );
#else
		// @TODO The lookup in the next line is probably redundant to a call in DetermineSampleColor.
		// 		 However, it is probably difficult to find a solution to keep the flexibility of having
		//		 the sample function in a separate file and the ability to provide lighting in general.
		float volume_value = texture( u_gradient_sampler, curr_sample_pos )[u_gradient_channel];
		vec3 normal = normalize( gl_NormalMatrix * ComputeGradient( curr_sample_pos, volume_value, offsets ) );
#endif
		float fLightDot = dot( normal, -gl_LightSource[0].position.xyz );
		fLightDot = clamp( fLightDot, 0.0, 1.0 );
		lighted_color += gl_LightSource[0].diffuse.rgb * curr_color.rgb * fLightDot;
		
		// Ambient
		lighted_color += gl_LightSource[0].ambient.rgb * curr_color.rgb;
		
		// Write back
		curr_color.rgb = lighted_color;
#endif // PERFORM_GRADIENT_LIGHTING

	
		float premultiplied_alpha = u_alpha_compensation * ( 1.0 - color.a ) * curr_color.a;

		color.rgb += premultiplied_alpha * curr_color.rgb;
		color.a += premultiplied_alpha;
	
		curr_sample_pos += ray_dir * u_step_size;
	}
		
	return vec4( color.rgba );
}
	