

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

#if( NOT WIN32 )
#	message( AUTHOR_WARNING "FindVOpenHaptics.cmake has only been tested for WIN32 yet!" )
#endif( NOT WIN32 )

if( NOT VOPENHAPTICS_FOUND )

	#firt option: custom deployment (/lib and /include folders)
	vista_find_package_root( OpenHaptics include/HD/hd.h NAMES "Senseable/3DTouch" "3DTouch" )

	if( OPENHAPTICS_ROOT_DIR )
		# two options: either, all includes/libs are in the same folders, or
		# the default deployment is used (with toplevel subfolders )

		if( WIN32 AND EXISTS "${OPENHAPTICS_ROOT_DIR}/utilities/include/HDU/hdu.h" )
			#install deployment
			set( OPENHAPTICS_ROOT_DIR ${OPENHAPTICS_ROOT_DIR} )
			set( OPENHAPTICS_INCLUDE_DIRS "${OPENHAPTICS_ROOT_DIR}/include"
										"${OPENHAPTICS_ROOT_DIR}/utilities/include" )
			set( OPENHAPTICS_LIBRARY_DIRS )

			set( OPENHAPTICS_hd_LIBRARY "OPENHAPTICS_hd_LIBRARY-NOTFOUND" CACHE PATH "OpenHaptics hd library" FORCE )
			find_library( OPENHAPTICS_hd_LIBRARY HD PATHS ${OPENHAPTICS_ROOT_DIR}
							PATH_SUFFIXES "lib/win32" "lib/x64"	NO_DEFAULT_PATH )
			if( OPENHAPTICS_hd_LIBRARY )
				list( APPEND OPENHAPTICS_LIBRARIES ${OPENHAPTICS_hd_LIBRARY} )
				get_filename_component( _PATH ${OPENHAPTICS_hd_LIBRARY} PATH )
				list( APPEND OPENHAPTICS_LIBRARY_DIRS ${_PATH} )
			else()
				message( SEND_ERROR "hd library was not found in \"${OPENHAPTICS_ROOT_DIR}\"" )
			endif()
			set( OPENHAPTICS_hdu_LIBRARY "OPENHAPTICS_hdu_LIBRARY-NOTFOUND" CACHE PATH "OpenHaptics hdu library" FORCE )
			find_library( OPENHAPTICS_hdu_LIBRARY HDU PATHS ${OPENHAPTICS_ROOT_DIR}
							PATH_SUFFIXES
							"utilities/lib/win32/ReleaseAcademicEdition" "utilities/lib/win32/Release"
							"utilities/lib/x64/ReleaseAcademicEdition" "utilities/lib/x64/Release"
							NO_DEFAULT_PATH
			)
			if( OPENHAPTICS_hdu_LIBRARY )
				list( APPEND OPENHAPTICS_LIBRARIES ${OPENHAPTICS_hdu_LIBRARY} )
				get_filename_component( _PATH ${OPENHAPTICS_hdu_LIBRARY} PATH )
				list( APPEND OPENHAPTICS_LIBRARY_DIRS ${_PATH} )				
			else()
				message( SEND_ERROR "hdu library was not found in \"${OPENHAPTICS_ROOT_DIR}\"" )
			endif()
			mark_as_advanced( OPENHAPTICS_hd_LIBRARY )
			mark_as_advanced( OPENHAPTICS_hdu_LIBRARY )
		else( WIN32 AND EXISTS "${OPENHAPTICS_ROOT_DIR}/utilities/include/HDU/hdu.h" )
			#own deployment
			#message( WARNING "Warning hdu library is not included - only hd library will be available" )
			set( OPENHAPTICS_ROOT_DIR ${OPENHAPTICS_ROOT_DIR} )
			set( OPENHAPTICS_INCLUDE_DIRS "${OPENHAPTICS_ROOT_DIR}/include" )
			set( OPENHAPTICS_LIBRARY_DIRS "${OPENHAPTICS_ROOT_DIR}/lib" )
			set( OPENHAPTICS_LIBRARIES ${OPENHAPTICS_LIBRARIES} HD )
		endif( WIN32 AND EXISTS "${OPENHAPTICS_ROOT_DIR}/utilities/include/HDU/hdu.h" )


	endif( OPENHAPTICS_ROOT_DIR )

endif( NOT VOPENHAPTICS_FOUND )

find_package_handle_standard_args( VOpenHaptics "OPENHAPTICS could not be found" OPENHAPTICS_ROOT_DIR OPENHAPTICS_LIBRARIES )
