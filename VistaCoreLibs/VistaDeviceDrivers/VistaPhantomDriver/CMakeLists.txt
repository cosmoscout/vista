		 
# Transcoder

set( dirFiles_transcoder
    VistaPhantomTranscoder.cpp
	VistaPhantomCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaPhantomTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaPhantomTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaPhantomTranscoder )
vista_install( VistaPhantomTranscoder "VistaDeviceDrivers/VistaPhantomDriver" "DriverPlugins" )	 
set_target_properties( VistaPhantomTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaPhantomTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( OpenHaptics QUIET )

set( VISTA_PHANTOMDRIVER_USES_VISTA_UNITS FALSE CACHE BOOL "if false, the delivered values are in millimeters, if true in meters" )
if( VISTA_PHANTOMDRIVER_USES_VISTA_UNITS )
	add_definitions( -DPHANTOM_USES_VISTA_UNITS )
endif()


if( NOT OPENHAPTICS_FOUND )
	message( STATUS "VistaPhantomDriver requires OpenHaptics, which is not available - only transcoder will be built" )
else( NOT OPENHAPTICS_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaPhantomPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaPhantomPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaPhantomPlugin
		VistaPhantomDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaPhantomPlugin )
	vista_install( VistaPhantomPlugin "VistaDeviceDrivers/VistaPhantomDriver" "DriverPlugins" )
	set_target_properties( VistaPhantomPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaPhantomPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( OpenHaptics REQUIRED )

	set( dirFiles_driver
		VistaPhantomDriver.h
		VistaPhantomDriver.cpp
		VistaPhantomCommonShare.h
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaPhantomDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaPhantomDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaPhantomDriver )
	vista_install( VistaPhantomDriver "VistaDeviceDrivers/VistaPhantomDriver" "DriverPlugins" )
	set_target_properties( VistaPhantomDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaPhantomDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaPhantomDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT OPENHAPTICS_FOUND )
