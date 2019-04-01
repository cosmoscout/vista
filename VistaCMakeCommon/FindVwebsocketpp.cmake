

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VWEBSOCKETPP_FOUND )
	vista_find_package_root( websocketpp websocketpp/client.hpp )	

	if( WEBSOCKETPP_ROOT_DIR )
		set( WEBSOCKETPP_INCLUDE_DIRS ${WEBSOCKETPP_ROOT_DIR})		
	else( WEBSOCKETPP_ROOT_DIR )
		message( WARNING "vista_find_package_root - cleint.h not found" )	
	endif( WEBSOCKETPP_ROOT_DIR )
endif( NOT VWEBSOCKETPP_FOUND )

find_package_handle_standard_args( Vwebsocketpp "websocketpp could not be found (client.hpp)" WEBSOCKETPP_ROOT_DIR )

