include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENVR_FOUND )
	vista_find_package_root( OPENVR headers/openvr.h )

	if( OPENVR_ROOT_DIR )
	
		set( OPENVR_INCLUDE_DIRS "${OPENVR_ROOT_DIR}/headers" )
		if( WIN32 )
			if( VISTA_64BIT )
				set( OPENVR_LIBRARY_DIRS "${OPENVR_ROOT_DIR}/lib/win64" )
				set( OPENVR_LIBRARIES openvr_api.lib )
			else()
				set( OPENVR_LIBRARY_DIRS "${OPENVR_ROOT_DIR}/lib/win32" )
				set( OPENVR_LIBRARIES openvr_api.lib )
			endif()
		else()
			if( VISTA_64BIT )
				set( OPENVR_LIBRARY_DIRS "${OPENVR_ROOT_DIR}/lib/linux64" )
				set( OPENVR_LIBRARIES libopenvr_api.so )
			endif()
		endif()
	endif()
endif()

find_package_handle_standard_args( VOPENVR "OPENVR could not be found" OPENVR_ROOT_DIR OPENVR_LIBRARIES )

