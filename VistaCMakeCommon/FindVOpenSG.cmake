include(FindPackageHandleStandardArgs)
include(VistaFindUtils)

if (NOT VOPENSG_FOUND)
    vista_find_package_root(OpenSG include/OpenSG/OSGAction.h)

    if (OPENSG_ROOT_DIR)
        set(OPENSG_INCLUDE_DIRS ${OPENSG_ROOT_DIR}/include ${OPENSG_ROOT_DIR}/include/OpenSG)

        if (WIN32)
            set(OPENSG_LIBRARY_SEARCH_DIRS
                    "${OPENSG_ROOT_DIR}/lib"
                    "${OPENSG_ROOT_DIR}/lib/x64"
                    "${OPENSG_ROOT_DIR}/lib/win64"
                    "${OPENSG_ROOT_DIR}/lib/Win64"
                    "${OPENSG_ROOT_DIR}/lib/x86"
                    "${OPENSG_ROOT_DIR}/lib/win32"
                    "${OPENSG_ROOT_DIR}/lib/Win32"
                    "${OPENSG_ROOT_DIR}/lib64"
            )

            find_library(OPENSG_SYSTEM_LIBRARY_RELEASE
                    NAMES OSGSystem OSGSystem.lib
                    PATHS ${OPENSG_LIBRARY_SEARCH_DIRS}
                    NO_DEFAULT_PATH
            )

            find_library(OPENSG_BASE_LIBRARY_RELEASE
                    NAMES OSGBase OSGBase.lib
                    PATHS ${OPENSG_LIBRARY_SEARCH_DIRS}
                    NO_DEFAULT_PATH
            )

            find_library(OPENSG_SYSTEM_LIBRARY_DEBUG
                    NAMES OSGSystemD OSGSystemD.lib OSGSystem OSGSystem.lib
                    PATHS ${OPENSG_LIBRARY_SEARCH_DIRS}
                    NO_DEFAULT_PATH
            )

            find_library(OPENSG_BASE_LIBRARY_DEBUG
                    NAMES OSGBaseD OSGBaseD.lib OSGBase OSGBase.lib
                    PATHS ${OPENSG_LIBRARY_SEARCH_DIRS}
                    NO_DEFAULT_PATH
            )
        else ()
            set(OPENSG_LIBRARY_SEARCH_DIRS
                    "${OPENSG_ROOT_DIR}/lib"
            )

            find_library(OPENSG_SYSTEM_LIBRARY_RELEASE
                    NAMES OSGSystem libOSGSystem.so
                    PATHS ${OPENSG_LIBRARY_SEARCH_DIRS}
                    NO_DEFAULT_PATH
            )

            find_library(OPENSG_BASE_LIBRARY_RELEASE
                    NAMES OSGBase libOSGBase.so
                    PATHS ${OPENSG_LIBRARY_SEARCH_DIRS}
                    NO_DEFAULT_PATH
            )

            find_library(OPENSG_SYSTEM_LIBRARY_DEBUG
                    NAMES OSGSystemD OSGSystem libOSGSystemD.so libOSGSystem.so
                    PATHS ${OPENSG_LIBRARY_SEARCH_DIRS}
                    NO_DEFAULT_PATH
            )

            find_library(OPENSG_BASE_LIBRARY_DEBUG
                    NAMES OSGBaseD OSGBase libOSGBaseD.so libOSGBase.so
                    PATHS ${OPENSG_LIBRARY_SEARCH_DIRS}
                    NO_DEFAULT_PATH
            )
        endif ()

        set(OPENSG_LIBRARIES
                optimized "${OPENSG_SYSTEM_LIBRARY_RELEASE}"
                optimized "${OPENSG_BASE_LIBRARY_RELEASE}"
                debug "${OPENSG_SYSTEM_LIBRARY_DEBUG}"
                debug "${OPENSG_BASE_LIBRARY_DEBUG}"
        )

        set(OPENSG_LIBRARY_DIRS)
        foreach (_OPENSG_LIBRARY
                "${OPENSG_SYSTEM_LIBRARY_RELEASE}"
                "${OPENSG_BASE_LIBRARY_RELEASE}"
                "${OPENSG_SYSTEM_LIBRARY_DEBUG}"
                "${OPENSG_BASE_LIBRARY_DEBUG}"
        )
            if (_OPENSG_LIBRARY)
                get_filename_component(_OPENSG_LIBRARY_DIR "${_OPENSG_LIBRARY}" DIRECTORY)
                list(APPEND OPENSG_LIBRARY_DIRS "${_OPENSG_LIBRARY_DIR}")
            endif ()
        endforeach ()

        if (OPENSG_LIBRARY_DIRS)
            list(REMOVE_DUPLICATES OPENSG_LIBRARY_DIRS)
        endif ()

        set(OPENSG_DEFINITIONS -DOSG_WITH_GIF -DOSG_WITH_TIF -DOSG_WITH_JPG -DOSG_BUILD_DLL -D_OSG_HAVE_CONFIGURED_H_ -DFT2_LIB -DFONTCONFIG_LIB)

        if (UNIX)
            string(REGEX MATCH ".*icpc" _IS_ICPC_COMPILER ${CMAKE_CXX_COMPILER})
            if (_IS_ICPC_COMPILER)
                set(OPENSG_DEFINITIONS ${OPENSG_DEFINITIONS} -DOSG_ICC_GNU_COMPAT)
            endif ()
        endif ()
    endif ()
endif ()

find_package_handle_standard_args(VOpenSG "OpenSG could not be found"
        OPENSG_ROOT_DIR
        OPENSG_INCLUDE_DIRS
        OPENSG_SYSTEM_LIBRARY_RELEASE
        OPENSG_BASE_LIBRARY_RELEASE
        OPENSG_SYSTEM_LIBRARY_DEBUG
        OPENSG_BASE_LIBRARY_DEBUG
)