include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENVR_FOUND )
	vista_find_package_root( OPENVR include/openvr/openvr.h )

	if( OPENVR_ROOT_DIR )
	
		set( OPENVR_INCLUDE_DIRS "${OPENVR_ROOT_DIR}/include" )
		message(STATUS "OPENVR_INCLUDE_DIRS: ${OPENVR_INCLUDE_DIRS}")

		if( WIN32 )
			set( OPENVR_LIBRARY_DIRS "${OPENVR_ROOT_DIR}/lib" )
			set( OPENVR_LIBRARIES openvr_api.lib )
		else()
			set( OPENVR_LIBRARY_DIRS "${OPENVR_ROOT_DIR}/lib" )
			set( OPENVR_LIBRARIES libopenvr_api.so )
		endif()
	endif()
endif()

find_package_handle_standard_args( VOPENVR "OPENVR could not be found" OPENVR_ROOT_DIR OPENVR_LIBRARIES )

