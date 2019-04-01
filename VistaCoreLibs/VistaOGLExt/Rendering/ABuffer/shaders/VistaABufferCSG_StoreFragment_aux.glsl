#version 400
#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable
#extension GL_NV_shader_buffer_load : enable
#extension GL_EXT_bindable_uniform : enable
#extension GL_NV_shader_buffer_store : enable
/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
layout(pixel_center_integer) in vec4 gl_FragCoord;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
coherent uniform uint*	u_pFragColor;
coherent uniform float*	u_pFragDepth;
coherent uniform int*	u_pObjectID;

uniform ivec4 u_v4Viewport;
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
struct FragmentIterator { int iData[2]; };

FragmentIterator InsertFragment( ivec2 v2Coords );

bool IsIteratorValid( FragmentIterator it );

/*============================================================================*/
/* RenderFragment                                                             */
/*============================================================================*/
void StoreFragment( ivec2 v2Coords, vec4 v4Color, float fDepth, int iObjectID )
{
	if( gl_SampleID != 0 ) return;

	v2Coords -= u_v4Viewport.xy;

	if(	v2Coords.x >= 0				&& 
		v2Coords.y >= 0				&&
		v2Coords.x < u_v4Viewport.z	&&
		v2Coords.y < u_v4Viewport.w	)
	{
		FragmentIterator it = InsertFragment( v2Coords );

		if( IsIteratorValid(it) )
		{
			uint iColor = packUnorm4x8( v4Color );
			u_pFragColor[ it.iData[0] ] = iColor;
			u_pFragDepth[ it.iData[0] ] = fDepth;
			u_pObjectID[  it.iData[0] ] = iObjectID*( gl_FrontFacing ? -1 : 1 );
		}
	}
}

void StoreFragment( vec4 v4Color,  int iObjectID )
{
	StoreFragment( ivec2( gl_FragCoord.xy ), v4Color, gl_FragCoord.z, iObjectID );
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/