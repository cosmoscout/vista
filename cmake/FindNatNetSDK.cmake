# Locate header.
find_path(NATNETSDK_INCLUDE_DIR NatNetTypes.h
    HINTS ${NATNETSDK_ROOT_DIR}/include)

# Locate libraries.
find_library(NATNETSDK_LIBRARY NAMES NatNetLib
    HINTS ${NATNETSDK_ROOT_DIR}/lib/x64)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NatNetSDK DEFAULT_MSG NATNETSDK_INCLUDE_DIR NATNETSDK_LIBRARY)

if (NATNETSDK_FOUND)
  set(NATNETSDK_INCLUDE_DIRS "${NATNETSDK_INCLUDE_DIR}")

  if (NOT NATNETSDK_FIND_QUIETLY)
    message(STATUS "NATNETSDK_INCLUDE_DIRS .......... ${NATNETSDK_INCLUDE_DIR}")
    message(STATUS "NATNETSDK_LIBRARY ............... ${NATNETSDK_LIBRARY}")
  endif ()

  if (NOT TARGET NatNet::SDK)
    add_library(NatNet::SDK UNKNOWN IMPORTED)
    set_target_properties(NatNet::SDK PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${NATNETSDK_INCLUDE_DIRS}")

    set_property(TARGET NatNet::SDK APPEND PROPERTY
        IMPORTED_LOCATION "${NATNETSDK_LIBRARY}")
  endif ()
endif ()