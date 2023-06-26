# Transcoder
set( dirFiles_transcoder
    VistaDepthSenseTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaDepthSenseTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaDepthSenseTranscoder
    VistaDeviceDriversBase
)
vista_configure_lib( VistaDepthSenseTranscoder )
vista_install( VistaDepthSenseTranscoder "VistaDeviceDrivers/VistaDepthSenseDriver" "DriverPlugins" )	 
set_target_properties( VistaDepthSenseTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaDepthSenseTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
vista_find_package( DepthSenseSDK QUIET )

if( NOT DEPTHSENSESDK_FOUND )
	message( STATUS "VistaDepthSenseDriver requires the DepthSenseSDK, which is not available - only transcoder will be built" )
else( NOT DEPTHSENSESDK_FOUND )
	# Driver Plugin
	set( dirFiles_plugin
		VistaDepthSensePlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaDepthSensePlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaDepthSensePlugin
		VistaDepthSenseDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaDepthSensePlugin )
	vista_install( VistaDepthSensePlugin "VistaDeviceDrivers/VistaDepthSenseDriver" "DriverPlugins" )
	set_target_properties( VistaDepthSensePlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaDepthSensePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( DepthSenseSDK REQUIRED )

	set( dirFiles_driver
		VistaDepthSenseDriver.h
		VistaDepthSenseDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaDepthSenseDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaDepthSenseDriver
		VistaDeviceDriversBase
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaDepthSenseDriver )
	vista_install( VistaDepthSenseDriver "VistaDeviceDrivers/VistaDepthSenseDriver" "DriverPlugins" )
	set_target_properties( VistaDepthSenseDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaDepthSenseDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaDepthSenseDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT DEPTHSENSESDK_FOUND )
