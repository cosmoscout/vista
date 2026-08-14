# Locate header.
find_path(JSW_INCLUDE_DIR jsw.h
		HINTS ${JSW_ROOT_DIR}/include)

# Locate libraries.
find_library(JSW_LIBRARY NAMES jsw
		HINTS ${JSW_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JSW DEFAULT_MSG JSW_INCLUDE_DIR JSW_LIBRARY)

# Add imported target.
if (JSW_FOUND)
	set(JSW_INCLUDE_DIRS "${JSW_INCLUDE_DIR}")

	if(NOT JSW_FIND_QUIETLY)
		message(STATUS "JSW_INCLUDE_DIRS ................ ${JSW_INCLUDE_DIR}")
		message(STATUS "JSW_LIBRARY ..................... ${JSW_LIBRARY}")
	endif()

	if(NOT TARGET JSW::JSW)
		add_library(JSW::JSW UNKNOWN IMPORTED)
		set_target_properties(JSW::JSW PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${JSW_INCLUDE_DIRS}")

		set_property(TARGET JSW::JSW APPEND PROPERTY
				IMPORTED_LOCATION "${JSW_LIBRARY}")
	endif()
endif ()