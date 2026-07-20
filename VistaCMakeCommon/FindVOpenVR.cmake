include(FindPackageHandleStandardArgs)
include(VistaFindUtils)

if (NOT VOPENVR_FOUND)
    vista_find_package_root(OPENVR include/openvr/openvr.h)

    if (OPENVR_ROOT_DIR)

        set(OPENVR_INCLUDE_DIRS "${OPENVR_ROOT_DIR}/include")
        message(STATUS "OPENVR_INCLUDE_DIRS: ${OPENVR_INCLUDE_DIRS}")

        if (WIN32)
            find_library(OPENVR_LIBRARIES
                    NAMES openvr_api openvr_api.lib
                    PATHS
                    "${OPENVR_ROOT_DIR}/lib"
                    "${OPENVR_ROOT_DIR}/lib/win64"
                    "${OPENVR_ROOT_DIR}/lib/win32"
                    NO_DEFAULT_PATH
            )

            get_filename_component(OPENVR_LIBRARY_DIRS "${OPENVR_LIBRARIES}" DIRECTORY)
        else ()
            find_library(OPENVR_LIBRARIES
                    NAMES openvr_api libopenvr_api.so
                    PATHS "${OPENVR_ROOT_DIR}/lib"
                    NO_DEFAULT_PATH
            )

            get_filename_component(OPENVR_LIBRARY_DIRS "${OPENVR_LIBRARIES}" DIRECTORY)
        endif ()
    endif ()
endif ()

find_package_handle_standard_args(VOpenVR "OPENVR could not be found" OPENVR_ROOT_DIR OPENVR_INCLUDE_DIRS OPENVR_LIBRARIES)