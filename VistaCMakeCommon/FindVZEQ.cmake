
include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VZEQ_FOUND )

	vista_find_package_root( ZEQ include/zeq/zeq.h NAMES zeq Zeq )

	if( ZEQ_ROOT_DIR )
		set( ZEQ_INCLUDE_DIRS "${ZEQ_ROOT_DIR}/include" )
		
		if( UNIX )
			message( "you are doomed!" )
			
		elseif(WIN32)
			set( ZEQ_LIBRARY_DIRS "${ZEQ_ROOT_DIR}/lib" "${ZEQ_ROOT_DIR}/bin" )
			set( ZEQ_LIBRARIES zeq )
			
		endif()
			
	endif()
	
	vista_add_package_dependency( ZEQ Boost REQUIRED FIND_DEPENDENCIES)
	vista_add_package_dependency( ZEQ zeromq REQUIRED FIND_DEPENDENCIES)
	vista_add_package_dependency( ZEQ Lunchbox REQUIRED FIND_DEPENDENCIES)
	
endif()

find_package_handle_standard_args( VZEQ "ZEQ could not be found" ZEQ_ROOT_DIR )

