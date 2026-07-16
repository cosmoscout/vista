file(GLOB_RECURSE ProjectSources
		"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library( VistaBase ${ProjectSources} )

if (VISTA_USE_PRECOMPILED_HEADERS)
	target_precompile_headers(VistaBase PRIVATE precompiled.pch)
endif()

vista_configure_lib( VistaBase )
vista_install( VistaBase VistaBase )
