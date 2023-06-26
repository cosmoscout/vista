

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VWIIUSE_FOUND )
	vista_find_package_root( wiiuse include/wiiuse.h )
	vista_find_package_root( wiiuse wiiuse.h )
	
	if( WIIUSE_ROOT_DIR )
		set( WIIUSE_LIBRARY_DIRS )
		vista_find_library_dir( WIIUSE_LIBRARY_DIRS wiiuse "${WIIUSE_ROOT_DIR}/lib" )
		if( WIIUSE_LIBRARY_DIRS )
			if( EXISTS "${WIIUSE_ROOT_DIR}/include/wiiuse.h" )
				set( WIIUSE_INCLUDE_DIRS "${WIIUSE_ROOT_DIR}/include" )
			else()
				set( WIIUSE_INCLUDE_DIRS "${WIIUSE_ROOT_DIR}" )
			endif()			
			set( WIIUSE_LIBRARIES wiiuse )
		else()
			set( WIIUSE_ROOT_DIR WIIUSE_ROOT_DIR-NOTFOUND )
		endif()		
	endif()

endif( NOT VWIIUSE_FOUND )

find_package_handle_standard_args( Vwiiuse "wiiuse could not be found" WIIUSE_LIBRARY_DIRS )



