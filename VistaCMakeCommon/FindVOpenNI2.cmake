

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENNI2_FOUND )
	vista_find_package_root( OpenNI2 Include/OpenNI.h )

	if( OPENNI2_ROOT_DIR )
		set( OPENNI2_INCLUDE_DIRS "${OPENNI2_ROOT_DIR}/Include" )
		set( OPENNI2_LIBRARY_DIRS "${OPENNI2_ROOT_DIR}/Bin/x64-Debug" )
		set( OPENNI2_LIBRARIES OpenNI2 )
	endif( OPENNI2_ROOT_DIR )
endif( NOT VOPENNI2_FOUND )

find_package_handle_standard_args( VOpenNI2 "OpenNI could not be found" OPENNI2_ROOT_DIR )
