

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VFOX_FOUND )
	vista_find_package_root( FOX include/fox-1.6/fx.h )

	if( FOX_ROOT_DIR )
		set( FOX_INCLUDE_DIRS ${FOX_ROOT_DIR}/include/fox-1.6 )
		set( FOX_LIBRARY_DIRS ${FOX_ROOT_DIR}/lib )
		set( FOX_LIBRARIES FOX-1.6 CHART-1.6 )
	endif( FOX_ROOT_DIR )

endif( NOT VFOX_FOUND )

find_package_handle_standard_args( VFOX "FOX could not be found" FOX_ROOT_DIR FOX_LIBRARIES )
