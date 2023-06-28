

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENNI_FOUND )

	#looking for two options: once with ni subdir, once without	
	vista_find_package_root( OpenNI include/ni/XnOpenNI.h )
	vista_find_package_root( OpenNI include/XnOpenNI.h )

	if( OPENNI_ROOT_DIR )
		if( EXISTS "${OPENNI_ROOT_DIR}/include/ni/XnOpenNI.h" )
			set( OPENNI_INCLUDE_DIRS "${OPENNI_ROOT_DIR}/include/ni" )
		else( EXISTS "${OPENNI_ROOT_DIR}/include/ni/XnOpenNI.h" )
			set( OPENNI_INCLUDE_DIRS "${OPENNI_ROOT_DIR}/include" )
		endif( EXISTS "${OPENNI_ROOT_DIR}/include/ni/XnOpenNI.h" )
		
		if( VISTA_64BIT )
			set( OPENNI_LIBRARY_DIRS "${OPENNI_ROOT_DIR}/lib64" "${OPENNI_ROOT_DIR}/bin64" )
			set( OPENNI_LIBRARIES OpenNI64 )
		else()
			set( OPENNI_LIBRARY_DIRS "${OPENNI_ROOT_DIR}/lib" "${OPENNI_ROOT_DIR}/bin" )
			set( OPENNI_LIBRARIES OpenNI )
		endif()
	endif( OPENNI_ROOT_DIR )	

endif( NOT VOPENNI_FOUND )

find_package_handle_standard_args( VOpenNI "OpenNI could not be found" OPENNI_ROOT_DIR )



