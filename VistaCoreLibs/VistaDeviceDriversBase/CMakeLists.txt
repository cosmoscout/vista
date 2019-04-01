

vista_adopt_version( VistaDeviceDriversBase VistaCoreLibs )

# set our dependencies to other corelibs
set( VISTADEVICEDRIVERSBASE_INTERNAL_DEPENDENCIES 
			VistaBase
			VistaAspects
			VistaInterProcComm
			VistaTools
			VistaMath )
set( VISTADEVICEDRIVERSBASE_INTERNAL_DEPENDENCIES ${VISTADEVICEDRIVERSBASE_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

if( WIN32 )
	set( LIBRARIES setupapi )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

set( VISTADEVICEDRIVERS_USE_ATOMICS TRUE CACHE BOOL "Specifies if the Device Driver History uses atomics" )

if( VISTADEVICEDRIVERS_USE_ATOMICS )
	add_definitions( -DVISTA_USE_ATOMICS )
endif( VISTADEVICEDRIVERS_USE_ATOMICS )

add_library( VistaDeviceDriversBase ${ProjectSources} )
target_link_libraries( VistaDeviceDriversBase
    ${VISTADEVICEDRIVERSBASE_INTERNAL_DEPENDENCIES}
	${LIBRARIES}
)

vista_configure_lib( VistaDeviceDriversBase )
vista_install( VistaDeviceDriversBase VistaDeviceDriversBase )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaDeviceDriversBase )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaDeviceDriversBase )
