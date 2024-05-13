		 
# Transcoder

set( dirFiles_transcoder
    VistaSDL2MouseTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaSDL2MouseTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaSDL2MouseTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaSDL2MouseTranscoder )
vista_install( VistaSDL2MouseTranscoder "VistaDeviceDrivers/VistaSDL2MouseDriver" "DriverPlugins" )	 
set_target_properties( VistaSDL2MouseTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaSDL2MouseTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( SDL2 QUIET )

if( NOT SDL2_FOUND )
	message( STATUS "VistaSDL2MouseDriver requires SDL2, which is not available - only transcoder will be built" )
else( NOT SDL2_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaSDL2MousePlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaSDL2MousePlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaSDL2MousePlugin
		VistaSDL2MouseDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaSDL2MousePlugin )
	vista_install( VistaSDL2MousePlugin "VistaDeviceDrivers/VistaSDL2MouseDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2MousePlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2MousePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( SDL2 REQUIRED )

	set( dirFiles_driver
		VistaSDL2MouseDriver.h
		VistaSDL2MouseDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaSDL2MouseDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaSDL2MouseDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		VistaKernel
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaSDL2MouseDriver )
	vista_install( VistaSDL2MouseDriver "VistaDeviceDrivers/VistaSDL2MouseDriver" "DriverPlugins" )
	set_target_properties( VistaSDL2MouseDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaSDL2MouseDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaSDL2MouseDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT SDL2_FOUND )
