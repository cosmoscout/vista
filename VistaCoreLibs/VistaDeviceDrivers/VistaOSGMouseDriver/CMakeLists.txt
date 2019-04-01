		 
# Transcoder

set( dirFiles_transcoder
    VistaOSGMouseTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaOSGMouseTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaOSGMouseTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaOSGMouseTranscoder )
vista_install( VistaOSGMouseTranscoder "VistaDeviceDrivers/VistaOSGMouseDriver" "DriverPlugins" )	 
set_target_properties( VistaOSGMouseTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaOSGMouseTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( OpenSceneGraph COMPONENTS osgViewer osgGA QUIET )

if( NOT OSG_FOUND )
	message( STATUS "VistaOSGMouseDriver requires OpenSceneGraph, which is not available - only transcoder will be built" )
else( NOT OSG_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaOSGMousePlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaOSGMousePlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaOSGMousePlugin
		VistaOSGMouseDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaOSGMousePlugin )
	vista_install( VistaOSGMousePlugin "VistaDeviceDrivers/VistaOSGMouseDriver" "DriverPlugins" )
	set_target_properties( VistaOSGMousePlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOSGMousePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( OpenSceneGraph REQUIRED osgViewer osgGA )

	set( dirFiles_driver
		VistaOSGMouseDriver.h
		VistaOSGMouseDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaOSGMouseDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaOSGMouseDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		VistaKernel
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaOSGMouseDriver )
	vista_install( VistaOSGMouseDriver "VistaDeviceDrivers/VistaOSGMouseDriver" "DriverPlugins" )
	set_target_properties( VistaOSGMouseDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOSGMouseDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaOSGMouseDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT OSG_FOUND )
