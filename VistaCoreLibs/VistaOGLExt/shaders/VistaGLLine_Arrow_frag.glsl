#version 330 compatibility
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform float u_fLineRadius = 0.01f;
uniform float u_fConeRadius = 0.02f;
uniform float u_fConeHeight = 0.04f;
uniform int   u_bComputeLighting = 1;

/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
in vec3 var_v3Position;

flat in vec4	var_v4Color1;
flat in vec4	var_v4Color2;
flat in vec3	var_v3Point1;
flat in vec3	var_v3Point2;
flat in vec3	var_3LineDir;
flat in float	var_fLineLength;

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
vec4 ShadeFragment(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor);

/*============================================================================*/
/* GLOBAL VARIABLES	AND FUNKTIONS											  */
/*============================================================================*/
vec3	g_v3RayDir;

struct Intersection
{
	float fDist;
	vec3  v3Point;
	vec3  v3Normal;
	vec4  v4Color;
};

Intersection IntersectDisc( float fOffset, float fRadius )
{
	Intersection I;
	I.v4Color  = mix( var_v4Color1, var_v4Color2, fOffset );
	I.v3Normal = -var_3LineDir;

	vec3 v3Center = mix( var_v3Point1, var_v3Point2, fOffset );

	float fCDotN = dot( v3Center, I.v3Normal );

	I.fDist   = fCDotN / dot(g_v3RayDir, I.v3Normal);
	I.v3Point = I.fDist*g_v3RayDir;

	if( length(I.v3Point - v3Center) > fRadius )
		I.fDist   = -1.0;
	
	return I;
}

Intersection IntersectCylinder()
{
	//		IntersectionPoint = r*g_v3RayDir
	//		f = dot( IntersectionPoint - var_v3Point1, var_3LineDir )
	//		u_fLineRadius = || IntersectionPoint - ( var_v3Point1 + f*var_3LineDir ) ||
	// =>
	//		f = dot( r*g_v3RayDir - var_v3Point1, v3X )
	//		u_fLineRadius = || r*g_v3RayDir - var_v3Point1 - f*var_3LineDir ||
	// =>
	//		f = r*dot( g_v3RayDir, var_3LineDir ) - dot( var_v3Point1, var_3LineDir )
	//		u_fLineRadius = || r*g_v3RayDir - var_v3Point1 - f*var_3LineDir ||
	float fRdotLD = dot( g_v3RayDir, var_3LineDir );
	float fPdotLD = dot( var_v3Point1, var_3LineDir );
	// =>
	//		f = r*fRdotLD - fPdotLD
	//		u_fLineRadius = || r*g_v3RayDir - var_v3Point1 - f*var_3LineDir ||
	// =>	u_fLineRadius = || r*g_v3RayDir - var_v3Point1 - (r*fRdotLD - fPdotLD)*var_3LineDir ||
	// =>	u_fLineRadius = || r*(g_v3RayDir - fRdotLD*var_3LineDir) - var_v3Point1 + fPdotLD*var_3LineDir ||
	vec3 v3V1 = g_v3RayDir   - fRdotLD*var_3LineDir;
	vec3 v3V2 = var_v3Point1 - fPdotLD*var_3LineDir;
	// =>	u_fLineRadius  = || r*v3V1 - v3V2 ||
	// =>	u_fLineRadius² = ( r*v3V1 - v3V2 )²
	// =>	u_fLineRadius² = r²*dot(v3V1,v3V1) - 2*r*dot(v3V1,v3V2) +dot(v3V2,v3V2)
	float a = dot( v3V1, v3V1 );
	float b =-dot( v3V1, v3V2 );
	float c = dot( v3V2, v3V2 ) - u_fLineRadius*u_fLineRadius;
	//solve 0 = a*r² + 2*b*r + c 
	float p = b/a;
	float q = c/a;

	Intersection I;
	I.fDist = -1.0;

	if( p*p - q < 0 ) return I; // No intersection

	I.fDist		= -p - sqrt( p*p -q );
	I.v3Point	= I.fDist*g_v3RayDir;

	float f = ( I.fDist*fRdotLD - fPdotLD )/(var_fLineLength);

	I.v4Color  = mix( var_v4Color1, var_v4Color2, f );
	I.v3Normal = mix( var_v3Point1, var_v3Point2, f );
	I.v3Normal = normalize( I.v3Point  - I.v3Normal );

	float fOffset = (var_fLineLength - u_fConeHeight) / var_fLineLength;
	
	if( !( 0 <= f && f <= fOffset) )
	{ 
		I.fDist = -1.0; 
	}
	
	return I;
}

Intersection IntersectCone()
{
	//	First we need to calculate the intersections between the ray and the truncated cone.
	//	For that we need to solve the following equation system:
	//
	//		IntersectionPoint = r*g_v3RayDir
	//		f = dot( IntersectionPoint - var_v3Point2, -var_3LineDir )
	//		ConeRadius = f*u_fConeRadius/u_fConeHeight
	//		ConeRadius = || (IntersectionPoint - var_v3Point2) - (-f*var_3LineDir) ||
	//
	//		f = dot( r*g_v3RayDir - var_v3Point2, -var_3LineDir  )
	//		f*u_fConeRadius/u_fConeHeight = || r*g_v3RayDir - var_v3Point2 + f*var_3LineDir ||
	// =>
	//		f = r*dot(v3RayDir, -var_3LineDir ) - dot( var_v3Point2, -var_3LineDir ) 
	//		f*u_fConeRadius/u_fConeHeight = || r*g_v3RayDir - var_v3Point2 + f*var_3LineDir ||
	float fSlope = u_fConeRadius / u_fConeHeight;
	float fRayDotLD = dot( g_v3RayDir,   -var_3LineDir );
	float fP2DotLD  = dot( var_v3Point2, -var_3LineDir );
	//		f = r*fRayDotLD - fP2DotLD
	//		f*fSlope = || r*g_v3RayDir - var_v3Point2 - f*var_3LineDir ||
	// =>
	//		(r*fRayDotLD - fP2DotLD)*fSlope = || r*g_v3RayDir - var_v3Point2 + (r*fRayDotLD - fP2DotLD)*var_3LineDir ||
	// =>
	//		r*fRayDotLD*fSlope - fP2DotLD*fSlope = || r*(g_v3RayDir + fRayDotLD*var_3LineDir) - (var_v3Point2 + fP2DotLD*var_3LineDir) ||
	float fF1  = fRayDotLD*fSlope;
	float fF2  = fP2DotLD*fSlope;
	vec3  v3V1 = g_v3RayDir   + fRayDotLD*var_3LineDir;
	vec3  v3V2 = var_v3Point2 + fP2DotLD*var_3LineDir;
	// =>	 r*fF1 - fF2    = || r*v3V1 - v3V2 ||
	// =>	(r*fF1 - fF2)²  =  ( r*v3V1 - v3V2 )²
	// =>	r²*fF1² - 2*r*fF1*fF2 + fF2² = r²*dot(v3V1,v3V1) - 2*r*dot(v3V1, v3V2) + dot(v3V2,v3V2)
	// =>
	//		r²*( fF1² - dot(v3V1,v3V1) ) - 2*r(fF1*fF2 - dot(v3V1, v3V2)) + fF2² - dot(v3V2,v3V2) = 0  
	float a = fF1*fF1 - dot( v3V1, v3V1 );
	float b = fF1*fF2 - dot( v3V1, v3V2 );
	float c = fF2*fF2 - dot( v3V2, v3V2 );
	//solve a*r^2 - 2*b*r + c = 0
	float p =-b/a;
	float q = c/a;
	
	Intersection I;
	I.fDist   = -1.0;

	if( p*p - q < 0 ) return I; // No intersection

	float r1 = -p - sqrt( p*p -q ); // distance to the first  intersection between the ray and the truncated cone
	float r2 = -p + sqrt( p*p -q ); // distance to the second intersection between the ray and the truncated cone
	
	float f1 = ( r1*fRayDotLD - fP2DotLD )/u_fConeHeight;
	float f2 = ( r2*fRayDotLD - fP2DotLD )/u_fConeHeight;

	float f  = u_fConeHeight/var_fLineLength;

	if( 0 <= f1 && f1 <= 1.0 && r1>0 ) // is the first intersection within the truncated cone
	{
		I.fDist = r1;
		I.v4Color = mix( var_v4Color1, var_v4Color2, 1.0f- f1*f );
		I.v3Normal = mix( var_v3Point1, var_v3Point2, 1.0f- f1*f );
	}
	else if( 0 <= f2 && f2 <= 1.0 && r2>0 ) // is the second intersection within the truncated cone
	{
		I.fDist = r2;
		I.v4Color = mix( var_v4Color1, var_v4Color2, 1.0f - f2*f );
		I.v3Normal = mix( var_v3Point1, var_v3Point2, 1.0f - f2*f );
	}
	
	I.v3Point  = I.fDist*g_v3RayDir;
	I.v3Normal = normalize( I.v3Point  - I.v3Normal );
	I.v3Normal = normalize( u_fConeHeight*I.v3Normal + var_3LineDir*u_fConeRadius );

	return I;
};
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	g_v3RayDir = normalize( var_v3Position );

	float fOffset = (var_fLineLength-u_fConeHeight)/var_fLineLength;

	Intersection Intersections[4] = Intersection[4](
		IntersectCylinder( ),
		IntersectCone( ),
		IntersectDisc( 0, u_fLineRadius ),
		IntersectDisc( fOffset, u_fConeRadius )
	);

	
	float fDist = -1;
	vec3  v3Point;
	vec3  v3Normal;
	vec4  v4Color;

	for(int i =0; i < 4; ++i)
	{
		float fR = Intersections[i].fDist;
		if( fDist < 0 || ( fR >= 0 && fR < fDist ) )
		{
			fDist    = fR;
			v3Point  = Intersections[i].v3Point;
			v3Normal = Intersections[i].v3Normal;
			v4Color  = Intersections[i].v4Color;
		}
	}

	if(fDist < 0) discard;

	if (u_bComputeLighting>0)
		gl_FragColor = ShadeFragment(v3Normal, v3Point, v4Color);
	else
		gl_FragColor = v4Color;

	//compute fragmenth depth
	vec4 v4Pos = gl_ProjectionMatrix * vec4( v3Point, 1);
	gl_FragDepth = (v4Pos.z / v4Pos.w + 1.0)* 0.5;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
