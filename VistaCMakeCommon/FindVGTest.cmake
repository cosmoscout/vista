

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VGTEST_FOUND )
	vista_find_package_root( GTest include/gtest/gtest.h NAMES GTest gtest GTEST )
	set( GTEST_ROOT ${GTEST_ROOT_DIR} )

	vista_find_original_package( VGTest )
	
	if( MSVC11 )
		set( GTEST_DEFINITIONS ${GTEST_DEFINITIONS} "-D_VARIADIC_MAX=10" )
	endif()
	
endif( NOT VGTEST_FOUND )

find_package_handle_standard_args( VGTest "GTest could not be found" GTEST_ROOT_DIR )

