# Locate header.
find_path(WIIUSE_INCLUDE_DIR wiiuse.h
		HINTS ${WIIUSE_ROOT_DIR}/include)

# Locate libraries.
find_library(WIIUSE_LIBRARY NAMES wiiuse
		HINTS ${WIIUSE_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wiiuse DEFAULT_MSG WIIUSE_INCLUDE_DIR WIIUSE_LIBRARY)

# Add imported target.
if (WIIUSE_FOUND)
	set(WIIUSE_INCLUDE_DIRS "${WIIUSE_INCLUDE_DIR}")

	if(NOT WIIUSE_FIND_QUIETLY)
		message(STATUS "WIIUSE_INCLUDE_DIRS ............. ${WIIUSE_INCLUDE_DIR}")
		message(STATUS "WIIUSE_LIBRARY .................. ${WIIUSE_LIBRARY}")
	endif()

	if(NOT TARGET wiiuse::wiiuse)
		add_library(wiiuse::wiiuse UNKNOWN IMPORTED)
		set_target_properties(wiiuse::wiiuse PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${WIIUSE_INCLUDE_DIRS}")

		set_property(TARGET wiiuse::wiiuse APPEND PROPERTY
				IMPORTED_LOCATION "${WIIUSE_LIBRARY}")
	endif()
endif ()