

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VFTGL_FOUND )

	vista_find_package_root( FTGL include/FTGL/ftgl.h )

	if( FTGL_ROOT_DIR )
		find_library( FTGL_LIBRARIES NAMES ftgl
					PATHS ${FTGL_ROOT_DIR}/lib
					CACHE "FTGL library" )
		mark_as_advanced( FTGL_LIBRARIES )

		set( FTGL_INCLUDE_DIRS ${FTGL_ROOT_DIR}/include )
		set( FTGL_LIBRARY_DIRS ${FTGL_ROOT_DIR}/lib )
		get_filename_component( FTGL_LIBRARY_DIRS ${FTGL_LIBRARIES} PATH )

	endif( FTGL_ROOT_DIR )

endif( NOT VFTGL_FOUND )

find_package_handle_standard_args( VFTGL "FTGL could not be found" FTGL_ROOT_DIR )

