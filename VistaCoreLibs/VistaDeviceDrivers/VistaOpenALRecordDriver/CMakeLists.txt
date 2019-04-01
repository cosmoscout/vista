# the transcoder curently requires OpenAL, too - we might change this
vista_find_package( OpenAL REQUIRED )
		 
# Transcoder

set( dirFiles_transcoder
    VistaOpenALRecordTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaOpenALRecordTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaOpenALRecordTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaOpenALRecordTranscoder )
vista_install( VistaOpenALRecordTranscoder "VistaDeviceDrivers/VistaOpenALRecordDriver" "DriverPlugins" )	 
set_target_properties( VistaOpenALRecordTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaOpenALRecordTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( ALUT QUIET )

if( NOT OPENAL_FOUND OR NOT ALUT_FOUND )
	message( STATUS "VistaOpenALRecordDriver requires OpenAL and ALUT, which is not available - only transcoder will be built" )
else( NOT OPENAL_FOUND OR NOT ALUT_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaOpenALRecordPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaOpenALRecordPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaOpenALRecordPlugin
		VistaOpenALRecordDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaOpenALRecordPlugin )
	vista_install( VistaOpenALRecordPlugin "VistaDeviceDrivers/VistaOpenALRecordDriver" "DriverPlugins" )
	set_target_properties( VistaOpenALRecordPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOpenALRecordPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( OpenAL REQUIRED )
	vista_use_package( ALUT REQUIRED )

	set( dirFiles_driver
		VistaOpenALRecordDriver.h
		VistaOpenALRecordDriver.cpp
		OpenALSoundContext.h
		OpenALSoundContext.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaOpenALRecordDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaOpenALRecordDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaOpenALRecordDriver )
	vista_install( VistaOpenALRecordDriver "VistaDeviceDrivers/VistaOpenALRecordDriver" "DriverPlugins" )
	set_target_properties( VistaOpenALRecordDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaOpenALRecordDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaOpenALRecordDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
endif( NOT OPENAL_FOUND OR NOT ALUT_FOUND )
