

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

if (VISTA_USE_PRECOMPILED_HEADERS)
	target_precompile_headers(VistaDataFlowNet PRIVATE precompiled.pch)
endif()

vista_configure_lib( VistaDataFlowNet )
vista_install( VistaDataFlowNet VistaDataFlowNet )
