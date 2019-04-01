

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPTITRACKCAMERASDK_FOUND )
	vista_find_package_root( OPTITRACKCAMERASDK include/cameramanager.h NAMES Optitrack Optitrack/CameraSDK CameraSDK "Camera SDK" "Optitrack/Camera SDK" )
	
	if( OPTITRACKCAMERASDK_ROOT_DIR )
		set( OPTITRACKCAMERASDK_INCLUDE_DIRS "${OPTITRACKCAMERASDK_ROOT_DIR}/include" )
		set( OPTITRACKCAMERASDK_LIBRARY_DIRS "${OPTITRACKCAMERASDK_ROOT_DIR}/lib" )
		if( VISTA_64BIT )
			set( OPTITRACKCAMERASDK_LIBRARIES cameralibrarydrtx64 )
		else()
			set( OPTITRACKCAMERASDK_LIBRARIES cameralibrarydrt )
		endif()
		if( WIN32 )
			set( OPTITRACKCAMERASDK_DEFINITIONS -DCAMERALIBRARY_IMPORTS )
		endif()
	endif()

endif()

find_package_handle_standard_args( VOptitrackCameraSDK "OptitrackCameraSDK could not be found" OPTITRACKCAMERASDK_ROOT_DIR )

