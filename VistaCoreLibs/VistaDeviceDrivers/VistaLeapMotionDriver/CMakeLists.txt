		 
# Transcoder

set( dirFiles_transcoder
    VistaLeapMotionTranscoder.cpp
	VistaLeapMotionCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaLeapMotionTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaLeapMotionTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaLeapMotionTranscoder )
vista_install( VistaLeapMotionTranscoder "VistaDeviceDrivers/VistaLeapMotionDriver" "DriverPlugins" )	 
set_target_properties( VistaLeapMotionTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaLeapMotionTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( LeapSDK 2.1+ QUIET )

if( NOT LEAPSDK_FOUND )
	message( STATUS "VistaLeapMotionDriver requires LeapSDK v2.1 or greater, which is not available - only transcoder will be built" )
else()

	# Driver Plugin

	set( dirFiles_plugin
		VistaLeapMotionPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaLeapMotionPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaLeapMotionPlugin
		VistaLeapMotionDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaLeapMotionPlugin )
	vista_install( VistaLeapMotionPlugin "VistaDeviceDrivers/VistaLeapMotionDriver" "DriverPlugins" )
	set_target_properties( VistaLeapMotionPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaLeapMotionPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( LeapSDK REQUIRED )

	set( dirFiles_driver
		VistaLeapMotionDriver.h
		VistaLeapMotionDriver.cpp
		VistaLeapMotionCommonShare.h
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaLeapMotionDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaLeapMotionDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaLeapMotionDriver )
	vista_install( VistaLeapMotionDriver "VistaDeviceDrivers/VistaLeapMotionDriver" "DriverPlugins" )
	set_target_properties( VistaLeapMotionDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaLeapMotionDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaLeapMotionDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif()
