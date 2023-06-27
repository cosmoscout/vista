

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VMESCHACH_FOUND )
	vista_find_package_root( Meschach include/zmatrix2.h )

	if( MESCHACH_ROOT_DIR )
		set( MESCHACH_INCLUDE_DIRS ${MESCHACH_ROOT_DIR}/include )
		set( MESCHACH_LIBRARY_DIRS ${MESCHACH_ROOT_DIR}/lib )
		set( MESCHACH_LIBRARIES
			optimized Meschach
			debug MeschachD
		)
	endif( MESCHACH_ROOT_DIR )
endif( NOT VMESCHACH_FOUND )

find_package_handle_standard_args( VMeschach "Meschach could not be found" MESCHACH_ROOT_DIR MESCHACH_LIBRARIES )

