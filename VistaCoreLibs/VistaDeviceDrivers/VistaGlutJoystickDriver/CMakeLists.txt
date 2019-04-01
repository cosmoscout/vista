		 
# Transcoder

set( dirFiles_transcoder
    VistaGlutJoystickTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaGlutJoystickTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaGlutJoystickTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaGlutJoystickTranscoder )
vista_install( VistaGlutJoystickTranscoder "VistaDeviceDrivers/VistaGlutJoystickDriver" "DriverPlugins" )	 
set_target_properties( VistaGlutJoystickTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaGlutJoystickTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( GLUT QUIET )

if( NOT GLUT_FOUND )
	message( STATUS "VistaGlutJoystickDriver requires GLUT, which is not available - only transcoder will be built" )
else( NOT GLUT_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaGlutJoystickPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaGlutJoystickPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaGlutJoystickPlugin
		VistaGlutJoystickDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaGlutJoystickPlugin )
	vista_install( VistaGlutJoystickPlugin "VistaDeviceDrivers/VistaGlutJoystickDriver" "DriverPlugins" )
	set_target_properties( VistaGlutJoystickPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaGlutJoystickPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( GLUT REQUIRED )

	set( dirFiles_driver
		VistaGlutJoystickDriver.h
		VistaGlutJoystickDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaGlutJoystickDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaGlutJoystickDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaGlutJoystickDriver )
	vista_install( VistaGlutJoystickDriver "VistaDeviceDrivers/VistaGlutJoystickDriver" "DriverPlugins" )
	set_target_properties( VistaGlutJoystickDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaGlutJoystickDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaGlutJoystickDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT GLUT_FOUND )
