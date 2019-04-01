		 
# Transcoder

set( dirFiles_transcoder
    VistaJswJoystickTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaJswJoystickTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaJswJoystickTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaJswJoystickTranscoder )
vista_install( VistaJswJoystickTranscoder "VistaDeviceDrivers/VistaJswJoystickDriver" "DriverPlugins" )	 
set_target_properties( VistaJswJoystickTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaJswJoystickTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( jsw QUIET )

if( NOT JSW_FOUND )
	message( STATUS "VistaJswJoystickDriver requires jsw, which is not available - only transcoder will be built" )
else( NOT JSW_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaJswJoystickPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaJswJoystickPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaJswJoystickPlugin
		VistaJswJoystickDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaJswJoystickPlugin )
	vista_install( VistaJswJoystickPlugin "VistaDeviceDrivers/VistaJswJoystickDriver" "DriverPlugins" )
	set_target_properties( VistaJswJoystickPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaJswJoystickPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( jsw REQUIRED )

	set( dirFiles_driver
		VistaJswJoystickDriver.h
		VistaJswJoystickDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaJswJoystickDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaJswJoystickDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaJswJoystickDriver )
	vista_install( VistaJswJoystickDriver "VistaDeviceDrivers/VistaJswJoystickDriver" "DriverPlugins" )
	set_target_properties( VistaJswJoystickDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaJswJoystickDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaJswJoystickDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT JSW_FOUND )
