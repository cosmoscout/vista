

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VSIXENSESDK_FOUND )
	vista_find_package_root( SixenseSDK include/sixense.h )
	
	if( SIXENSESDK_ROOT_DIR )
		set( SIXENSESDK_INCLUDE_DIRS "${SIXENSESDK_ROOT_DIR}/include" )
		if( WIN32 )
			if( VISTA_64BIT )
				set( SIXENSESDK_LIBRARY_DIRS "${SIXENSESDK_ROOT_DIR}/lib/x64/debug_dll" "${SIXENSESDK_ROOT_DIR}/lib/x64/release_dll" "${SIXENSESDK_ROOT_DIR}/bin/x64/debug_dll" "${SIXENSESDK_ROOT_DIR}/bin/x64/release_dll" )
				set( SIXENSESDK_LIBRARIES optimized sixense_x64 optimized sixense_utils_x64 debug sixensed_x64 debug sixense_utilsd_x64 )
			else()
				set( SIXENSESDK_LIBRARY_DIRS "${SIXENSESDK_ROOT_DIR}/lib/win32/debug_dll" "${SIXENSESDK_ROOT_DIR}/lib/win32/release_dll" "${SIXENSESDK_ROOT_DIR}/bin/win32/debug_dll" "${SIXENSESDK_ROOT_DIR}/bin/win32/release_dll" )
				set( SIXENSESDK_LIBRARIES optimized sixense optimized sixense_utils debug sixensed debug sixense_utilsd )
			endif()
		elseif( APPLE )
			if( VISTA_64BIT )
				set( SIXENSESDK_LIBRARY_DIRS "${SIXENSESDK_ROOT_DIR}/lib/osx_x64/debug_dll" "${SIXENSESDK_ROOT_DIR}/lib/osx_x64/release_dll" )
				set( SIXENSESDK_LIBRARIES optimized sixense_x64 optimized sixense_utils_x64 debug sixensed_x64 debug sixense_utilsd_x64 )
			else()
				set( SIXENSESDK_LIBRARY_DIRS "${SIXENSESDK_ROOT_DIR}/lib/osx/debug_dll" "${SIXENSESDK_ROOT_DIR}/lib/osx/release_dll" )
				set( SIXENSESDK_LIBRARIES optimized sixense optimized sixense_utils debug sixensed debug sixense_utilsd )
			endif()
		elseif( UNIX )
			if( VISTA_64BIT )
				set( SIXENSESDK_LIBRARY_DIRS "${SIXENSESDK_ROOT_DIR}/lib/linux_x64/release" "${SIXENSESDK_ROOT_DIR}/lib/linux_x64/debug" )
				set( SIXENSESDK_LIBRARIES optimized sixense_x64 optimized sixense_utils_x64 debug sixensed_x64 debug sixense_utilsd_x64 )
			else()
				set( SIXENSESDK_LIBRARY_DIRS "${SIXENSESDK_ROOT_DIR}/lib/linux/release" "${SIXENSESDK_ROOT_DIR}/lib/linux/debug" )
				set( SIXENSESDK_LIBRARIES optimized sixense optimized sixense_utils debug sixensed debug sixense_utilsd )
			endif()
		elseif( NOT SIXENSESDK_FIND_QUIET )
			message( WARNING "SixenseSDK Find Module not supported for this platform" )
		endif()
		

	endif( SIXENSESDK_ROOT_DIR )

endif( NOT VSIXENSESDK_FOUND )

find_package_handle_standard_args( VSIXENSESDK "SixenseSDK could not be found" SIXENSESDK_LIBRARY_DIRS SIXENSESDK_LIBRARIES )

