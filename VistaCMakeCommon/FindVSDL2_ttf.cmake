include(FindPackageHandleStandardArgs)
include(VistaFindUtils)

if (NOT VSDL2_TTF_FOUND)
    vista_find_package_root(SDL2_ttf include/SDL2/SDL_ttf.h)

    if (SDL2_TTF_ROOT_DIR)

        set(SDL2_TTF_INCLUDE_DIRS "${SDL2_TTF_ROOT_DIR}/include")
        message(STATUS "SDL2_TTF_INCLUDE_DIRS: ${SDL2_TTF_INCLUDE_DIRS}")

        if (WIN32)
            find_library(SDL2_TTF_LIBRARY_RELEASE
                    NAMES SDL2_ttf SDL2_ttf.lib
                    PATHS
                    "${SDL2_TTF_ROOT_DIR}/lib"
                    "${SDL2_TTF_ROOT_DIR}/lib/x64"
                    "${SDL2_TTF_ROOT_DIR}/lib/win64"
                    "${SDL2_TTF_ROOT_DIR}/lib/Win64"
                    "${SDL2_TTF_ROOT_DIR}/lib/x86"
                    "${SDL2_TTF_ROOT_DIR}/lib/win32"
                    "${SDL2_TTF_ROOT_DIR}/lib/Win32"
                    NO_DEFAULT_PATH
            )

            find_library(SDL2_TTF_LIBRARY_DEBUG
                    NAMES SDL2_ttfd SDL2_ttfd.lib SDL2_ttf SDL2_ttf.lib
                    PATHS
                    "${SDL2_TTF_ROOT_DIR}/lib"
                    "${SDL2_TTF_ROOT_DIR}/lib/x64"
                    "${SDL2_TTF_ROOT_DIR}/lib/win64"
                    "${SDL2_TTF_ROOT_DIR}/lib/Win64"
                    "${SDL2_TTF_ROOT_DIR}/lib/x86"
                    "${SDL2_TTF_ROOT_DIR}/lib/win32"
                    "${SDL2_TTF_ROOT_DIR}/lib/Win32"
                    NO_DEFAULT_PATH
            )

            set(SDL2_TTF_LIBRARIES
                    optimized "${SDL2_TTF_LIBRARY_RELEASE}"
                    debug "${SDL2_TTF_LIBRARY_DEBUG}"
            )

            get_filename_component(SDL2_TTF_LIBRARY_DIRS "${SDL2_TTF_LIBRARY_RELEASE}" DIRECTORY)
        else ()
            find_library(SDL2_TTF_LIBRARY_RELEASE
                    NAMES SDL2_ttf libSDL2_ttf SDL2_ttf.so libSDL2_ttf.so
                    PATHS "${SDL2_TTF_ROOT_DIR}/lib"
                    NO_DEFAULT_PATH
            )

            find_library(SDL2_TTF_LIBRARY_DEBUG
                    NAMES SDL2_ttfd libSDL2_ttfd SDL2_ttf libSDL2_ttf SDL2_ttfd.so libSDL2_ttfd.so SDL2_ttf.so libSDL2_ttf.so
                    PATHS "${SDL2_TTF_ROOT_DIR}/lib"
                    NO_DEFAULT_PATH
            )

            set(SDL2_TTF_LIBRARIES
                    optimized "${SDL2_TTF_LIBRARY_RELEASE}"
                    debug "${SDL2_TTF_LIBRARY_DEBUG}"
            )

            get_filename_component(SDL2_TTF_LIBRARY_DIRS "${SDL2_TTF_LIBRARY_RELEASE}" DIRECTORY)
        endif ()
    endif ()
endif ()

find_package_handle_standard_args(VSDL2_ttf "SDL2_TTF could not be found" SDL2_TTF_ROOT_DIR SDL2_TTF_INCLUDE_DIRS SDL2_TTF_LIBRARY_RELEASE SDL2_TTF_LIBRARY_DEBUG)