

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VDEPTHSENSESDK_FOUND )
	vista_find_package_root( DepthSenseSDK include/DepthSense.hxx )

	if( DEPTHSENSESDK_ROOT_DIR )
		unset ( VDEPTHSENSESDK_FOUND CACHE )
	
		set( DEPTHSENSESDK_LIBRARIES DepthSense  )
		set( DEPTHSENSESDK_LIBRARY_DIRS "${DEPTHSENSESDK_ROOT_DIR}/lib" )
		set( DEPTHSENSESDK_INCLUDE_DIRS "${DEPTHSENSESDK_ROOT_DIR}/include" )
		
	endif( DEPTHSENSESDK_ROOT_DIR )
	

endif( NOT VDEPTHSENSESDK_FOUND )

find_package_handle_standard_args( VDepthSenseSDK "DepthSenseSDK could not be found" DEPTHSENSESDK_LIBRARIES )
