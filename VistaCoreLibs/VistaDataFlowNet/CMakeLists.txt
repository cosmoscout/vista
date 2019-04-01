

vista_adopt_version( VistaDataFlowNet VistaCoreLibs )

# set our dependencies to other corelibs
set( VISTADATAFLOWNET_INTERNAL_DEPENDENCIES 
			VistaBase
			VistaAspects
			VistaInterProcComm
			VistaTools
			VistaMath
			VistaDeviceDriversBase )
set( VISTADATAFLOWNET_INTERNAL_DEPENDENCIES ${VISTADATAFLOWNET_INTERNAL_DEPENDENCIES} PARENT_SCOPE )
set( VISTADATAFLOWNET_TRANSMIT_INCREMENTAL_HISTORIES FALSE CACHE BOOL "experimental incemental history transmission" )
if( VISTADATAFLOWNET_TRANSMIT_INCREMENTAL_HISTORIES )
	add_definitions( -DVISTA_TRANSMIT_INCREMENTAL_HISTORIES )
endif()

include( "./_SourceFiles.cmake" )

add_library( VistaDataFlowNet ${ProjectSources} )
target_link_libraries( VistaDataFlowNet
	${VISTADATAFLOWNET_INTERNAL_DEPENDENCIES}
)

vista_configure_lib( VistaDataFlowNet )
vista_install( VistaDataFlowNet VistaDataFlowNet )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaDataFlowNet )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaDataFlowNet )
