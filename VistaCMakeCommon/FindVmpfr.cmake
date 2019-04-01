


include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VMPFR_FOUND )

	vista_find_package_root( mpfr "include/mpfr.h"  )
	
	if( MPFR_ROOT_DIR)
		set( MPFR_INCLUDE_DIRS "${MPFR_ROOT_DIR}/include" )
		set( MPFR_LIBRARY_DIRS "${MPFR_ROOT_DIR}/lib" )
		if( WIN32 )
			set( MPFR_LIBRARIES "libmpfr-4" )
		else()
			set( MPFR_LIBRARIES "mpfr" )
		endif()
	endif()

endif( NOT VMPFR_FOUND )

find_package_handle_standard_args( Vmpfr "mpfr could not be found" MPFR_ROOT_DIR )

