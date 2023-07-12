/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2011 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
#version 430
#extension GL_NV_gpu_shader5 : enable
#extension GL_NV_shader_buffer_load : enable
#extension GL_NV_shader_buffer_store : enable
#extension GL_ARB_shader_atomic_counters : enable
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
struct FragmentIterator
{
	int iData[2]; //{ index, UNUSED }
};

void DeleteFragments( ivec2 v2Coords );

FragmentIterator InsertFragment( ivec2 v2Coords );
FragmentIterator GetLastFragment( ivec2 v2Coords );
FragmentIterator GetPreviousFragment( FragmentIterator it );

bool IsIteratorValid( FragmentIterator it );

/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform uint g_nWidth;
uniform uint g_nHeight;

uniform uint g_nMaxNumFragments;

coherent uniform int*	g_pHeadPointerBuffer;
coherent uniform int*	g_pNextPointerBuffer;

layout(binding = 0, offset = 0) uniform atomic_uint g_acFragmentCounter;
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void DeleteFragments( ivec2 v2Coords )
{
	if(	v2Coords.x >= 0			&& 
		v2Coords.y >= 0			&&
		v2Coords.x < g_nWidth	&&
		v2Coords.y < g_nHeight	)
	{
		uint idx = v2Coords.x + v2Coords.y*g_nWidth;

        g_pHeadPointerBuffer[idx] = -1;
	}
}

/******************************************************************************/
FragmentIterator InsertFragment( ivec2 v2Coords )
{
	FragmentIterator it;
    it.iData[0] = -1;
	
	if(	v2Coords.x >= 0			&& 
		v2Coords.y >= 0			&&
		v2Coords.x < g_nWidth	&&
		v2Coords.y < g_nHeight	)
	{
		uint idx = v2Coords.x + v2Coords.y*g_nWidth;
		//Increment Fragment count
		int iNewFragment = int( atomicCounterIncrement(g_acFragmentCounter) );

		if( iNewFragment < g_nMaxNumFragments )
		{
			//Exchange HeadPointer
			
			//int iNext = atomicExchange( g_pHeadPointerBuffer + idx;, iNewFragment );
			// the line above is not working because of a bug the current NVideia driver.
			// therefore we use the following workaround 
			coherent int* pHead = g_pHeadPointerBuffer + idx;
			int iNext = atomicExchange( pHead, iNewFragment );
			
			// Set NextPointer
			g_pNextPointerBuffer[ iNewFragment ] = iNext;
            
            it.iData[0] = iNewFragment;
		}
	}

	return it;
}

/******************************************************************************/
FragmentIterator GetLastFragment( ivec2 v2Coords )
{
	FragmentIterator it;
    it.iData[0] = -1;

	if(	v2Coords.x >= 0			&& 
		v2Coords.y >= 0			&&
		v2Coords.x < g_nWidth	&&
		v2Coords.y < g_nHeight	)
	{
		uint idx = v2Coords.x + v2Coords.y*g_nWidth;

		it.iData[0] = g_pHeadPointerBuffer[ idx ];
	}

	return it;
}

/******************************************************************************/
FragmentIterator GetPreviousFragment( FragmentIterator it )
{
	FragmentIterator itNext;
    itNext.iData[0] = -1;

	if(	it.iData[0] >= 0 && it.iData[0] <  g_nMaxNumFragments )
    {
		itNext.iData[0] = g_pNextPointerBuffer[ it.iData[0] ];
    }

	return itNext;
}

/******************************************************************************/
bool IsIteratorValid( FragmentIterator it )
{
	return( it.iData[0] >= 0 && it.iData[0] <  g_nMaxNumFragments );
}

/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
