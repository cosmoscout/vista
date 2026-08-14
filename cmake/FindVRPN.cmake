# Locate header.
find_path(VRPN_INCLUDE_DIR vrpn_Connection.h
		HINTS ${VRPN_ROOT_DIR}/include)

# Locate libraries.
find_library(VRPN_LIBRARY NAMES vrpn
		HINTS ${VRPN_ROOT_DIR}/lib)

find_library(VRPN_SERVER_LIBRARY NAMES vrpnserver
		HINTS ${VRPN_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VRPN DEFAULT_MSG VRPN_INCLUDE_DIR VRPN_LIBRARY VRPN_SERVER_LIBRARY)

# Add imported target.
if (VRPN_FOUND)
	set(VRPN_INCLUDE_DIRS "${VRPN_INCLUDE_DIR}")

	if(NOT VRPN_FIND_QUIETLY)
		message(STATUS "VRPN_INCLUDE_DIRS ............... ${VRPN_INCLUDE_DIR}")
		message(STATUS "VRPN_LIBRARY .................... ${VRPN_LIBRARY}")
		message(STATUS "VRPN_SERVER_LIBRARY ............. ${VRPN_SERVER_LIBRARY}")
	endif()

	if(NOT TARGET VRPN::vrpn)
		add_library(VRPN::vrpn UNKNOWN IMPORTED)
		set_target_properties(VRPN::vrpn PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${VRPN_INCLUDE_DIRS}")

		set_property(TARGET VRPN::vrpn APPEND PROPERTY
				IMPORTED_LOCATION "${VRPN_LIBRARY}")
	endif()

	if(NOT TARGET VRPN::Server)
		add_library(VRPN::Server UNKNOWN IMPORTED)
		set_target_properties(VRPN::Server PROPERTIES
				IMPORTED_LOCATION "${VRPN_SERVER_LIBRARY}")
	endif()
endif ()