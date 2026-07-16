

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

if (VISTA_USE_PRECOMPILED_HEADERS)
	target_precompile_headers(VistaMath PRIVATE precompiled.pch)
endif()

vista_configure_lib( VistaMath )
vista_install( VistaMath VistaMath )