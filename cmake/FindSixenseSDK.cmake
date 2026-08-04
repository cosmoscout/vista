# Locate header.
find_path(SIXENSESDK_INCLUDE_DIR sixensesdk.h
    HINTS ${SIXENSESDK_ROOT_DIR}/include)

# Locate libraries.
find_library(SIXENSESDK_LIBRARY NAMES sixense_x64 sixensed_x64
  HINTS
    ${SIXENSESDK_ROOT_DIR}/lib/x64/debug_dll
    ${SIXENSESDK_ROOT_DIR}/lib/x64/release_dll
    ${SIXENSESDK_ROOT_DIR}/bin/x64/debug_dll
    ${SIXENSESDK_ROOT_DIR}/bin/x64/release_dll
    ${SIXENSESDK_ROOT_DIR}/lib/linux_x64/release
    ${SIXENSESDK_ROOT_DIR}/lib/linux_x64/debug
)

find_library(SIXENSESDK_UTILS_LIBRARY NAMES sixense_utils_x64 sixense_utilsd_x64
  HINTS
    ${SIXENSESDK_ROOT_DIR}/lib/x64/debug_dll
    ${SIXENSESDK_ROOT_DIR}/lib/x64/release_dll
    ${SIXENSESDK_ROOT_DIR}/bin/x64/debug_dll
    ${SIXENSESDK_ROOT_DIR}/bin/x64/release_dll
    ${SIXENSESDK_ROOT_DIR}/lib/linux_x64/release
    ${SIXENSESDK_ROOT_DIR}/lib/linux_x64/debug
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SixenseSDK DEFAULT_MSG SIXENSESDK_INCLUDE_DIR SIXENSESDK_LIBRARY)

# Add imported target.
if (SIXENSESDK_FOUND)
  set(SIXENSESDK_INCLUDE_DIRS "${SIXENSESDK_INCLUDE_DIR}")

  if(NOT SIXENSESDK_FIND_QUIETLY)
    message(STATUS "SIXENSESDK_INCLUDE_DIRS ............ ${SIXENSESDK_INCLUDE_DIR}")
    message(STATUS "SIXENSESDK_LIBRARY ................. ${SIXENSESDK_LIBRARY}")
    message(STATUS "SIXENSESDK_UTILS_LIBRARY ........... ${SIXENSESDK_UTILS_LIBRARY}")
  endif()

  if(NOT TARGET Sixense::SDK)
    add_library(Sixense::SDK UNKNOWN IMPORTED)
    set_target_properties(Sixense::SDK PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SIXENSESDK_INCLUDE_DIRS}")

    set_property(TARGET Sixense::SDK APPEND PROPERTY
        IMPORTED_LOCATION "${SIXENSESDK_LIBRARY}")
  endif()

  if(NOT TARGET Sixense::Utils)
    add_library(Sixense::Utils UNKNOWN IMPORTED)
    set_target_properties(Sixense::Utils PROPERTIES
        IMPORTED_LOCATION "${SIXENSESDK_UTILS_LIBRARY}")
  endif()
endif ()