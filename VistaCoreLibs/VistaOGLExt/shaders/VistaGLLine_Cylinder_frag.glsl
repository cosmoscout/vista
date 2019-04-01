#version 330 compatibility
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform float u_fLineRadius;
uniform int   u_bComputeLighting = 1;

/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
in vec3 var_v3Position;
flat in vec3 var_v3Point1;
flat in vec3 var_v3Point2;
flat in vec4 var_v4Color1;
flat in vec4 var_v4Color2;

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
vec4 ShadeFragment(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor);

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	vec3 v3RayDir = normalize(var_v3Position);

	vec3  v3LineDir   = normalize( var_v3Point2 - var_v3Point1 );
	float fLineLength = length( var_v3Point2 - var_v3Point1 );
	//
	//	First we need to calculate the intersections between the ray and the cylinder.
	//	For that we need to solve the following equation system:
	//
	//		IntersectionPoint = r*v3RayDir
	//		f = dot(IntersectionPoint-var_Point1 , v3LineDir)
	//		u_fLineRadius = || IntersectionPoint - (f*v3LineDir + var_Point1) ||
	//
	//	The variable "f" indicates the distance between the intersection point
	//	and the plane, which extends orthogonally to the line through the 
	//	starting point of the line. The value of "f" must be between 0 and fLineLength.
	//
	//	=>
	//		f == dot(r * v3RayDir-var_Point1 , v3LineDir)
	//		u_fLineRadius == || r*v3RayDir - (f*v3LineDir + var_Point1) ||
	//
	//	=>
	//		f == r*dot( v3RayDir, v3LineDir ) - dot( var_Point1, v3LineDir )
	//		u_fLineRadius == || r*v3RayDir - (f*v3LineDir + var_Point1) ||
	//
	float fRayDotLine = dot( v3RayDir, v3LineDir );
	float fP1DotLine  = dot( var_v3Point1,   v3LineDir );
	//	=>
	//		f == r*fRayDotLine - fP1DotLine
	//		u_fLineRadius == || r*v3RayDir - (f*v3LineDir + var_Point1) ||
	//	=>
	//		u_fLineRadius == || r*v3RayDir - ((r*fRayDotLine - fP1DotLine)*v3LineDir + var_Point1 ) ||
	//	<=>
	//		u_fLineRadius == || r*(v3RayDir - fRayDotLine*v3LineDir) + fP1DotLine*v3LineDir - var_Point1) ||
	//
	vec3  v3V1 = v3RayDir - fRayDotLine*v3LineDir;
	vec3  v3V2 = fP1DotLine*v3LineDir    - var_v3Point1;
	//  => u_fLineRadius   == || r*v3V1 + v3V2 ||
	// <=> u_fLineRadius^2 == ( r*v3V1 + v3V2 )^2
	// <=> u_fLineRadius^2 == r^2*dot(v3V1,v3V1) + 2*r*dot(v3V1,v3V2) + dot(v3V1,v3V2)
	// <=> r^2*dot(v3V1,v3V1)  +  2*r*dot(v3V1,v3V2)  +  dot(v3V1,v3V2) - u_fLineRadius^2 == 0
	float a = dot( v3V1, v3V1 );
	float b = dot( v3V1, v3V2 );
	float c = dot( v3V2, v3V2 ) - u_fLineRadius*u_fLineRadius;
	//  => a*r^2 + 2*b*r + c = 0
	float p = b/a;
	float q = c/a;
	
	if( p*p - q < 0) discard;

	float r = -p - sqrt( p*p -q ); // distance to the first intersection between the ray and the cylinder
	float f = r*fRayDotLine - fP1DotLine;

	if(f < 0 || fLineLength < f ) r = -1; 

	// Next, we test whether one of the spheres, which are located at the end/beginning of the line, is hit by the Ray.
	float fP1DotRay = dot( var_v3Point1, v3RayDir );
	float fP2DotRay = dot( var_v3Point2, v3RayDir );
	float fDist1 = length(fP1DotRay*v3RayDir - var_v3Point1)/u_fLineRadius; // Distance between the ray and the start point of the line
	float fDist2 = length(fP2DotRay*v3RayDir - var_v3Point2)/u_fLineRadius; // Distance between the ray and the end point of the line
	
	if(fDist1 <= 1.0) // does the ray hits the sphere at the beginning of the line
	{
		float fR = fP1DotRay - u_fLineRadius*sqrt(1.0 - fDist1*fDist1);
		if(fR < r || r < 0) // is the sphere hit before the cylinder
		{
			f = 0.0f; 
			r = fR;
		}
	};
	if(fDist2 <= 1.0) // does the ray hits the sphere at the end of the line
	{
		float fR = fP2DotRay - u_fLineRadius*sqrt(1.0 - fDist2*fDist2);
		if(fR < r || r < 0) // is the sphere hit before the cylinder
		{
			f = fLineLength; 
			r = fR;
		}
	};

	if( 0 <= f &&  f <= fLineLength && 0 <= r )
	{
		vec4 v4Color             = mix( var_v4Color1, var_v4Color2, f/fLineLength );
		vec4 v4IntersectionPoint = vec4( r*v3RayDir, 1.0 );
		vec3 v3Normal            = normalize( v4IntersectionPoint.xyz - (var_v3Point1 + f*v3LineDir) );
	
		//compute fragmenth color
		if (u_bComputeLighting>0)
			gl_FragColor = ShadeFragment(v3Normal, v4IntersectionPoint.xyz, v4Color);
		else
			gl_FragColor = v4Color;
	
		//compute fragmenth depth
		v4IntersectionPoint = gl_ProjectionMatrix * v4IntersectionPoint;
		gl_FragDepth = ( v4IntersectionPoint.z / v4IntersectionPoint.w + 1.0 )* 0.5;
	}
	else
		discard;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
