		 
# Transcoder

set( dirFiles_transcoder
    VistaDirectXGamepadTranscoder.cpp
	VistaDirectXGamepadSensorMeasures.h
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaDirectXGamepadTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaDirectXGamepadTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaDirectXGamepadTranscoder )
vista_install( VistaDirectXGamepadTranscoder "VistaDeviceDrivers/VistaDirectXGamepadDriver" "DriverPlugins" )	 
set_target_properties( VistaDirectXGamepadTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaDirectXGamepadTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( DirectX )

if( NOT DIRECTX_FOUND )
	message( STATUS "VistaDirectXGamepadTranscoder requires DirectX, which is not available - only transcoder will be built" )
else( NOT DIRECTX_FOUND )
	# Driver Plugin

	set( dirFiles_plugin
		VistaDirectXGamepadPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaDirectXGamepadPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaDirectXGamepadPlugin
		VistaDirectXGamepadDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaDirectXGamepadPlugin )
	vista_install( VistaDirectXGamepadPlugin "VistaDeviceDrivers/VistaDirectXGamepadDriver" "DriverPlugins" )
	set_target_properties( VistaDirectXGamepadPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaDirectXGamepadPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( DirectX REQUIRED )

	set( dirFiles_driver
		VistaDirectXGamepadDriver.h
		VistaDirectXGamepadDriver.cpp
		VistaDirectXGamepadSensorMeasures.h
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaDirectXGamepadDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaDirectXGamepadDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaDirectXGamepadDriver )
	vista_install( VistaDirectXGamepadDriver "VistaDeviceDrivers/VistaDirectXGamepadDriver" "DriverPlugins" )
	set_target_properties( VistaDirectXGamepadDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaDirectXGamepadDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaDirectXGamepadDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT DIRECTX_FOUND )
