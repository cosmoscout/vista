

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT V3DCSDK_FOUND )
	vista_find_package_root( 3DCSDK Inc/si.h SUBDIRS "3Dconnexion/3DxWare SDK" "3Dconnexion" "3DxWare SDK" "" )

	if( 3DCSDK_ROOT_DIR )
		unset ( V3DCSDK_FOUND CACHE )
	if( VISTA_64BIT )
			vista_find_library_uncached(
				NAMES siapp siappD
				PATH_SUFFIXES Lib/x64
				PATHS
				${3DCSDK_ROOT_DIR}
			)
		else()
			vista_find_library_uncached(
				NAMES siapp siappD
				PATH_SUFFIXES Lib/x86
				PATHS
				${3DCSDK_ROOT_DIR}
			)
		endif()
		
		if( VISTA_UNCACHED_LIBRARY )
			message( STATUS "3DCsdk libraries: ${VISTA_UNCACHED_LIBRARY}" )
			
			set( 3DCSDK_LIBRARIES ${VISTA_UNCACHED_LIBRARY} )
		endif()
		
		set( 3DCSDK_LIBRARY_DIRS "${3DCSDK_ROOT_DIR}/Lib" )
		set( 3DCSDK_INCLUDE_DIRS "${3DCSDK_ROOT_DIR}/Inc" )
		
	endif( 3DCSDK_ROOT_DIR )
	

endif( NOT V3DCSDK_FOUND )

find_package_handle_standard_args( V3DCSDK "3DCSDK could not be found" 3DCSDK_ROOT_DIR )
