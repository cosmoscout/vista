		 
# Transcoder

set( dirFiles_transcoder
    VistaHapticDeviceEmulatorTranscoder.cpp
	VistaHapticDeviceEmulatorCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaHapticDeviceEmulatorTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaHapticDeviceEmulatorTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaHapticDeviceEmulatorTranscoder )
vista_install( VistaHapticDeviceEmulatorTranscoder "VistaDeviceDrivers/VistaHapticDeviceEmulatorDriver" "DriverPlugins" )	 
set_target_properties( VistaHapticDeviceEmulatorTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaHapticDeviceEmulatorTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin

set( dirFiles_plugin
	VistaHapticDeviceEmulatorPlugin.cpp
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaHapticDeviceEmulatorPlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaHapticDeviceEmulatorPlugin
	VistaHapticDeviceEmulatorDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaHapticDeviceEmulatorPlugin )
vista_install( VistaHapticDeviceEmulatorPlugin "VistaDeviceDrivers/VistaHapticDeviceEmulatorDriver" "DriverPlugins" )
set_target_properties( VistaHapticDeviceEmulatorPlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaHapticDeviceEmulatorPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

# Actual Driver

set( dirFiles_driver
	VistaHapticDeviceEmulatorDriver.h
	VistaHapticDeviceEmulatorDriver.cpp
	VistaHapticDeviceEmulatorCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaHapticDeviceEmulatorDriver
	${dirFiles_driver}
)

target_link_libraries( VistaHapticDeviceEmulatorDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaHapticDeviceEmulatorDriver )
vista_install( VistaHapticDeviceEmulatorDriver "VistaDeviceDrivers/VistaHapticDeviceEmulatorDriver" "DriverPlugins" )
set_target_properties( VistaHapticDeviceEmulatorDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaHapticDeviceEmulatorDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaHapticDeviceEmulatorDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

