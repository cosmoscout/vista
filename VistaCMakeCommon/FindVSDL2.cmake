include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VSDL2_FOUND )
	vista_find_package_root( SDL2 include/SDL2/SDL.h )

	if( SDL2_ROOT_DIR )

		set( SDL2_INCLUDE_DIRS "${SDL2_ROOT_DIR}/include" )
		message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")

		if( WIN32 )
			find_library( SDL2_LIBRARY_RELEASE
					NAMES SDL2 SDL2.lib
					PATHS
					"${SDL2_ROOT_DIR}/lib"
					"${SDL2_ROOT_DIR}/lib/x64"
					"${SDL2_ROOT_DIR}/lib/win64"
					"${SDL2_ROOT_DIR}/lib/Win64"
					"${SDL2_ROOT_DIR}/lib/x86"
					"${SDL2_ROOT_DIR}/lib/win32"
					"${SDL2_ROOT_DIR}/lib/Win32"
					NO_DEFAULT_PATH
			)

			find_library( SDL2MAIN_LIBRARY_RELEASE
					NAMES SDL2main SDL2main.lib
					PATHS
					"${SDL2_ROOT_DIR}/lib"
					"${SDL2_ROOT_DIR}/lib/x64"
					"${SDL2_ROOT_DIR}/lib/win64"
					"${SDL2_ROOT_DIR}/lib/Win64"
					"${SDL2_ROOT_DIR}/lib/x86"
					"${SDL2_ROOT_DIR}/lib/win32"
					"${SDL2_ROOT_DIR}/lib/Win32"
					NO_DEFAULT_PATH
			)

			find_library( SDL2_LIBRARY_DEBUG
					NAMES SDL2d SDL2d.lib SDL2 SDL2.lib
					PATHS
					"${SDL2_ROOT_DIR}/lib"
					"${SDL2_ROOT_DIR}/lib/x64"
					"${SDL2_ROOT_DIR}/lib/win64"
					"${SDL2_ROOT_DIR}/lib/Win64"
					"${SDL2_ROOT_DIR}/lib/x86"
					"${SDL2_ROOT_DIR}/lib/win32"
					"${SDL2_ROOT_DIR}/lib/Win32"
					NO_DEFAULT_PATH
			)

			find_library( SDL2MAIN_LIBRARY_DEBUG
					NAMES SDL2maind SDL2maind.lib SDL2main SDL2main.lib
					PATHS
					"${SDL2_ROOT_DIR}/lib"
					"${SDL2_ROOT_DIR}/lib/x64"
					"${SDL2_ROOT_DIR}/lib/win64"
					"${SDL2_ROOT_DIR}/lib/Win64"
					"${SDL2_ROOT_DIR}/lib/x86"
					"${SDL2_ROOT_DIR}/lib/win32"
					"${SDL2_ROOT_DIR}/lib/Win32"
					NO_DEFAULT_PATH
			)

			set( SDL2_LIBRARIES
					optimized "${SDL2_LIBRARY_RELEASE}"
					optimized "${SDL2MAIN_LIBRARY_RELEASE}"
					debug "${SDL2_LIBRARY_DEBUG}"
					debug "${SDL2MAIN_LIBRARY_DEBUG}"
			)

			get_filename_component( SDL2_LIBRARY_DIRS "${SDL2_LIBRARY_RELEASE}" DIRECTORY )
		else()
			find_library( SDL2_LIBRARY_RELEASE
					NAMES SDL2 libSDL2 SDL2.so libSDL2.so
					PATHS "${SDL2_ROOT_DIR}/lib"
					NO_DEFAULT_PATH
			)

			find_library( SDL2MAIN_LIBRARY_RELEASE
					NAMES SDL2main libSDL2main SDL2main.so libSDL2main.so
					PATHS "${SDL2_ROOT_DIR}/lib"
					NO_DEFAULT_PATH
			)

			find_library( SDL2_LIBRARY_DEBUG
					NAMES SDL2d libSDL2d SDL2 libSDL2 SDL2d.so libSDL2d.so SDL2.so libSDL2.so
					PATHS "${SDL2_ROOT_DIR}/lib"
					NO_DEFAULT_PATH
			)

			find_library( SDL2MAIN_LIBRARY_DEBUG
					NAMES SDL2maind libSDL2maind SDL2main libSDL2main SDL2maind.so libSDL2maind.so SDL2main.so libSDL2main.so
					PATHS "${SDL2_ROOT_DIR}/lib"
					NO_DEFAULT_PATH
			)

			set( SDL2_LIBRARIES
					optimized "${SDL2_LIBRARY_RELEASE}"
					optimized "${SDL2MAIN_LIBRARY_RELEASE}"
					debug "${SDL2_LIBRARY_DEBUG}"
					debug "${SDL2MAIN_LIBRARY_DEBUG}"
			)

			get_filename_component( SDL2_LIBRARY_DIRS "${SDL2_LIBRARY_RELEASE}" DIRECTORY )
		endif()
	endif()
endif()

find_package_handle_standard_args( VSDL2 "SDL2 could not be found" SDL2_ROOT_DIR SDL2_INCLUDE_DIRS SDL2_LIBRARY_RELEASE SDL2MAIN_LIBRARY_RELEASE SDL2_LIBRARY_DEBUG SDL2MAIN_LIBRARY_DEBUG )