		 
# Transcoder

set( dirFiles_transcoder
	VistaMIDICommonShare.h
    VistaMIDITranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaMIDITranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaMIDITranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaMIDITranscoder )
vista_install( VistaMIDITranscoder "VistaDeviceDrivers/VistaMIDIDriver" "DriverPlugins" )	 
set_target_properties( VistaMIDITranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaMIDITranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin

set( dirFiles_plugin
	VistaMIDIPlugin.cpp
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaMIDIPlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaMIDIPlugin
	VistaMIDIDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaMIDIPlugin )
vista_install( VistaMIDIPlugin "VistaDeviceDrivers/VistaMIDIDriver" "DriverPlugins" )
set_target_properties( VistaMIDIPlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaMIDIPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

# Actual Driver

set( dirFiles_driver
	VistaMIDICommonShare.h
	VistaMIDIDriver.h
	VistaMIDIDriver.cpp
)
source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaMIDIDriver
	${dirFiles_driver}
)

target_link_libraries( VistaMIDIDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaMIDIDriver )
vista_install( VistaMIDIDriver "VistaDeviceDrivers/VistaMIDIDriver" "DriverPlugins" )
set_target_properties( VistaMIDIDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaMIDIDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaMIDIDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

