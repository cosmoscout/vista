		 
# Transcoder

set( dirFiles_transcoder
    VistaPhantomServerTranscoder.cpp
	VistaPhantomServerCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaPhantomServerTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaPhantomServerTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaPhantomServerTranscoder )
vista_install( VistaPhantomServerTranscoder "VistaDeviceDrivers/VistaPhantomServerDriver" "DriverPlugins" )	 
set_target_properties( VistaPhantomServerTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaPhantomServerTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin

set( dirFiles_plugin
	VistaPhantomServerPlugin.cpp
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaPhantomServerPlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaPhantomServerPlugin
	VistaPhantomServerDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaPhantomServerPlugin )
vista_install( VistaPhantomServerPlugin "VistaDeviceDrivers/VistaPhantomServerDriver" "DriverPlugins" )
set_target_properties( VistaPhantomServerPlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaPhantomServerPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

# Actual Driver

set( dirFiles_driver
	VistaPhantomServerDriver.h
	VistaPhantomServerDriver.cpp
	VistaPhantomServerCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaPhantomServerDriver
	${dirFiles_driver}
)

target_link_libraries( VistaPhantomServerDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaPhantomServerDriver )
vista_install( VistaPhantomServerDriver "VistaDeviceDrivers/VistaPhantomServerDriver" "DriverPlugins" )
set_target_properties( VistaPhantomServerDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaPhantomServerDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaPhantomServerDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

