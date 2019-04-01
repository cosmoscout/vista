

vista_adopt_version( VistaBase VistaCoreLibs )

# set our dependencies to other corelibs
set( VISTABASE_INTERNAL_DEPENDENCIES )
set( VISTABASE_INTERNAL_DEPENDENCIES ${VISTABASE_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

if( UNIX AND NOT APPLE )
	set( LIBRARIES rt )
endif( UNIX AND NOT APPLE )


include( "./_SourceFiles.cmake" )

add_library( VistaBase ${ProjectSources} )
target_link_libraries( VistaBase
	${LIBRARIES}
)

vista_configure_lib( VistaBase )
vista_install( VistaBase VistaBase )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaBase )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaBase )
