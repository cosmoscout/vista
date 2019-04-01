# $Id: FindVASIO.cmake 21495 2016-08-17 07:52:18Z js908001 $

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VASIO_FOUND )
	vista_find_package_root( ASIO "include/common/asio.h" )	

	if( ASIO_ROOT_DIR )
		set( ASIO_INCLUDE_DIRS "${ASIO_ROOT_DIR}/include" "${ASIO_ROOT_DIR}/include/host" )
        set( ASIO_LIBRARY_DIRS "${ASIO_ROOT_DIR}/lib" "${ASIO_ROOT_DIR}/bin" )
		if( WIN32 )
			list( APPEND ASIO_INCLUDE_DIRS "${ASIO_ROOT_DIR}/include/host/pc" )
			set( ASIO_LIBRARIES "ASIO" "winmm" )
		else( WIN32 )
			set( ASIO_LIBRARIES "ASIO" )
		endif( WIN32 )
	else( ASIO_ROOT_DIR )
		message( WARNING "vista_find_package_root - File named asio.h not found" )	
	endif( ASIO_ROOT_DIR )
endif( NOT VASIO_FOUND )

find_package_handle_standard_args( VASIO "ASIO SDK could not be found" ASIO_ROOT_DIR )

