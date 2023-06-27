		 
# Transcoder

set( dirFiles_transcoder
    VistaGlutMouseTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaGlutMouseTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaGlutMouseTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaGlutMouseTranscoder )
vista_install( VistaGlutMouseTranscoder "VistaDeviceDrivers/VistaGlutMouseDriver" "DriverPlugins" )	 
set_target_properties( VistaGlutMouseTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaGlutMouseTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( GLUT QUIET )

if( NOT GLUT_FOUND )
	message( STATUS "VistaGlutMouseDriver requires GLUT, which is not available - only transcoder will be built" )
else( NOT GLUT_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaGlutMousePlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaGlutMousePlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaGlutMousePlugin
		VistaGlutMouseDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaGlutMousePlugin )
	vista_install( VistaGlutMousePlugin "VistaDeviceDrivers/VistaGlutMouseDriver" "DriverPlugins" )
	set_target_properties( VistaGlutMousePlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaGlutMousePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( GLUT REQUIRED )

	set( dirFiles_driver
		VistaGlutMouseDriver.h
		VistaGlutMouseDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaGlutMouseDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaGlutMouseDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaGlutMouseDriver )
	vista_install( VistaGlutMouseDriver "VistaDeviceDrivers/VistaGlutMouseDriver" "DriverPlugins" )
	set_target_properties( VistaGlutMouseDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaGlutMouseDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaGlutMouseDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT GLUT_FOUND )
