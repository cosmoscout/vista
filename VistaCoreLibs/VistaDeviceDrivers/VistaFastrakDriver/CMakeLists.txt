		 
# Transcoder

set( dirFiles_transcoder
	VistaFastrakCommonShare.h
    VistaFastrakTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaFastrakTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaFastrakTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaFastrakTranscoder )
vista_install( VistaFastrakTranscoder "VistaDeviceDrivers/VistaFastrakDriver" "DriverPlugins" )	 
set_target_properties( VistaFastrakTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaFastrakTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin

set( dirFiles_plugin
	VistaFastrakCommonShare.h
	VistaFastrakPlugin.cpp
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaFastrakPlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaFastrakPlugin
	VistaFastrakDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaFastrakPlugin )
vista_install( VistaFastrakPlugin "VistaDeviceDrivers/VistaFastrakDriver" "DriverPlugins" )
set_target_properties( VistaFastrakPlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaFastrakPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

# Actual Driver

set( dirFiles_driver
	VistaFastrakCommonShare.h
	VistaFastrakDriver.h
	VistaFastrakDriver.cpp
)
source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaFastrakDriver
	${dirFiles_driver}
)

target_link_libraries( VistaFastrakDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaFastrakDriver )
vista_install( VistaFastrakDriver "VistaDeviceDrivers/VistaFastrakDriver" "DriverPlugins" )
set_target_properties( VistaFastrakDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaFastrakDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaFastrakDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

