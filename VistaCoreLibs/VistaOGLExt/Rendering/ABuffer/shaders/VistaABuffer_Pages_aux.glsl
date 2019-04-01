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
#version 400
#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable
#extension GL_NV_shader_buffer_load : enable
#extension GL_EXT_bindable_uniform : enable
#extension GL_NV_shader_buffer_store : enable
#extension GL_ARB_shader_atomic_counters : enable
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
struct FragmentIterator
{
	int iData[2]; //{ fragment index, page index }
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

uniform uint g_nPageSize;
uniform uint g_nNumPages;

coherent uniform int*	g_pPagePointerBuffer;
coherent uniform int*	g_pNextPointerBuffer;
coherent uniform uint*	g_pSemaphoreBuffer;
coherent uniform uint*	g_pFragCountBuffer;

layout(binding = 0, offset = 0) uniform atomic_uint ac;
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
        
		g_pPagePointerBuffer[idx] = -1;
        g_pFragCountBuffer[idx]   = 0;
	}
}

/******************************************************************************/
FragmentIterator InsertFragment( ivec2 v2Coords )
{
	FragmentIterator it;
    it.iData[0] = -1;
    it.iData[1] =  0;
	
	if(	v2Coords.x >= 0			&& 
		v2Coords.y >= 0			&&
		v2Coords.x < g_nWidth	&&
		v2Coords.y < g_nHeight	)
	{
		uint idx = v2Coords.x + v2Coords.y*g_nWidth;
        
		int  iCurPage       = -1;
		uint nCurFragIdx    =  0;
		
        ///Manual critical section => This can most probably be optimized !
		int ii=0; //prevents infinite loops
		bool leaveLoop = false;
        while( !leaveLoop && ii<100 ) 
        {
			//Aquire semaphore
			if( atomicExchange( g_pSemaphoreBuffer + idx, 1U ) == 0U )
            {
                nCurFragIdx = g_pFragCountBuffer[idx]++;

                if( nCurFragIdx%g_nPageSize != 0 )
                {
                    // Use existing Page
                    iCurPage =  g_pPagePointerBuffer[ idx ];
                }
                else 
                {
                    // Create new Page
                    int iNewPage = int( atomicCounterIncrement(ac) );

                    if( iNewPage < g_nNumPages)
                    {
			            //Exchange HeadPointer
			            int iNext = atomicExchange( g_pPagePointerBuffer + idx, iNewPage );
                        // Set NextPointer
                        g_pNextPointerBuffer[iNewPage] = iNext;
                        iCurPage = iNewPage;
                    }
                }
                
                atomicExchange( g_pSemaphoreBuffer + idx, 0U );
                leaveLoop = true;
            }
            ++ii;
        }

        if( iCurPage >= 0 )
        {
            it.iData[0] = int( iCurPage*g_nPageSize + nCurFragIdx%g_nPageSize );
            it.iData[1] = iCurPage;
        }
	}

	return it;
}

/******************************************************************************/
FragmentIterator GetLastFragment( ivec2 v2Coords )
{
	FragmentIterator it;
    it.iData[0] = -1;
    it.iData[1] =  0;

//	if(	v2Coords.x >= 0			&& 
//		v2Coords.y >= 0			&&
//		v2Coords.x < g_nWidth	&&
//		v2Coords.y < g_nHeight	)
	{
		uint idx = v2Coords.x + v2Coords.y*g_nWidth;

		int  iPage       = g_pPagePointerBuffer[ idx ];
		uint nNumFrags   = g_pFragCountBuffer[ idx ] - 1;

        it.iData[0] = int( (iPage*g_nPageSize) + (nNumFrags%g_nPageSize) );
        it.iData[1] = iPage;
	}

	return it;
}

/******************************************************************************/
FragmentIterator GetPreviousFragment( FragmentIterator it )
{
	FragmentIterator itNext = it;

	itNext.iData[0] -= 1;

    if( it.iData[0] == it.iData[1]*g_nPageSize && 
		it.iData[1] >= 0 && it.iData[1] < g_nNumPages )
    {
		int  iNext      = g_pNextPointerBuffer[ it.iData[1] ];
		itNext.iData[1] = iNext;
        itNext.iData[0] = int( iNext*g_nPageSize + g_nPageSize-1 );
    }

	return itNext;
}

/******************************************************************************/
bool IsIteratorValid( FragmentIterator it )
{
	return( it.iData[0] >= 0  && it.iData[1] >= 0 &&
			it.iData[0] >= it.iData[1]*g_nPageSize  );
}

/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
