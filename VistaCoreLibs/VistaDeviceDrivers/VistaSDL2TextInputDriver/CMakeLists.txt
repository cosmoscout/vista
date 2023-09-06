		 
# Transcoder

set( dirFiles_transcoder
    VistaSDL2TextInputTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaSDL2TextInputTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaSDL2TextInputTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaSDL2TextInputTranscoder )
vista_install( VistaSDL2TextInputTranscoder "VistaDeviceDrivers/VistaSDL2TextInputDriver" "DriverPlugins" )
set_target_properties( VistaSDL2TextInputTranscoder PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaSDL2TextInputTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( SDL2 QUIET )

if( NOT SDL2_FOUND )
	message( STATUS "VistaSDL2TextInputDriver requires SDL2, which is not available - only transcoder will be built" )
else( NOT SDL2_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaSDL2TextInputPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaSDL2TextInputPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaSDL2TextInputPlugin
		VistaSDL2TextInputDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaSDL2TextInputPlugin )
	vista_install( VistaSDL2TextInputPlugin "VistaDeviceDrivers/VistaSDL2TextInputDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2TextInputPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2TextInputPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( SDL2 REQUIRED )

	set( dirFiles_driver
		VistaSDL2TextInputDriver.h
		VistaSDL2TextInputDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaSDL2TextInputDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaSDL2TextInputDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		VistaKernel
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaSDL2TextInputDriver )
	vista_install( VistaSDL2TextInputDriver "VistaDeviceDrivers/VistaSDL2TextInputDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2TextInputDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2TextInputDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaSDL2TextInputDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT SDL2_FOUND )
