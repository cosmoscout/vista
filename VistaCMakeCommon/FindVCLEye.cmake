

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VCLEYE_FOUND )
	if( WIN32 AND VISTA_32BIT )
	
		vista_find_package_root( CLEye include/CLEyeMulticam.h NAMES "Code Laboratories" "CL-Eye Platform SDK" "Code Laboratories/CL-Eye Platform SDK" )

		if( CLEYE_ROOT_DIR )
			set( CLEYE_INCLUDE_DIRS ${CLEYE_ROOT_DIR}/include )
			set( CLEYE_LIBRARY_DIRS ${CLEYE_ROOT_DIR}/lib )
			set( CLEYE_LIBRARIES CLEyeMulticam )
		endif( CLEYE_ROOT_DIR )

	elseif( NOT VCLEye_FIND_QUIETLY )
	
		message( "CLEye SDK only available as 32bit Windows" )
		
	endif()
	
endif( NOT VCLEYE_FOUND )

find_package_handle_standard_args( VCLEye "CLEYE could not be found" CLEYE_ROOT_DIR CLEYE_LIBRARIES )



