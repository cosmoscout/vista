

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT ZLIB_FOUND )

	vista_find_package_root( ZLIB include/zlib.h NAMES zlib Zlib ZLIB)

	if( ZLIB_ROOT_DIR )
		find_library( ZLIB_LIBRARIES	NAMES zlib libz
					PATHS ${ZLIB_ROOT_DIR}/lib
					CACHE "ZLIB library" )
					
		set( ZLIB_INCLUDE_DIRS ${ZLIB_ROOT_DIR}/include CACHE STRING "ZLIB include dir.")
		mark_as_advanced( ZLIB_INCLUDE_DIRS )
		set( ZLIB_LIBRARY_DIRS ${ZLIB_ROOT_DIR}/lib ${ZLIB_ROOT_DIR}/bin CACHE STRING "ZLIB library dir.")
		mark_as_advanced( ZLIB_LIBRARY_DIRS )
	endif( ZLIB_ROOT_DIR )

endif( NOT ZLIB_FOUND )

find_package_handle_standard_args( VZLIB "ZLIB could not be found" ZLIB_ROOT_DIR ZLIB_LIBRARIES)

