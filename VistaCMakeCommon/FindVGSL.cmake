

# find package for the gnu scientific library
# download from: http://gnuwin32.sourceforge.net/packages/gsl.htm
# check the library names in the download. The c-libraries are named with gnu
# like naming, that will collide when building on windows.
# consider renaming the libraries from "libgsl.a" to "gsl.lib", same for cblas

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VGSL_FOUND )

	vista_find_package_root( GSL include/gsl/gsl_sys.h )

	if( GSL_ROOT_DIR )
		#find_library( GSL_LIBRARY NAMES libgsl.a
		#			PATHS ${GSL_ROOT_DIR}/lib
		#			CACHE "GSL library" )
		#find_library( GSL_LIBRARY_CBLAS NAMES cblas
		#			PATHS ${GSL_ROOT_DIR}/lib
		#			CACHE "GSL cblas library" )
		#mark_as_advanced( GSL_LIBRARY )
		#mark_as_advanced( GSL_LIBRARY_CBLAS )
		#set( GSL_LIBRARIES "${GSL_LIBRARY}" "${GSL_LIBRARY_CBLAS}" )
		set( GSL_LIBRARIES gsl gslcblas )
		
		set( GSL_INCLUDE_DIRS "${GSL_ROOT_DIR}/include" )
		set( GSL_LIBRARY_DIRS "${GSL_ROOT_DIR}/lib" )
		

	endif( GSL_ROOT_DIR )

endif( NOT VGSL_FOUND )

find_package_handle_standard_args( VGSL "GSL could not be found" GSL_ROOT_DIR )

