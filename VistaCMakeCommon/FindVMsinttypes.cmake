

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT MSINTTYPES_FOUND )

	vista_find_package_root( MSINTTYPES inttypes.h NAMES msinttypes )
	if( MSINTTYPES_ROOT_DIR )	
		set( MSINTTYPES_INCLUDE_DIRS ${MSINTTYPES_ROOT_DIR} CACHE STRING "MSINTTYPES include dir.")
		mark_as_advanced( MSINTTYPES_INCLUDE_DIRS )
	endif( MSINTTYPES_ROOT_DIR )

endif( NOT MSINTTYPES_FOUND )

find_package_handle_standard_args( VMSINTTYPES "MSINTTYPES could not be found" MSINTTYPES_ROOT_DIR )

