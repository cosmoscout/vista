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
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
struct FragmentIterator
{
	int iData[2]; //{ Index, array offset }
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

uniform uint g_nFragmentsPerPixel;

coherent uniform uint*	g_pFragCountBuffer;

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

        g_pFragCountBuffer[idx] = 0;
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
		//Increment Fragment count
		
        //uint nFragmentID = atomicAdd( g_pFragCountBuffer+idx, 1U );
		// the line above is not working because of a bug the current NVideia driver.
		// therefore we use the following workaround 
		coherent uint* pFragCount = g_pFragCountBuffer + idx;
		uint nFragmentID = atomicAdd( pFragCount, 1U );
		
		if( nFragmentID < g_nFragmentsPerPixel )
		{
            it.iData[1] = int( idx*g_nFragmentsPerPixel );
            it.iData[0] = it.iData[1] + int( nFragmentID );
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

	if(	v2Coords.x >= 0			&& 
		v2Coords.y >= 0			&&
		v2Coords.x < g_nWidth	&&
		v2Coords.y < g_nHeight	)
	{
		uint idx = v2Coords.x + v2Coords.y*g_nWidth;
        
		int iNumFragmetns = min(
			int( g_pFragCountBuffer[ idx ] ),
			int( g_nFragmentsPerPixel ) );
		
        it.iData[1] = int( idx*g_nFragmentsPerPixel  );
        it.iData[0] = it.iData[1] + iNumFragmetns - 1;
	}

	return it;
}

/******************************************************************************/
FragmentIterator GetPreviousFragment( FragmentIterator it )
{
	FragmentIterator itNext = it;

    itNext.iData[0] -= 1;

	return itNext;
}

/******************************************************************************/
bool IsIteratorValid( FragmentIterator it )
{	
	return( it.iData[0] >= it.iData[1] );
}

/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
