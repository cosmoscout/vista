include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VFFTW_FOUND )
    vista_find_package_root( fftw include/fftw3.h ) # find dir containing 'fftw3.h'
    if( FFTW_ROOT_DIR )
        set( FFTW_INCLUDE_DIRS "${FFTW_ROOT_DIR}/include" )
        set( FFTW_LIBRARY_DIRS "${FFTW_ROOT_DIR}/lib" "${FFTW_ROOT_DIR}/bin" )
		set( FFTW_LIBRARIES "libfftw3-3" "libfftw3f-3" "libfftw3l-3" )
    endif( FFTW_ROOT_DIR )
endif( NOT VFFTW_FOUND )

find_package_handle_standard_args( Vfftw "FFTW could not be found" FFTW_ROOT_DIR )