		 
# Transcoder

set( dirFiles_transcoder
    VistaSpaceMouseTranscoder.cpp
	VistaSpaceMouseCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaSpaceMouseTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaSpaceMouseTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaSpaceMouseTranscoder )
vista_install( VistaSpaceMouseTranscoder "VistaDeviceDrivers/VistaSpaceMouseDriver" "DriverPlugins" )	 
set_target_properties( VistaSpaceMouseTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaSpaceMouseTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin

set( dirFiles_plugin
	VistaSpaceMousePlugin.cpp
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaSpaceMousePlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaSpaceMousePlugin
	VistaSpaceMouseDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaSpaceMousePlugin )
vista_install( VistaSpaceMousePlugin "VistaDeviceDrivers/VistaSpaceMouseDriver" "DriverPlugins" )
set_target_properties( VistaSpaceMousePlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaSpaceMousePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

# Actual Driver

set( dirFiles_driver
	VistaSpaceMouseDriver.h
	VistaSpaceMouseDriver.cpp
	VistaSpaceMouseCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaSpaceMouseDriver
	${dirFiles_driver}
)

target_link_libraries( VistaSpaceMouseDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaSpaceMouseDriver )
vista_install( VistaSpaceMouseDriver "VistaDeviceDrivers/VistaSpaceMouseDriver" "DriverPlugins" )
set_target_properties( VistaSpaceMouseDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaSpaceMouseDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaSpaceMouseDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

