		 
# Transcoder

set( dirFiles_transcoder
    VistaIRManTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaIRManTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaIRManTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaIRManTranscoder )
vista_install( VistaIRManTranscoder "VistaDeviceDrivers/VistaIRManDriver" "DriverPlugins" )	 
set_target_properties( VistaIRManTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaIRManTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin

set( dirFiles_plugin
	VistaIRManPlugin.cpp
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaIRManPlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaIRManPlugin
	VistaIRManDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaIRManPlugin )
vista_install( VistaIRManPlugin "VistaDeviceDrivers/VistaIRManDriver" "DriverPlugins" )
set_target_properties( VistaIRManPlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaIRManPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

# Actual Driver

set( dirFiles_driver
	VistaIRManDriver.h
	VistaIRManDriver.cpp
)
source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaIRManDriver
	${dirFiles_driver}
)

target_link_libraries( VistaIRManDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaIRManDriver )
vista_install( VistaIRManDriver "VistaDeviceDrivers/VistaIRManDriver" "DriverPlugins" )
set_target_properties( VistaIRManDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaIRManDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaIRManDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

