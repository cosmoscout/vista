

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENMP_FOUND )

	vista_find_original_package( OpenMP )
		
	if( OPENMP_FOUND )
		find_file( OPENMP_USE_FILE "VOpenMPUseFile.cmake" )
		mark_as_advanced( OPENMP_USE_FILE )
	endif()

endif( NOT VOPENMP_FOUND )

find_package_handle_standard_args( VOPENMP "OpenMP could not be found" OPENMP_USE_FILE )


