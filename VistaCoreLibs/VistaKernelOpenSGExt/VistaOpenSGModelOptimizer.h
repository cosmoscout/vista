/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
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


#ifndef _VISTAOPENSGMODELOPTIMIZER_H_
#define _VISTAOPENSGMODELOPTIMIZER_H_

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaKernelOpenSGExtConfig.h"

#include <VistaBase/VistaBaseTypes.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaNode;
class VistaSceneGraph;
class IVistaClusterBarrier;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELOPENSGEXTAPI VistaOpenSGModelOptimizer
{
public:
	enum
	{
		OPT_MERGE_MATERIALS					= 1<<0,
		OPT_STRIPE_GEOMETRIES				= 1<<1,
		OPT_MERGE_GEOMETRIES				= 1<<2,
		OPT_CONVERT_GEOMS_SINGLE_INDEXED	= 1<<3,
		OPT_USE_VBO							= 1<<4,
		OPT_CALCULATE_NORMALS_30DEG			= 1<<5,
		OPT_CALCULATE_NORMALS_45DEG			= 1<<6,
		OPT_CALCULATE_NORMALS_60DEG			= 1<<7,
		OPT_CALCULATE_NORMALS_90DEG			= 1<<8,
		OPT_CALCULATE_NORMALS_120DEG		= 1<<9,
		OPT_CALCULATE_NORMALS_180DEG		= 1<<10,
		OPT_CALCULATE_TANGENT_SPACE			= 1<<11,

		// preset combinations
		OPT_NONE				= 0,
		OPT_DEFAULT				= OPT_MERGE_MATERIALS | OPT_MERGE_GEOMETRIES,
		OPT_ALL					= ~0
	};

	static bool OptimizeNode( IVistaNode* pNode,
						int nOptimizationMode = OPT_DEFAULT,
						bool bVerbose = true );
	static bool OptimizeAndSaveNode( IVistaNode* pNode,
						const std::string& sOutputFilename,
						int nOptimizationMode = OPT_DEFAULT,
						bool bVerbose = true );
	static bool OptimizeFile( const std::string& sFilename,
						const std::string& sOutputFilename = "",
						int nOptimizationMode = OPT_DEFAULT,
						bool bVerbose = true  );
	struct VISTAKERNELOPENSGEXTAPI LoadInfo
	{
		LoadInfo();
		bool m_bCacheWasLoaded;
		std::string m_sFullFilename;
		std::string m_sCacheFilename;
		VistaType::systemtime m_nFileTimestamp;
		VistaType::systemtime m_nCacheTimestamp;
	};
	/**
	 * Loads a model file, applies the desired optimizations, and writes it
	 * in a dumped format.
	 * If bCompareTimestamps is true, the cache will only be loaded if it is newer than the model file
	 * If bAllowLoadingCachedFileWithoutOriginal is true, the cache file is loaded even if the
	 *     original model is absent.
	 * If ClusterBarrier is not NULL, only the cluster leader will optimize the model, while all other
	 * nodes will wait with loading until the leader has finished writing the ptimized file
	 * If pInfo is non-NULL and the call is successfull, it will hold info about the load
	 */
	static IVistaNode* LoadAutoOptimizedFile( VistaSceneGraph* pSceneGraph,
						const std::string& sFilename,
						int nOptimizationMode = OPT_DEFAULT,
						const std::string& sDumpDataFormat = "osb",
						bool bCompareTimestamps = true,
						bool bVerbose = true,
						bool bAllowLoadingCachedFileWithoutOriginal = false,
						IVistaClusterBarrier* pClusterBarrier = NULL,
						LoadInfo* pInfo = NULL );
	static IVistaNode* LoadAutoOptimizedFile( VistaSceneGraph* pSceneGraph,
						const std::string& sFilename,
						const std::string& sOutputDirectory,
						int nOptimizationMode = OPT_DEFAULT,
						const std::string& sDumpDataFormat = "osb",
						bool bCompareTimestamps = true,
						bool bVerbose = true,
						bool bAllowLoadingCachedFileWithoutOriginal = false,
						IVistaClusterBarrier* pClusterBarrier = NULL,
						LoadInfo* pInfo = NULL );
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGMODELOPTIMIZER_H_
