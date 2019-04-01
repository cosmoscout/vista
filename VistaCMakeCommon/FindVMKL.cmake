

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )
include( VistaCommon )

if( NOT VMKL_FOUND )
	vista_find_package_root( MKL include/mkl.h PATHS "C:/Program Files (x86)/Intel/Composer XE/mkl/" "$ENV{MKLROOT}" )
	
	if( MKL_ROOT_DIR )
		set( MKL_INCLUDE_DIRS "${MKL_ROOT_DIR}/include" )
		if( WIN32 )			
			if( VISTA_64BIT )
				set( MKL_LIBRARY_DIRS "${MKL_ROOT_DIR}/lib/intel64" "${MKL_ROOT_DIR}/../redist/intel64/mkl" )
				set( MKL_LIBRARIES mkl_core mkl_lapack95_lp64 mkl_rt )
			else()
				set( MKL_LIBRARY_DIRS "${MKL_ROOT_DIR}/lib/ia32" "${MKL_ROOT_DIR}/../redist/ia32/mkl" )
				set( MKL_LIBRARIES mkl_core mkl_lapack95 mkl_rt )
			endif()
		else()
			include_directories( "${MKL_ROOT_DIR}/include" )
			if( VISTA_64BIT )
				set( MKL_LIBRARY_DIRS "${MKL_ROOT_DIR}/lib/intel64" "${MKL_ROOT_DIR}/../compiler/lib/intel64" )
				set( MKL_LIBRARIES mkl_core mkl_intel_lp64 mkl_intel_thread iomp5 )
			else()
				set( MKL_LIBRARY_DIRS "${MKL_ROOT_DIR}/lib/ia32" "${MKL_ROOT_DIR}/../compiler/lib/ia32" )
				set( MKL_LIBRARIES mkl_core mkl_lapack95 mkl_rt iomp5 )
			endif()
		endif()
	endif()	

endif( NOT VMKL_FOUND )

find_package_handle_standard_args( VMKL "MKL could not be found" MKL_ROOT_DIR MKL_LIBRARIES )

