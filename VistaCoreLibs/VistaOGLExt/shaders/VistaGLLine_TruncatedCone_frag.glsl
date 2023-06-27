#version 330 compatibility
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
in vec3 var_v3Position;

flat in vec3  var_v3Point1;
flat in vec3  var_v3Point2;
flat in vec4  var_v4Color1;
flat in vec4  var_v4Color2;
flat in float var_fRadius1;
flat in float var_fRadius2;

uniform int   u_bComputeLighting = 1;
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
vec4 ShadeFragment(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor);

/*============================================================================*/
/* GLOBAL VARIABLES	AND FUNKTIONS											  */
/*============================================================================*/
struct Intersection
{
	float fDist;
	vec3  v3Point;
	vec3  v3Normal;
	vec4  v4Color;
};

Intersection IntersectSphere( vec3 v3RayDir, vec3 v3Pos, float fRadius, vec4 v4Color )
{
	float fPosDotRay = dot( v3Pos, v3RayDir );

	// Shortest Distance between the ray and the center of the Sphere
	vec3 v3ShortestDistance = fPosDotRay*v3RayDir - v3Pos;

	float fPenetrationDepth = length(v3ShortestDistance)/fRadius;
 
	Intersection I;
	I.fDist   = -1.0;
	I.v4Color = v4Color;

	if( fPenetrationDepth <= 1.0 ) // does the ray hits the sphere
	{
		float fR = fPosDotRay - fRadius*sqrt(1.0 - fPenetrationDepth*fPenetrationDepth);
		
		I.fDist    = fR;
		I.v3Point  = fR*v3RayDir;
		I.v3Normal = normalize( I.v3Point - v3Pos );
	};

	return I;
};

Intersection IntersectCone( vec3 v3RayDir )
{
	vec3  v3X     = normalize( var_v3Point2 - var_v3Point1 );
	float fLength =    length( var_v3Point2 - var_v3Point1 );

	float fAngle = atan( ( var_fRadius2 - var_fRadius1 ) / fLength );

	float fRadius1 = cos(fAngle)*var_fRadius1;
	float fRadius2 = cos(fAngle)*var_fRadius2;

	float fOffset1 = sin(fAngle)*var_fRadius1;
	float fOffset2 = sin(fAngle)*var_fRadius2;

	vec3 v3Point1 = var_v3Point1 - fOffset1*v3X;
	vec3 v3Point2 = var_v3Point2 - fOffset2*v3X;

	fLength = length( v3Point2 - v3Point1 );

	//	First we need to calculate the intersections between the ray and the truncated cone.
	//	For that we need to solve the following equation system:
	//
	//		IntersectionPoint = r*v3RayDirection
	//		f = dot( IntersectionPoint - v3Point1 , v3X )
	//		ConeRadius = fRadius1 + f*(fRadius2 - fRadius1)/fLength
	//		ConeRadius = || IntersectionPoint - (f*v3X + v3Point1) ||
	//
	//	The variable "f" indicates the distance between the intersection point
	//	and the plane, which extends orthogonally to the line through the 
	//	starting point of the line. The value of "f" must be between 0 and fLength.
	//
	//	=>
	//		f == dot(r * v3RayDirection-v3Point1 , v3X)
	//		fRadius1 + f*(fRadius2 - fRadius1)/fLength == || r*v3RayDirection - (f*v3X + v3Point1) ||
	//
	//	=>
	//		f == r*dot(v3RayDirection,v3X) - dot(v3Point1,v3X)
	//		fRadius1 + f/fLength*(fRadius2 - fRadius1) == || r*v3RayDirection - (f*v3X + v3Point1) ||
	//
	float fSlope   = ( fRadius2 - fRadius1 ) / fLength;
	float fRayDotX = dot( v3RayDir, v3X );
	float fP1DotX  = dot( v3Point1, v3X );
	//	=>
	//		f == r*fRayDotX - fP1DotX
	//		fRadius1 + f*fSlope == || r*v3RayDirection - (f*v3X + v3Point1) ||
	//	=>
	//		fRadius1 + (r*fRayDotX - fP1DotX)*fSlope  == || r*v3RayDirection - ((r*fRayDotX - fP1DotX)*v3X + v3Point1) ||
	//	<=>
	//		r*fRayDotX*fSlope + fRadius1 - fP1DotX*fSlope == || r*(v3RayDirection - fRayDotX*v3X) + fP1DotX*v3X - v3Point1) ||
	float fF1  = fRayDotX*fSlope;
	float fF2  = fRadius1    - fP1DotX*fSlope;
	vec3  v3V1 = v3RayDir    - fRayDotX*v3X;
	vec3  v3V2 = fP1DotX*v3X - v3Point1;
	//  => r*fF1 + fF2 == || r*v3V1 + v3V2 ||
	// <=> (r*fF1 + fF2)^2 == ( r*v3V1 + v3V2 )^2
	// <=> r^2*fF1^2 + 2*r*fF1*fF2 + fF2^2 == r^2*dot(v3V1,v3V1) + 2*r*dot(v3V1,v3V2) + dot(v3V1,v3V2)
	// <=> r^2*(fF1^2 - dot(v3V1,v3V1))  +  2*r*(fF1*fF2 - dot(v3V1,v3V2))  +  (fF2^2 - dot(v3V1,v3V2)) == 0
	float a = fF1*fF1 - dot( v3V1, v3V1 );
	float b = fF1*fF2 - dot( v3V1, v3V2 );
	float c = fF2*fF2 - dot( v3V2, v3V2 );

	//solve a*r^2 + 2*b*r + c = 0
	float p = b/a;
	float q = c/a;
	
	Intersection I;
	I.fDist   = -1.0;

	if( p*p - q < 0 ) return I; // No intersection

	float r1 = -p - sqrt( p*p -q ); // distance to the first  intersection between the ray and the truncated cone
	float r2 = -p + sqrt( p*p -q ); // distance to the second intersection between the ray and the truncated cone
	
	float f1 = ( r1*fRayDotX - fP1DotX )/fLength;
	float f2 = ( r2*fRayDotX - fP1DotX )/fLength;

	if( 0 <= f1 && f1 <= 1.0 && r1>0 ) // is the first intersection within the truncated cone
	{
		I.fDist = r1;
		I.v3Point  = I.fDist*v3RayDir;
		I.v4Color = mix( var_v4Color1, var_v4Color2, f1 );
		I.v3Normal = mix( v3Point1, v3Point2, f1 );
	}
	else if( 0 <= f2 && f2 <= 1.0 && r2>0 ) // is the second intersection within the truncated cone
	{
		I.fDist = r2;
		I.v3Point  = I.fDist*v3RayDir;
		I.v4Color = mix( var_v4Color1, var_v4Color2, f2 );
		I.v3Normal = mix( v3Point1, v3Point2, f2 );
	}
	
	I.v3Normal = normalize( I.v3Point  - I.v3Normal );
	I.v3Normal = normalize( I.v3Normal - fSlope*v3X );

	return I;
};
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	vec3 v3RayDirection = normalize( var_v3Position );

	
	Intersection Intersections[3] = Intersection[3](
		IntersectSphere( v3RayDirection, var_v3Point1, var_fRadius1, var_v4Color1 ),
		IntersectSphere( v3RayDirection, var_v3Point2, var_fRadius2, var_v4Color2 ),
		IntersectCone( v3RayDirection )
	);

	
	float fDist = -1;
	vec3  v3Point;
	vec3  v3Normal;
	vec4  v4Color;

	for(int i =0; i < 3; ++i)
	{
		float fR = Intersections[i].fDist;
		if( fDist < 0 || ( fR >= 0 && fR < fDist ) )
		{
			fDist   = fR;
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
