

vista_adopt_version( VistaInterProcComm VistaCoreLibs )

set( VISTAINTERPROCCOMM_INTERNAL_DEPENDENCIES 
			VistaBase
			VistaAspects )
set( VISTAINTERPROCCOMM_INTERNAL_DEPENDENCIES ${VISTAINTERPROCCOMM_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

if( WIN32 )
	add_definitions( -D_USE_WIN32_MUTEX )
	set( LIBRARIES winmm ws2_32 )
elseif( UNIX )
	set( LIBRARIES pthread )
endif( WIN32 )

set( VISTACORELIBS_IPC_USE_EXCEPTIONS ON CACHE BOOL "Specifies wether or nor the IPC throws exceptions when errors occur" )
if( VISTACORELIBS_IPC_USE_EXCEPTIONS )
	add_definitions( -DVISTA_IPC_USE_EXCEPTIONS )
endif( VISTACORELIBS_IPC_USE_EXCEPTIONS )

include( "./_SourceFiles.cmake" )

add_library( VistaInterProcComm ${ProjectSources} )
target_link_libraries( VistaInterProcComm
    ${VISTAINTERPROCCOMM_INTERNAL_DEPENDENCIES}
	${LIBRARIES}
)

vista_configure_lib( VistaInterProcComm )
vista_install( VistaInterProcComm VistaInterProcComm )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaInterProcComm )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaInterProcComm )
