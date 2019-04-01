		 
# Transcoder

set( dirFiles_transcoder
    VistaOSGKeyboardTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaOSGKeyboardTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaOSGKeyboardTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaOSGKeyboardTranscoder )
vista_install( VistaOSGKeyboardTranscoder "VistaDeviceDrivers/VistaOSGKeyboardDriver" "DriverPlugins" )	 
set_target_properties( VistaOSGKeyboardTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaOSGKeyboardTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( OpenSceneGraph COMPONENTS osgViewer osgGA QUIET )

if( NOT OSG_FOUND )
	message( STATUS "VistaOSGKeyboardDriver requires OSG, which is not available - only transcoder will be built" )
else( NOT OSG_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaOSGKeyboardPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaOSGKeyboardPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaOSGKeyboardPlugin
		VistaOSGKeyboardDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaOSGKeyboardPlugin )
	vista_install( VistaOSGKeyboardPlugin "VistaDeviceDrivers/VistaOSGKeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaOSGKeyboardPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOSGKeyboardPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( OpenSceneGraph REQUIRED osgViewer osgGA )

	set( dirFiles_driver
		VistaOSGKeyboardDriver.h
		VistaOSGKeyboardDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaOSGKeyboardDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaOSGKeyboardDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		VistaKernel
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaOSGKeyboardDriver )
	vista_install( VistaOSGKeyboardDriver "VistaDeviceDrivers/VistaOSGKeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaOSGKeyboardDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOSGKeyboardDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaOSGKeyboardDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT OSG_FOUND )
