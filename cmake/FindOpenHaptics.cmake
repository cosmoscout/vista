# Locate header.
find_path(OPENHAPTICS_INCLUDE_DIR hdu.h
		HINTS ${OPENHAPTICS_ROOT_DIR}/utilities/include/HDU)

# Locate libraries.
find_library(OPENHAPTICS_HD_LIBRARY NAMES HD
		HINTS ${OPENHAPTICS_ROOT_DIR}/lib/x64)

find_library(OPENHAPTICS_HDU_LIBRARY NAMES HDU
		HINTS ${OPENHAPTICS_ROOT_DIR}/utilities/lib/x64/Release)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenHaptics DEFAULT_MSG OPENHAPTICS_INCLUDE_DIR OPENHAPTICS_HD_LIBRARY OPENHAPTICS_HDU_LIBRARY)

# Add imported target.
if (OPENHAPTICS_FOUND)
	set(OPENHAPTICS_INCLUDE_DIRS "${OPENHAPTICS_ROOT_DIR}/include" "${OPENHAPTICS_ROOT_DIR}/utilities/include")

	if(NOT OPENHAPTICS_FIND_QUIETLY)
		message(STATUS "OPENHAPTICS_INCLUDE_DIRS ........ ${OPENHAPTICS_INCLUDE_DIR}")
		message(STATUS "OPENHAPTICS_HD_LIBRARY .......... ${OPENHAPTICS_HD_LIBRARY}")
		message(STATUS "OPENHAPTICS_HDU_LIBRARY ......... ${OPENHAPTICS_HDU_LIBRARY}")
	endif()

	if(NOT TARGET OpenHaptics::HD)
		add_library(OpenHaptics::HD UNKNOWN IMPORTED)
		set_target_properties(OpenHaptics::HD PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${OPENHAPTICS_INCLUDE_DIRS}")

		set_property(TARGET OpenHaptics::HD APPEND PROPERTY
				IMPORTED_LOCATION "${OPENHAPTICS_HD_LIBRARY}")
	endif()

	if(NOT TARGET OpenHaptics::HDU)
		add_library(OpenHaptics::HDU UNKNOWN IMPORTED)
		set_target_properties(OpenHaptics::HDU UNKNOWN PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${OPENHAPTICS_INCLUDE_DIRS}")

		set_target_properties(OpenHaptics::HDU PROPERTIES
				IMPORTED_LOCATION "${OPENHAPTICS_HDU_LIBRARY}")
	endif()
endif ()