

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VCURL_FOUND )
	vista_find_package_root( CURL include/curl/curl.h )

	if( CURL_ROOT_DIR )
		set( CURL_LIBRARIES libcurl )
		mark_as_advanced( CURL_LIBRARIES )

		set( CURL_INCLUDE_DIRS ${CURL_ROOT_DIR}/include )
		set( CURL_LIBRARY_DIRS ${CURL_ROOT_DIR}/lib )
	endif( CURL_ROOT_DIR )
endif( NOT VCURL_FOUND )

find_package_handle_standard_args( VCURL "CURL could not be found" CURL_ROOT_DIR )

