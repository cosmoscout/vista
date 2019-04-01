		 
# Transcoder

set( dirFiles_transcoder
    VistaOpenCVCaptureTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaOpenCVCaptureTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaOpenCVCaptureTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaOpenCVCaptureTranscoder )
vista_install( VistaOpenCVCaptureTranscoder "VistaDeviceDrivers/VistaOpenCVCaptureDriver" "DriverPlugins" )	 
set_target_properties( VistaOpenCVCaptureTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaOpenCVCaptureTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( OpenCV QUIET )

if( NOT OPENCV_FOUND )
	message( STATUS "VistaOpenCVCaptureDriver requires OpenCV, which is not available - only transcoder will be built" )
else( NOT OPENCV_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaOpenCVCapturePlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaOpenCVCapturePlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaOpenCVCapturePlugin
		VistaOpenCVCaptureDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaOpenCVCapturePlugin )
	vista_install( VistaOpenCVCapturePlugin "VistaDeviceDrivers/VistaOpenCVCaptureDriver" "DriverPlugins" )
	set_target_properties( VistaOpenCVCapturePlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOpenCVCapturePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( OpenCV REQUIRED )

	set( dirFiles_driver
		VistaOpenCVCaptureDriver.h
		VistaOpenCVCaptureDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaOpenCVCaptureDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaOpenCVCaptureDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaOpenCVCaptureDriver )
	vista_install( VistaOpenCVCaptureDriver "VistaDeviceDrivers/VistaOpenCVCaptureDriver" "DriverPlugins" )
	set_target_properties( VistaOpenCVCaptureDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOpenCVCaptureDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaOpenCVCaptureDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT OPENCV_FOUND )
