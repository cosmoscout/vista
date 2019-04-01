

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VPNG_FOUND )

	# find libpng looking for various lib names
	vista_find_package_root( PNG include/png.h NAMES libpng LibPNG libPNG png PNG)

	if( PNG_ROOT_DIR )
		set(PNG_NAMES ${PNG_NAMES} png libpng png15 libpng15 png15d libpng15d png14 libpng14 png14d libpng14d png12 libpng12 png12d libpng12d)
		find_library( PNG_LIBRARIES NAMES ${PNG_NAMES}
					PATHS ${PNG_ROOT_DIR}/lib
					CACHE "PNG library" )
		set( PNG_INCLUDE_DIRS ${PNG_ROOT_DIR}/include CACHE STRING "PNG include dir.")
		set( PNG_LIBRARY_DIRS ${PNG_ROOT_DIR}/lib ${PNG_ROOT_DIR}/bin CACHE STRING "PNG library dir.")

		set( PNG_DEPENDENCIES package ZLIB REQUIRED)

		endif( PNG_ROOT_DIR )
endif( NOT VPNG_FOUND )

find_package_handle_standard_args( VPNG "PNG could not be found" PNG_ROOT_DIR PNG_LIBRARIES)

