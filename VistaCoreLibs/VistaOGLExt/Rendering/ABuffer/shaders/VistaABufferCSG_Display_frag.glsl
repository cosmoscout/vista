#version 400
#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable
#extension GL_NV_shader_buffer_load : enable
#extension GL_EXT_bindable_uniform : enable
#extension GL_NV_shader_buffer_store : enable
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
struct FragmentIterator
{ int iData[2]; };

FragmentIterator GetLastFragment( ivec2 v2Coords );
FragmentIterator GetPreviousFragment( struct FragmentIterator it );

bool IsIteratorValid( FragmentIterator it );

int		ReadFragments();
void	InsertionSort( int nNumFrag );
vec4	ResolveAlphaBlend( uint nNumFrag );

void OnEnterObject( int iObject );
void OnExitObject( int iObject );
bool RenderObject( int iObject );
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
coherent uniform uint*	u_pFragColor;
coherent uniform float*	u_pFragDepth;
coherent uniform int*	u_pObjectID;

uniform ivec4 u_v4Viewport;

uniform sampler2DRect u_texOpaqueDepth;
/*============================================================================*/
/* GLOBAL VARIABLES														      */
/*============================================================================*/
#define MAX_NUM_FRAGMETS 32

uint  g_iColors[MAX_NUM_FRAGMETS];
int   g_iObjects[MAX_NUM_FRAGMETS];
float g_fDepths[MAX_NUM_FRAGMETS];
/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	int iNumFragments = ReadFragments();

	InsertionSort( iNumFragments );

	gl_FragColor = ResolveAlphaBlend( iNumFragments );
}

int	ReadFragments()
{
	ivec2 v2Coords = ivec2( gl_FragCoord.xy );
	v2Coords -= u_v4Viewport.xy;

	FragmentIterator it = GetLastFragment( v2Coords );
	if( !IsIteratorValid(it) ) discard;

	float fOpaqueDepth = texture(u_texOpaqueDepth, gl_FragCoord.xy).r;

	int iNumFragments = 0;
	while( iNumFragments<MAX_NUM_FRAGMETS && IsIteratorValid(it) )
    {
		float fDepth = u_pFragDepth[ it.iData[0] ];
		if( fDepth < fOpaqueDepth )
		{
			g_fDepths[  iNumFragments ] = fDepth;
			g_iColors[  iNumFragments ] = u_pFragColor[ it.iData[0] ];
			g_iObjects[ iNumFragments ] = u_pObjectID[  it.iData[0] ];

			++iNumFragments;
		}

        it = GetPreviousFragment( it );
	}

	return iNumFragments;
}

void InsertionSort( int nNumFrag )
{
	for (uint i = 1; i < nNumFrag; ++i)
	{
		uint j = i;
		uint iColor  = g_iColors[i];
		int  iObject = g_iObjects[i];
		float fDepth = g_fDepths[i];
		while( j>0 )
		{
			if( g_fDepths[j-1] > fDepth )
				break;
				
			g_iColors[j]  = g_iColors[j-1];
			g_iObjects[j] = g_iObjects[j-1];
			g_fDepths[j]  = g_fDepths[j-1];
			--j;
		}
		g_iColors[j]  = iColor;
		g_iObjects[j] = iObject;
		g_fDepths[j]  = fDepth;
	}
}

vec4 ResolveAlphaBlend( uint nNumFrag )
{
	vec4 v4FinalColor = vec4(0);

	for( int i=0; i<nNumFrag; ++i )
	{
		int iAbs = abs(g_iObjects[i]);
		int iSgn = sign(g_iObjects[i]);
		if(iSgn>0)
			OnEnterObject(iAbs);
		
		
		vec4 v4Color = unpackUnorm4x8( g_iColors[ i ] );
		
		if(RenderObject(iAbs))
		{
			v4FinalColor *= (1-v4Color.a);
			v4Color.rgb  *=  v4Color.a;
			v4FinalColor +=  v4Color;
			//v4Color          *= (1-v4FinalColor.a);
			//v4FinalColor.rgb *= v4FinalColor.a;
			//v4FinalColor     += v4Color;
		}
		
		if(iSgn<0)
			OnExitObject(iAbs);
	}

	return v4FinalColor;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/