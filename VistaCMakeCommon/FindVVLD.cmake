

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( WIN32 )
	if( NOT VVLD_FOUND )
		vista_find_package_root( vld "lib/Win64/vld.lib" )	

		if( VLD_ROOT_DIR )
			set( VLD_INCLUDE_DIRS "${VLD_ROOT_DIR}/include")
			set( VLD_LIBRARY_DIRS 
				"${VLD_ROOT_DIR}/lib/Win64"
				"${VLD_ROOT_DIR}/bin/Win64"
				)		
		else( VLD_ROOT_DIR )
			message( WARNING "vista_find_package_root - vld.lib" )	
		endif( VLD_ROOT_DIR )
	endif( NOT VVLD_FOUND )
	find_package_handle_standard_args( Vvld "vld could not be found" VLD_ROOT_DIR )	
endif( WIN32 )
