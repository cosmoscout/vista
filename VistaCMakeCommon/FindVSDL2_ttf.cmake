include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VSDL2_TTF_FOUND )
	vista_find_package_root( SDL2_ttf include/SDL2/SDL_ttf.h )

	if( SDL2_TTF_ROOT_DIR )
	
		set( SDL2_TTF_INCLUDE_DIRS "${SDL2_TTF_ROOT_DIR}/include" )
		message(STATUS "SDL2_TTF_INCLUDE_DIRS: ${SDL2_TTF_INCLUDE_DIRS}")

		if( WIN32 )
			set( SDL2_TTF_LIBRARY_DIRS "${SDL2_TTF_ROOT_DIR}/lib" )
			set( SDL2_TTF_LIBRARIES
				optimized SDL2_ttf.lib
				debug SDL2_ttfd.lib
			)
		else()
			set( SDL2_TTF_LIBRARY_DIRS "${SDL2_TTF_ROOT_DIR}/lib" )
			set( SDL2_TTF_LIBRARIES
				optimized SDL2_ttf.so
				debug SDL2_ttfd.so
			)
		endif()
	endif()
endif()

find_package_handle_standard_args( VSDL2_ttf "SDL2_TTF could not be found" SDL2_TTF_ROOT_DIR SDL2_TTF_LIBRARIES )

