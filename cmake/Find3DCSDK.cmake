# Locate header.
find_path(3DCSDK_INCLUDE_DIR si.h
    HINTS ${3DCSDK_ROOT_DIR}/Inc)

# Locate libraries.
find_library(3DCSDK_LIBRARY NAMES siapp
    HINTS ${3DCSDK_ROOT_DIR}/Lib/x64)

find_library(3DCSDK_MATH_LIBRARY NAMES spwmath spwmathD
    HINTS ${3DCSDK_ROOT_DIR}/Lib/x64)

find_library(3DCSDK_MATH_MT_LIBRARY NAMES spwmathMT spwmathMTD
    HINTS ${3DCSDK_ROOT_DIR}/Lib/x64)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(3DCSDK DEFAULT_MSG 3DCSDK_INCLUDE_DIR 3DCSDK_LIBRARY)

# Add imported target.
if (3DCSDK_FOUND)
  set(3DCSDK_INCLUDE_DIRS "${3DCSDK_INCLUDE_DIR}")

  if(NOT 3DCSDK_FIND_QUIETLY)
    message(STATUS "3DCSDK_INCLUDE_DIRS ............. ${3DCSDK_INCLUDE_DIR}")
    message(STATUS "3DCSDK_LIBRARY .................. ${3DCSDK_LIBRARY}")
    message(STATUS "3DCSDK_MATH_LIBRARY ............. ${3DCSDK_MATH_LIBRARY}")
    message(STATUS "3DCSDK_MATH_MT_LIBRARY .......... ${3DCSDK_MATH_MT_LIBRARY}")
  endif()

  if(NOT TARGET 3DCSDK::Base)
    add_library(3DCSDK::Base UNKNOWN IMPORTED)
    set_target_properties(3DCSDK::Base PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${3DCSDK_INCLUDE_DIRS}")

    set_property(TARGET 3DCSDK::Base APPEND PROPERTY
        IMPORTED_LOCATION "${3DCSDK_LIBRARY}")
  endif()

  if(NOT TARGET 3DCSDK::Math)
    add_library(3DCSDK::Math UNKNOWN IMPORTED)
    set_target_properties(3DCSDK::Math PROPERTIES
        IMPORTED_LOCATION "${3DCSDK_MATH_LIBRARY}")
  endif()

  if(NOT TARGET 3DCSDK::MathMT)
    add_library(3DCSDK::MathMT UNKNOWN IMPORTED)
    set_target_properties(3DCSDK::MathMT PROPERTIES
        IMPORTED_LOCATION "${3DCSDK_MATH_MT_LIBRARY}")
  endif()
endif ()