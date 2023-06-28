

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VSOLID_FOUND )
	vista_find_package_root( Solid include/SOLID.h )

	if( SOLID_ROOT_DIR )

		set( SOLID_INCLUDE_DIRS ${SOLID_ROOT_DIR}/include )
		set( SOLID_LIBRARY_DIRS ${SOLID_ROOT_DIR}/lib )

		if( WIN32 )
			set( SOLID_LIBRARIES
				optimized solid
				optimized qhull
				optimized broad
				optimized complex
				optimized convex
				debug solidD
				debug qhullD
				debug broadD
				debug complexD
				debug convexD
			)
		else()
			set( SOLID_LIBRARIES solid )
		endif( WIN32 )

	endif( SOLID_ROOT_DIR )

endif( NOT VSOLID_FOUND )

find_package_handle_standard_args( VSolid "Solid could not be found" SOLID_ROOT_DIR )
