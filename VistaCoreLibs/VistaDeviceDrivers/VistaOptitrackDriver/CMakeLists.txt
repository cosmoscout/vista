		 
# Transcoder

set( dirFiles_transcoder
	VistaOptitrackCommonShare.h
    VistaOptitrackTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaOptitrackTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaOptitrackTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaOptitrackTranscoder )
vista_install( VistaOptitrackTranscoder "VistaDeviceDrivers/VistaOptitrackDriver" "DriverPlugins" )	 
set_target_properties( VistaOptitrackTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaOptitrackTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# Driver Plugin
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( NatNetSDK QUIET )

if( NOT NATNETSDK_FOUND )
	message( STATUS "VistaOptitrackDriver requires NatNetSDK, which is not available - only transcoder will be built" )
else()

	set( dirFiles_plugin
		VistaOptitrackPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaOptitrackPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaOptitrackPlugin
		VistaOptitrackDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaOptitrackPlugin )
	vista_install( VistaOptitrackPlugin "VistaDeviceDrivers/VistaOptitrackDriver" "DriverPlugins" )
	set_target_properties( VistaOptitrackPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOptitrackPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver
	vista_use_package( NatNetSDK REQUIRED )

	set( dirFiles_driver
		VistaOptitrackCommonShare.h
		VistaOptitrackDriver.h
		VistaOptitrackDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaOptitrackDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaOptitrackDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaOptitrackDriver )
	vista_install( VistaOptitrackDriver "VistaDeviceDrivers/VistaOptitrackDriver" "DriverPlugins" )
	set_target_properties( VistaOptitrackDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOptitrackDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaOptitrackDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif()

