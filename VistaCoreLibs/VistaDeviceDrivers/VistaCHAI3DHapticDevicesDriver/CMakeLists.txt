		 
# Transcoder

set( dirFiles_transcoder
    VistaCHAI3DHapticDevicesTranscoder.cpp
	VistaCHAI3DHapticDevicesCommonShare.h
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaCHAI3DHapticDevicesTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaCHAI3DHapticDevicesTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaCHAI3DHapticDevicesTranscoder )
vista_install( VistaCHAI3DHapticDevicesTranscoder "VistaDeviceDrivers/VistaCHAI3DHapticDevicesDriver" "DriverPlugins" )	 
set_target_properties( VistaCHAI3DHapticDevicesTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaCHAI3DHapticDevicesTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( Chai3D QUIET )
vista_find_package( OpenGL QUIET )

if( NOT CHAI3D_FOUND OR NOT OPENGL_FOUND)
	message( STATUS "VistaCHAI3DHapticDevicesDriver requires Chai3D, which is not available - only transcoder will be built" )
else( NOT CHAI3D_FOUND OR NOT OPENGL_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaCHAI3DHapticDevicesPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaCHAI3DHapticDevicesPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaCHAI3DHapticDevicesPlugin
		VistaCHAI3DHapticDevicesDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaCHAI3DHapticDevicesPlugin )
	vista_install( VistaCHAI3DHapticDevicesPlugin "VistaDeviceDrivers/VistaCHAI3DHapticDevicesDriver" "DriverPlugins" )
	set_target_properties( VistaCHAI3DHapticDevicesPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaCHAI3DHapticDevicesPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( Chai3D REQUIRED )
	
	set( dirFiles_driver
		VistaCHAI3DHapticDevicesDriver.h
		VistaCHAI3DHapticDevicesDriver.cpp
		VistaCHAI3DHapticDevicesCommonShare.h
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaCHAI3DHapticDevicesDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaCHAI3DHapticDevicesDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaCHAI3DHapticDevicesDriver )
	vista_install( VistaCHAI3DHapticDevicesDriver "VistaDeviceDrivers/VistaCHAI3DHapticDevicesDriver" "DriverPlugins" )
	set_target_properties( VistaCHAI3DHapticDevicesDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaCHAI3DHapticDevicesDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaCHAI3DHapticDevicesDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT CHAI3D_FOUND OR NOT OPENGL_FOUND )
