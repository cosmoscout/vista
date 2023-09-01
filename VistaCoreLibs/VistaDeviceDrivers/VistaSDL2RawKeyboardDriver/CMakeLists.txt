		 
# Transcoder

set( dirFiles_transcoder
    VistaSDL2RawKeyboardTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaSDL2RawKeyboardTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaSDL2RawKeyboardTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaSDL2RawKeyboardTranscoder )
vista_install( VistaSDL2RawKeyboardTranscoder "VistaDeviceDrivers/VistaSDL2RawKeyboardDriver" "DriverPlugins" )	 
set_target_properties( VistaSDL2RawKeyboardTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaSDL2RawKeyboardTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( SDL2 QUIET )

if( NOT SDL2_FOUND )
	message( STATUS "VistaSDL2RawKeyboardDriver requires SDL2, which is not available - only transcoder will be built" )
else( NOT SDL2_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaSDL2RawKeyboardPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaSDL2RawKeyboardPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaSDL2RawKeyboardPlugin
		VistaSDL2RawKeyboardDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaSDL2RawKeyboardPlugin )
	vista_install( VistaSDL2RawKeyboardPlugin "VistaDeviceDrivers/VistaSDL2RawKeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2RawKeyboardPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2RawKeyboardPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( SDL2 REQUIRED )

	set( dirFiles_driver
		VistaSDL2RawKeyboardDriver.h
		VistaSDL2RawKeyboardDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaSDL2RawKeyboardDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaSDL2RawKeyboardDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaSDL2RawKeyboardDriver )
	vista_install( VistaSDL2RawKeyboardDriver "VistaDeviceDrivers/VistaSDL2RawKeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2RawKeyboardDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2RawKeyboardDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaSDL2RawKeyboardDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT SDL2_FOUND )
