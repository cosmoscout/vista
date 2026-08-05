# Locate header.
find_path(OVR_INCLUDE_DIR OVR_CAPI.h
    HINTS ${OVR_ROOT_DIR}/Include)

# Locate libraries.
find_library(OVR_LIBRARY NAMES LibOVR
    HINTS ${OVR_ROOT_DIR}/Lib/Windows/x64/Release/VS2017)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OVR DEFAULT_MSG OVR_INCLUDE_DIR OVR_LIBRARY)

# Add imported target.
if (OVR_FOUND)
  set(OVR_INCLUDE_DIRS "${OVR_INCLUDE_DIR}")

  if(NOT OVR_FIND_QUIETLY)
    message(STATUS "OVR_INCLUDE_DIRS ............. ${OVR_INCLUDE_DIR}")
    message(STATUS "OVR_LIBRARY .................. ${OVR_LIBRARY}")
  endif()

  if (NOT TARGET OVR::SDK)
    add_library(OVR::SDK UNKNOWN IMPORTED)
    set_target_properties(OVR::SDK PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OVR_INCLUDE_DIRS}")

    set_property(TARGET OVR::SDK APPEND PROPERTY
        IMPORTED_LOCATION "${OVR_LIBRARY}")
  endif()
endif ()