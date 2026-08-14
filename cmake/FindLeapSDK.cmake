# Locate header.
find_path(LEAPSDK_INCLUDE_DIR Leap.h
		HINTS ${LEAPSDK_ROOT_DIR}/include)

# Locate libraries.
find_library(LEAPSDK_LIBRARY NAMES libLeap Leap
		HINTS ${LEAPSDK_ROOT_DIR}/lib/x64)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LEAPSDK DEFAULT_MSG LEAPSDK_INCLUDE_DIR LEAPSDK_LIBRARY)

# Add imported target.
if (LEAPSDK_FOUND)
	set(LEAPSDK_INCLUDE_DIRS "${LEAPSDK_INCLUDE_DIR}")

	if(NOT LEAPSDK_FIND_QUIETLY)
		message(STATUS "LEAPSDK_INCLUDE_DIRS ............ ${LEAPSDK_INCLUDE_DIR}")
		message(STATUS "LEAPSDK_LIBRARY ................. ${LEAPSDK_LIBRARY}")
	endif()

	if(NOT TARGET Leap::SDK)
		add_library(Leap::SDK UNKNOWN IMPORTED)
		set_target_properties(Leap::SDK PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${LEAPSDK_INCLUDE_DIRS}")

		set_property(TARGET Leap::SDK APPEND PROPERTY
				IMPORTED_LOCATION "${LEAPSDK_LIBRARY}")
	endif()
endif ()