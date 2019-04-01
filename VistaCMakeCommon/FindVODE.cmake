

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VODE_FOUND )

	vista_find_package_root( ODE include/ode/ode.h NAMES ode ODE)

	if( ODE_ROOT_DIR )
		vista_find_library_uncached( NAMES ode ODE ode_double.lib ode_single.lib
									PATHS "${ODE_ROOT_DIR}/lib"
									CACHE "ODE library" )

		set( ODE_INCLUDE_DIRS ${ODE_ROOT_DIR}/include )
		set( ODE_LIBRARY_DIRS ${ODE_ROOT_DIR}/lib )
				
		get_filename_component( ODE_LIBRARY_DIRS "${VISTA_UNCACHED_LIBRARY}" PATH )
		get_filename_component( ODE_LIBRARIES "${VISTA_UNCACHED_LIBRARY}" NAME )
	endif( ODE_ROOT_DIR )

endif( NOT VODE_FOUND )

find_package_handle_standard_args( VODE "ODE could not be found" ODE_ROOT_DIR ODE_LIBRARIES)

