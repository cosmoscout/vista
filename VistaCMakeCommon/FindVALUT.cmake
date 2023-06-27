

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VALUT_FOUND )
	vista_find_package_root( ALUT include/AL/alut.h NAMES alut freealut )

	if( ALUT_ROOT_DIR )
		set( ALUT_INCLUDE_DIRS ${ALUT_ROOT_DIR}/include/AL )
		set( ALUT_LIBRARY_DIRS ${ALUT_ROOT_DIR}/lib )
		set( ALUT_LIBRARIES alut )
	endif( ALUT_ROOT_DIR )

endif( NOT VALUT_FOUND )

find_package_handle_standard_args( VALUT "ALUT could not be found" ALUT_ROOT_DIR )
