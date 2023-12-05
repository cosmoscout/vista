		 
# Transcoder

set( dirFiles_transcoder
    VistaSDL2ControllerTranscoder.cpp
	VistaSDL2ControllerState.h
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaSDL2ControllerTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaSDL2ControllerTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaSDL2ControllerTranscoder )
vista_install( VistaSDL2ControllerTranscoder "VistaDeviceDrivers/VistaSDL2ControllerDriver" "DriverPlugins" )	 
set_target_properties( VistaSDL2ControllerTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaSDL2ControllerTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( SDL2 )

if( NOT SDL2_FOUND )
	message( STATUS "VistaSDL2ControllerTranscoder requires SDL2, which is not available - only transcoder will be built" )
else( NOT SDL2_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaSDL2ControllerPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaSDL2ControllerPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaSDL2ControllerPlugin
		VistaSDL2ControllerDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaSDL2ControllerPlugin )
	vista_install( VistaSDL2ControllerPlugin "VistaDeviceDrivers/VistaSDL2ControllerDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2ControllerPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2ControllerPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( SDL2 REQUIRED )

	set( dirFiles_driver
		VistaSDL2ControllerDriver.h
		VistaSDL2ControllerDriver.cpp
		VistaSDL2ControllerState.h
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaSDL2ControllerDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaSDL2ControllerDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		VistaKernel
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaSDL2ControllerDriver )
	vista_install( VistaSDL2ControllerDriver "VistaDeviceDrivers/VistaSDL2ControllerDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2ControllerDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2ControllerDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaSDL2ControllerDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT SDL2_FOUND )
