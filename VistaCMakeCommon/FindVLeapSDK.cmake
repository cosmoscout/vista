

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VLeapSDK_FOUND )
	vista_find_package_root( LeapSDK include/Leap.h SUBDIRS LeapSDK )

	if( LEAPSDK_ROOT_DIR )
		set( LEAPSDK_INCLUDE_DIRS "${LEAPSDK_ROOT_DIR}/include" )
		if( VISTA_64BIT )
			set( LEAPSDK_LIBRARY_DIRS "${LEAPSDK_ROOT_DIR}/lib/x64" )
		else()
			set( LEAPSDK_LIBRARY_DIRS "${LEAPSDK_ROOT_DIR}/lib/x86" )
		endif()
		if( WIN32 )
		vista_find_library_uncached( _LEAP_DEBUG_LIB Leapd "${LEAPSDK_LIBRARY_DIRS}" )
			if( _LEAP_DUBUG_LIB )
				set( LEAPSDK_LIBRARIES optimized Leap debug Leapd )
			else()
				set( LEAPSDK_LIBRARIES Leap )
			endif()
		else()
			set( LEAPSDK_LIBRARIES Leap )
		endif()
	endif()
endif()

find_package_handle_standard_args( VLeapSDK "LeapSDK could not be found" LEAPSDK_ROOT_DIR LEAPSDK_LIBRARIES )

