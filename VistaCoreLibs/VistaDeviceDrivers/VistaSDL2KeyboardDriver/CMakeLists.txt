		 
# Transcoder

set( dirFiles_transcoder
    VistaSDL2KeyboardTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaSDL2KeyboardTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaSDL2KeyboardTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaSDL2KeyboardTranscoder )
vista_install( VistaSDL2KeyboardTranscoder "VistaDeviceDrivers/VistaSDL2KeyboardDriver" "DriverPlugins" )	 
set_target_properties( VistaSDL2KeyboardTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaSDL2KeyboardTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( SDL2 QUIET )

if( NOT SDL2_FOUND )
	message( STATUS "VistaSDL2KeyboardDriver requires SDL2, which is not available - only transcoder will be built" )
else( NOT SDL2_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaSDL2KeyboardPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaSDL2KeyboardPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaSDL2KeyboardPlugin
		VistaSDL2KeyboardDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaSDL2KeyboardPlugin )
	vista_install( VistaSDL2KeyboardPlugin "VistaDeviceDrivers/VistaSDL2KeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2KeyboardPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2KeyboardPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( SDL2 REQUIRED )

	set( dirFiles_driver
		VistaSDL2KeyboardDriver.h
		VistaSDL2KeyboardDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaSDL2KeyboardDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaSDL2KeyboardDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaSDL2KeyboardDriver )
	vista_install( VistaSDL2KeyboardDriver "VistaDeviceDrivers/VistaSDL2KeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2KeyboardDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2KeyboardDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaSDL2KeyboardDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT SDL2_FOUND )
