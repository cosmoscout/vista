# Dependencies (link to FTDI API)
vista_use_package( FTDI )
include_directories( ${FTDI_INCLUDE_DIRS} )

# Transcoder
set( dirFiles_transcoder
    VistaIcarosTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaIcarosTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaIcarosTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaIcarosTranscoder )
vista_install( VistaIcarosTranscoder "VistaDeviceDrivers/VistaIcarosDriver" "DriverPlugins" )	 
set_target_properties( VistaIcarosTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaIcarosTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin
set( dirFiles_plugin
	VistaIcarosPlugin.cpp
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaIcarosPlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaIcarosPlugin
	VistaIcarosDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaIcarosPlugin )
vista_install( VistaIcarosPlugin "VistaDeviceDrivers/VistaIcarosDriver" "DriverPlugins" )
set_target_properties( VistaIcarosPlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaIcarosPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

# Actual Driver
set( dirFiles_driver
	VistaIcarosDriver.h
	VistaIcarosDriver.cpp
)
source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaIcarosDriver
	${dirFiles_driver}
)

target_link_libraries( VistaIcarosDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	ftd2xx
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaIcarosDriver )
vista_install( VistaIcarosDriver "VistaDeviceDrivers/VistaIcarosDriver" "DriverPlugins" )
set_target_properties( VistaIcarosDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaIcarosDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaIcarosDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

