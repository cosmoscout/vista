include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VSDL2_FOUND )
	vista_find_package_root( SDL2 include/SDL2/SDL.h )

	if( SDL2_ROOT_DIR )
	
		set( SDL2_INCLUDE_DIRS "${SDL2_ROOT_DIR}/include" )
		message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")

		if( WIN32 )
			set( SDL2_LIBRARY_DIRS "${SDL2_ROOT_DIR}/lib" )
			set( SDL2_LIBRARIES SDL2.lib SDL2main.lib )
		else()
			set( SDL2_LIBRARY_DIRS "${SDL2_ROOT_DIR}/lib" )
			set( SDL2_LIBRARIES SDL2.so SDL2main.so )
		endif()
	endif()
endif()

find_package_handle_standard_args( VSDL2 "SDL2 could not be found" SDL2_ROOT_DIR SDL2_LIBRARIES )
