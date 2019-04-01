

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENMESH_FOUND )

	vista_find_package_root( OPENMESH include/OpenMesh/Core/Mesh/TriMeshT.hh )

	if( OPENMESH_ROOT_DIR )
		set( OPENMESH_INCLUDE_DIRS "${OPENMESH_ROOT_DIR}/include" )
		set( OPENMESH_LIBRARY_DIRS "${OPENMESH_ROOT_DIR}/lib" )
		
		vista_find_library_uncached_var( _RELEASE_LIB OpenMeshCore PATHS ${OPENMESH_LIBRARY_DIRS} NO_DEFAULT_PATH )
		vista_find_library_uncached_var( _DEBUG_LIB OpenMeshCoreD PATHS ${OPENMESH_LIBRARY_DIRS} NO_DEFAULT_PATH )
		if( _RELEASE_LIB AND _DEBUG_LIB )
			set( OPENMESH_LIBRARIES
				optimized OpenMeshCore
				optimized OpenMeshTools
				debug	  OpenMeshCoreD
				debug	  OpenMeshToolsD
			)
		elseif( UNIX AND _RELEASE_LIB )
			set( OPENMESH_LIBRARIES
				OpenMeshCore
				OpenMeshTools
			)
		elseif( NOT OPENMESH_FIND_QUIET )
			if( NOT _RELEASE_LIB AND NOT _DEBUG_LIB )
				message( WARNING "OpenMesh libraries could not be found in \"${OPENMESH_LIBRARY_DIRS}\"" )
			elseif( NOT _DEBUG_LIB )
				message( WARNING "OpenMesh debug libraries could not be found in \"${OPENMESH_LIBRARY_DIRS}\"" )
			else()
				message( WARNING "OpenMesh release libraries could not be found in \"${OPENMESH_LIBRARY_DIRS}\"" )
			endif()
		endif()
		
	endif( OPENMESH_ROOT_DIR )

endif( NOT VOPENMESH_FOUND )

find_package_handle_standard_args( VOPENMESH "OpenMesh could not be found" OPENMESH_ROOT_DIR )

