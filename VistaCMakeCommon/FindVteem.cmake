

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VTEEM_FOUND )
	vista_find_package_root( TEEM include/teem/nrrd.h DEBUG_OUTPUT )

	if( TEEM_ROOT_DIR )
        find_library(TEEM_LIBRARIES NAMES teem.lib teem PATHS ${TEEM_ROOT_DIR}/lib CACHE "teem library")
        #set(TEEM_LIBRARIES optimized teem debug teem)
        mark_as_advanced(TEEM_LIBRARIES)
	set (VTEEM_FOUND)

		set( TEEM_INCLUDE_DIRS ${TEEM_ROOT_DIR}/include )
        set( TEEM_LIBRARY_DIRS ${TEEM_ROOT_DIR}/lib )
        get_filename_component(TEEM_LIBRARY_DIRS ${TEEM_LIBRARIES} PATH)
	endif( TEEM_ROOT_DIR )
endif( NOT VTEEM_FOUND )

find_package_handle_standard_args( VTEEM "teem could not be found" TEEM_ROOT_DIR )
