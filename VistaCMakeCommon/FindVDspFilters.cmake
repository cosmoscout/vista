# $Id: FindVDspFilters.cmake 21495 2011-05-25 07:52:18Z dr165799 $

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VDSPFILTERS_FOUND )
	vista_find_package_root( DspFilters "include/DspFilters/Bessel.h" )	

	if( DSPFILTERS_ROOT_DIR )
		set( DSPFILTERS_INCLUDE_DIRS "${DSPFILTERS_ROOT_DIR}/include" )
        set( DSPFILTERS_LIBRARY_DIRS "${DSPFILTERS_ROOT_DIR}/lib" )
		set( DSPFILTERS_LIBRARIES optimized DspFilters debug DspFiltersD )
	else( DSPFILTERS_ROOT_DIR )
		message( WARNING "vista_find_package_root - File named Bessel.h not found" )	
	endif( DSPFILTERS_ROOT_DIR )
endif( NOT VDSPFILTERS_FOUND )

find_package_handle_standard_args( VDSPFILTERS "DspFilters could not be found" DSPFILTERS_ROOT_DIR )

