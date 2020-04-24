

vista_adopt_version( VistaTools VistaCoreLibs )

set( VISTATOOLS_INTERNAL_DEPENDENCIES 
			VistaBase
			VistaAspects
			VistaInterProcComm
			VistaMath )
set( VISTATOOLS_INTERNAL_DEPENDENCIES ${VISTATOOLS_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTATOOLS_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTATOOLS_STATIC )
	endif( BUILD_SHARED_LIBS )
	set( LIBRARIES PowrProf dbghelp Psapi )
else()
	set( LIBRARIES dl )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaTools ${ProjectSources} )
target_link_libraries( VistaTools
	${VISTATOOLS_INTERNAL_DEPENDENCIES}
	${LIBRARIES}
)

if (VISTA_USE_PRECOMPILED_HEADERS)
	target_precompile_headers(VistaTools PRIVATE precompiled.pch)
endif()

vista_configure_lib( VistaTools )
vista_install( VistaTools VistaTools )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaTools )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaTools )
