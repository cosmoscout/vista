vista_find_package( wiiuse REQUIRED )
		 
# Transcoder

set( dirFiles_transcoder
    VistaWiimoteTranscoder.cpp
	VistaWiimoteCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaWiimoteTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaWiimoteTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaWiimoteTranscoder )
vista_install( VistaWiimoteTranscoder "VistaDeviceDrivers/VistaWiimoteDriver" "DriverPlugins" )	 
set_target_properties( VistaWiimoteTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaWiimoteTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( wiiuse QUIET )

if( NOT WIIUSE_FOUND )
	message( STATUS "VistaWiimoteDriver requires wiiuse, which is not available - only transcoder will be built" )
else( NOT WIIUSE_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaWiimotePlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaWiimotePlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaWiimotePlugin
		VistaWiimoteDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaWiimotePlugin )
	vista_install( VistaWiimotePlugin "VistaDeviceDrivers/VistaWiimoteDriver" "DriverPlugins" )
	set_target_properties( VistaWiimotePlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaWiimotePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( wiiuse REQUIRED )

	set( dirFiles_driver
		VistaWiimoteDriver.h
		VistaWiimoteDriver.cpp
		VistaWiimoteCommonShare.h
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaWiimoteDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaWiimoteDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaWiimoteDriver )
	vista_install( VistaWiimoteDriver "VistaDeviceDrivers/VistaWiimoteDriver" "DriverPlugins" )
	set_target_properties( VistaWiimoteDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaWiimoteDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaWiimoteDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT WIIUSE_FOUND )
