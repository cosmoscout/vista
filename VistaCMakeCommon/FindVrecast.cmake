

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VRECAST_FOUND )

	vista_find_package_root( RECAST include/Recast.h )

	if( RECAST_ROOT_DIR )
		find_library( RECAST_LIBRARIES NAMES Recast recast.lib
					PATHS ${RECAST_ROOT_DIR}/lib 
					CACHE "recast library" )
		mark_as_advanced( RECAST_LIBRARIES )

		set( RECAST_INCLUDE_DIRS ${RECAST_ROOT_DIR}/include )
		set( RECAST_LIBRARY_DIRS ${RECAST_ROOT_DIR}/lib  )
		get_filename_component( RECAST_LIBRARY_DIRS ${RECAST_LIBRARIES} PATH )

	endif( RECAST_ROOT_DIR )

endif( NOT VRECAST_FOUND )

find_package_handle_standard_args( Vrecast "recast could not be found" RECAST_ROOT_DIR )

