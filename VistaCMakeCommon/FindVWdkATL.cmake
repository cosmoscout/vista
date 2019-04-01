

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VWDKATL_FOUND )

	if( NOT WDKATL_ROOT_DIR )
		set( _CANDIDATE_PATHES )
		set( _SEARCH_FILE "inc/atl71/atlbase.h" )

		foreach( _PATH $ENV{WDK_ROOT} "C:" ${VISTA_PACKAGE_SEARCH_PATHS} )		
			file( GLOB _TMP_PATHES "${_PATH}/WinDDK/*.*.*/" )
			list( APPEND _CANDIDATE_PATHES ${_TMP_PATHES} )
		endforeach()

		#check unversioned pathes
		foreach( _PATH ${_CANDIDATE_PATHES} )
			file( TO_CMAKE_PATH "${_PATH}" _PATH )
			if( EXISTS "${_PATH}/${_EXAMPLE_FILE}" )			
				# determine version
				string( REGEX MATCH ".+WinDDK/([0-9]+).([0-9]+).([0-9]+)" _MATCHED "${_PATH}" )
				if( _MATCHED )
					set( _FOUND_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}" )
					# TODO. version checking - for now, we take the most current one
					if( NOT WDKATL_VERSION OR _FOUND_VERSION VERSION_GREATER WDKATL_VERSION )
						set( WDKATL_ROOT_DIR "${_PATH}" )
						set( WDKATL_VERSION "${_FOUND_VERSION}" )
					endif()
				else()
					message( "FindVWdkATL can't extract version from \"${_PATH}\"" )
					if( NOT WDKATL_ROOT_DIR )
						set( WDKATL_ROOT_DIR "${_PATH}" )
					endif()
				endif( _MATCHED )
			endif()
			
		endforeach( _PATH ${_VERSIONED_PATHES} )	
		
		set( WDKATL_ROOT_DIR ${WDKATL_ROOT_DIR} CACHE PATH "Root dir of WindowDriverKit's Installation for ATL" FORCE )
	endif()
	

	if( WDKATL_ROOT_DIR )
		set( WDKATL_INCLUDE_DIRS "${WDKATL_ROOT_DIR}/inc/atl71" )
		if( VISTA_64BIT )
			set( WDKATL_LIBRARY_DIRS "${WDKATL_ROOT_DIR}/lib/ATL/amd64" ) # TODO: or ia64?
		else()
			set( WDKATL_LIBRARY_DIRS "${WDKATL_ROOT_DIR}/lib/ATL/i386" )
		endif()
		set( WDKATL_LIBRARIES
			optimized atl
			debug atlsd # TODO: is this really debug?
		)

	endif( WDKATL_ROOT_DIR )

endif( NOT VWDKATL_FOUND )

find_package_handle_standard_args( VWDKATL "WindowsDriverKit's ATL installation could not be found" WDKATL_ROOT_DIR )

