		 
# Transcoder

set( dirFiles_transcoder
    VistaSDL2EventKeyboardTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaSDL2EventKeyboardTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaSDL2EventKeyboardTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaSDL2EventKeyboardTranscoder )
vista_install( VistaSDL2EventKeyboardTranscoder "VistaDeviceDrivers/VistaSDL2EventKeyboardDriver" "DriverPlugins" )	 
set_target_properties( VistaSDL2EventKeyboardTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaSDL2EventKeyboardTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( SDL2 QUIET )

if( NOT SDL2_FOUND )
	message( STATUS "VistaSDL2EventKeyboardDriver requires SDL2, which is not available - only transcoder will be built" )
else( NOT SDL2_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaSDL2EventKeyboardPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaSDL2EventKeyboardPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaSDL2EventKeyboardPlugin
		VistaSDL2EventKeyboardDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaSDL2EventKeyboardPlugin )
	vista_install( VistaSDL2EventKeyboardPlugin "VistaDeviceDrivers/VistaSDL2EventKeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2EventKeyboardPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2EventKeyboardPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( SDL2 REQUIRED )

	set( dirFiles_driver
		VistaSDL2EventKeyboardDriver.h
		VistaSDL2EventKeyboardDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaSDL2EventKeyboardDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaSDL2EventKeyboardDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		VistaKernel
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaSDL2EventKeyboardDriver )
	vista_install( VistaSDL2EventKeyboardDriver "VistaDeviceDrivers/VistaSDL2EventKeyboardDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2EventKeyboardDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2EventKeyboardDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaSDL2EventKeyboardDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT SDL2_FOUND )
