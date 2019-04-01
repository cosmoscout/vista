		 
# Transcoder

set( dirFiles_transcoder
	VistaHIDCommonShare.h
    VistaHIDTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaHIDTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaHIDTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaHIDTranscoder )
vista_install( VistaHIDTranscoder "VistaDeviceDrivers/VistaHIDDriver" "DriverPlugins" )	 
set_target_properties( VistaHIDTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaHIDTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin

set( dirFiles_plugin
	VistaHIDCommonShare.h
	VistaHIDPlugin.cpp
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaHIDPlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaHIDPlugin
	VistaHIDDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaHIDPlugin )
vista_install( VistaHIDPlugin "VistaDeviceDrivers/VistaHIDDriver" "DriverPlugins" )
set_target_properties( VistaHIDPlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaHIDPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

# Actual Driver

set( dirFiles_driver
	VistaHIDCommonShare.h
	VistaHIDDriver.h
	VistaHIDDriver.cpp
	VistaHIDDriverForceFeedbackAspect.h
	VistaHIDDriverForceFeedbackAspect.cpp
)
source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaHIDDriver
	${dirFiles_driver}
)

target_link_libraries( VistaHIDDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaHIDDriver )
vista_install( VistaHIDDriver "VistaDeviceDrivers/VistaHIDDriver" "DriverPlugins" )
set_target_properties( VistaHIDDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaHIDDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaHIDDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

