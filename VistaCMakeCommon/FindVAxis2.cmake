

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VAXIS2_FOUND )

	vista_find_package_root( AXIS2 include/axis2_addr.h ) # Arbitrary header chosen

	if( AXIS2_ROOT_DIR )
		if( UNIX )
			set( AXIS2_LIBRARIES
				axis2_axiom
				axis2_engine
				axis2_http_common
				axis2_http_receiver
				axis2_http_sender
				axis2_parser
				axis2_xpath
				axutil
				guththila
				neethi				
				)
		else()
			set( AXIS2_LIBRARIES 
				axiom
				axis2_engine
				axis2_http_receiver
				axis2_http_sender
				axis2_parser
				axis2_tcp_receiver
				axis2_tcp_sender
				axis2_xpath
				axutil
				guththila
				neethi
				)
		endif()
		mark_as_advanced( AXIS2_LIBRARIES )

		set( AXIS2_INCLUDE_DIRS ${AXIS2_ROOT_DIR}/include )
		set( AXIS2_LIBRARY_DIRS ${AXIS2_ROOT_DIR}/lib )
	endif( AXIS2_ROOT_DIR )

endif( NOT VAXIS2_FOUND )

find_package_handle_standard_args( VAXIS2 "AXIS2 could not be found" AXIS2_ROOT_DIR )

