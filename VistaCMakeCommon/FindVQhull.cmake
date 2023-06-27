

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VQHULL_FOUND )

	vista_find_package_root( QHULL include/libqhullcpp/Qhull.h )

	if( QHULL_ROOT_DIR )
		set( QHULL_LIBRARIES 
			optimized qhullcpp
			optimized qhullstatic_p
			debug qhullcppD
			debug qhullstatic_pD )
		mark_as_advanced( QHULL_LIBRARIES )

		set( QHULL_INCLUDE_DIRS ${QHULL_ROOT_DIR}/include )
		set( QHULL_LIBRARY_DIRS ${QHULL_ROOT_DIR}/lib )

	endif( QHULL_ROOT_DIR )

endif( NOT VQHULL_FOUND )

find_package_handle_standard_args( VQHULL "Qhull could not be found" QHULL_ROOT_DIR )

