		 
# Transcoder

set( dirFiles_transcoder
    VistaOpenCVFaceTrackTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaOpenCVFaceTrackTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaOpenCVFaceTrackTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaOpenCVFaceTrackTranscoder )
vista_install( VistaOpenCVFaceTrackTranscoder "VistaDeviceDrivers/VistaOpenCVFaceTrackDriver" "DriverPlugins" )	 
set_target_properties( VistaOpenCVFaceTrackTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaOpenCVFaceTrackTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( OpenCV 2.4+ QUIET )

if( NOT OPENCV_FOUND )
	message( STATUS "VistaOpenCVFaceTrackDriver requires OpenCV 2.4+, which is not available - only transcoder will be built" )
else( NOT OPENCV_FOUND )

	vista_use_package( CLEye QUIET )
	if( CLEYE_FOUND )
		message( STATUS "OpenCVDriver found CLEye API - enabling custom support" )
		add_definitions( -DOPENCVFACETRACK_WITH_CLEYE )
	endif()

	# Driver Plugin

	set( dirFiles_plugin
		VistaOpenCVFaceTrackPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaOpenCVFaceTrackPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaOpenCVFaceTrackPlugin
		VistaOpenCVFaceTrackDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaOpenCVFaceTrackPlugin )
	vista_install( VistaOpenCVFaceTrackPlugin "VistaDeviceDrivers/VistaOpenCVFaceTrackDriver" "DriverPlugins" )
	set_target_properties( VistaOpenCVFaceTrackPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOpenCVFaceTrackPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( OpenCV REQUIRED )

	set( dirFiles_driver
		VistaOpenCVFaceTrackDriver.h
		VistaOpenCVFaceTrackDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaOpenCVFaceTrackDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaOpenCVFaceTrackDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaOpenCVFaceTrackDriver )
	vista_install( VistaOpenCVFaceTrackDriver "VistaDeviceDrivers/VistaOpenCVFaceTrackDriver" "DriverPlugins" )
	set_target_properties( VistaOpenCVFaceTrackDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOpenCVFaceTrackDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaOpenCVFaceTrackDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT OPENCV_FOUND )
