		 
# Transcoder

set( dirFiles_transcoder
    VistaOpenNITranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaOpenNITranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaOpenNITranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaOpenNITranscoder )
vista_install( VistaOpenNITranscoder "VistaDeviceDrivers/VistaOpenNIDriver" "DriverPlugins" )	 
set_target_properties( VistaOpenNITranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaOpenNITranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( OpenNI QUIET )

if( NOT OPENNI_FOUND )
	message( STATUS "VistaOpenNIDriver requires OpenNI, which is not available - only transcoder will be built" )
else( NOT OPENNI_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaOpenNIPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaOpenNIPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaOpenNIPlugin
		VistaOpenNIDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaOpenNIPlugin )
	vista_install( VistaOpenNIPlugin "VistaDeviceDrivers/VistaOpenNIDriver" "DriverPlugins" )
	set_target_properties( VistaOpenNIPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOpenNIPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( OpenNI REQUIRED )

	set( dirFiles_driver
		VistaOpenNIDriver.h
		VistaOpenNIDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaOpenNIDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaOpenNIDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaOpenNIDriver )
	vista_install( VistaOpenNIDriver "VistaDeviceDrivers/VistaOpenNIDriver" "DriverPlugins" )
	set_target_properties( VistaOpenNIDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOpenNIDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaOpenNIDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT OPENNI_FOUND )
