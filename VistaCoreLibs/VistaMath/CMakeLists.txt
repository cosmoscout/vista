

vista_adopt_version( VistaMath VistaCoreLibs )

set( VISTAMATH_INTERNAL_DEPENDENCIES 
			VistaBase )
set( VISTAMATH_INTERNAL_DEPENDENCIES ${VISTAMATH_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAMATH_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAMATH_STATIC )
	endif( BUILD_SHARED_LIBS )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaMath ${ProjectSources} )
target_link_libraries( VistaMath
	${VISTAMATH_INTERNAL_DEPENDENCIES}
)

if (${CMAKE_VERSION} GREATER_EQUAL 3.16)
	target_precompile_headers(VistaMath PRIVATE precompiled.pch)
endif()

vista_configure_lib( VistaMath )
vista_install( VistaMath VistaMath )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaMath )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaMath )