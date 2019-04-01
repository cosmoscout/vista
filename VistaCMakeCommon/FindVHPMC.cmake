

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VHPMC_FOUND )
	vista_find_package_root( HPMC hpmc/include/hpmc.h )

	if( HPMC_ROOT_DIR )
		set( HPMC_INCLUDE_DIRS ${HPMC_ROOT_DIR}/hpmc/include )
		set( HPMC_LIBRARY_DIRS ${HPMC_ROOT_DIR}/hpmc/lib )
		set( HPMC_LIBRARIES
			optimized HPMC
			debug HPMCD
		)
	endif( HPMC_ROOT_DIR )
endif( NOT VHPMC_FOUND )

find_package_handle_standard_args( VHPMC "HPMC could not be found" HPMC_ROOT_DIR HPMC_LIBRARIES )

