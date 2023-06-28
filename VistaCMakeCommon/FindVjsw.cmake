

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VJSW_FOUND )
	vista_find_package_root( jsw include/jsw.h )

	if( JSW_ROOT_DIR )
		set( JSW_INCLUDE_DIRS ${JSW_ROOT_DIR}/include )
		set( JSW_LIBRARY_DIRS ${JSW_ROOT_DIR}/lib )
		set( JSW_LIBRARIES jsw )
	endif( JSW_ROOT_DIR )

endif( NOT VJSW_FOUND )

find_package_handle_standard_args( Vjsw "JSW could not be found" JSW_ROOT_DIR )
