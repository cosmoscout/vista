


include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VNATNETSDK_FOUND )

	vista_find_package_root( NatNetSDK "include/NatNetServer.h" )
	
	if( NATNETSDK_ROOT_DIR )
		
		set( NATNETSDK_INCLUDE_DIRS "${NATNETSDK_ROOT_DIR}/include" )
		if( VISTA_64BIT )
			set( NATNETSDK_LIBRARY_DIRS "${NATNETSDK_ROOT_DIR}/lib/x64" )
		else()
			set( NATNETSDK_LIBRARY_DIRS "${NATNETSDK_ROOT_DIR}/lib" )
		endif()
		set( NATNETSDK_LIBRARIES NatNetLib )
	
	endif()
	
endif()

find_package_handle_standard_args( VNatNetSDK "Optitrack NatNetSDK could not be found" NATNETSDK_ROOT_DIR NATNETSDK_LIBRARIES )

