		 
# Transcoder

set( dirFiles_transcoder
    VistaGlutKeyboardTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaGlutKeyboardTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaGlutKeyboardTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaGlutKeyboardTranscoder )
vista_install( VistaGlutKeyboardTranscoder "VistaDeviceDrivers/VistaGlutKeyboardDriver" "DriverPlugins" )	 
set_target_properties( VistaGlutKeyboardTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaGlutKeyboardTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( GLUT QUIET )

if( NOT GLUT_FOUND )
	message( STATUS "VistaGlutKeyboardDriver requires GLUT, which is not available - only transcoder will be built" )
else( NOT GLUT_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaGlutKeyboardPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaGlutKeyboardPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaGlutKeyboardPlugin
		VistaGlutKeyboardDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaGlutKeyboardPlugin )
	vista_install( VistaGlutKeyboardPlugin "VistaDeviceDrivers/VistaGlutKeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaGlutKeyboardPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaGlutKeyboardPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( GLUT REQUIRED )

	set( dirFiles_driver
		VistaGlutKeyboardDriver.h
		VistaGlutKeyboardDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaGlutKeyboardDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaGlutKeyboardDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaGlutKeyboardDriver )
	vista_install( VistaGlutKeyboardDriver "VistaDeviceDrivers/VistaGlutKeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaGlutKeyboardDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaGlutKeyboardDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaGlutKeyboardDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT GLUT_FOUND )
