

vista_adopt_version( VistaAspects VistaCoreLibs )

# set our dependencies to other corelibs
set( VISTAASPECTS_INTERNAL_DEPENDENCIES 
				VistaBase )
set( VISTAASPECTS_INTERNAL_DEPENDENCIES ${VISTAASPECTS_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

if( UNIX AND NOT APPLE )
	set( LIBRARIES rt )
endif( UNIX AND NOT APPLE )

include( "./_SourceFiles.cmake" )

add_library( VistaAspects ${ProjectSources} )
target_link_libraries( VistaAspects
	${VISTAASPECTS_INTERNAL_DEPENDENCIES}
	${LIBRARIES}
)

vista_configure_lib( VistaAspects )
vista_install( VistaAspects VistaAspects )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaAspects )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaAspects )
