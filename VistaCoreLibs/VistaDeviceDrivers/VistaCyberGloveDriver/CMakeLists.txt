		 
# Transcoder

set( dirFiles_transcoder
	VistaCyberGloveCommonShare.h
    VistaCyberGloveTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaCyberGloveTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaCyberGloveTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaCyberGloveTranscoder )
vista_install( VistaCyberGloveTranscoder "VistaDeviceDrivers/VistaCyberGloveDriver" "DriverPlugins" )	 
set_target_properties( VistaCyberGloveTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaCyberGloveTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin

set( dirFiles_plugin
	VistaCyberGlovePlugin.cpp
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaCyberGlovePlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaCyberGlovePlugin
	VistaCyberGloveDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaCyberGlovePlugin )
vista_install( VistaCyberGlovePlugin "VistaDeviceDrivers/VistaCyberGloveDriver" "DriverPlugins" )
set_target_properties( VistaCyberGlovePlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaCyberGlovePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

# Actual Driver

set( dirFiles_driver
	VistaCyberGloveCommonShare.h
	VistaCyberGloveDriver.h
	VistaCyberGloveDriver.cpp
)
source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaCyberGloveDriver
	${dirFiles_driver}
)

target_link_libraries( VistaCyberGloveDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaCyberGloveDriver )
vista_install( VistaCyberGloveDriver "VistaDeviceDrivers/VistaCyberGloveDriver" "DriverPlugins" )
set_target_properties( VistaCyberGloveDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaCyberGloveDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaCyberGloveDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

