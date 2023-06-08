vista_use_package( OpenVR REQUIRED 0.5  )
add_definitions( -DVISTA_WITH_OPENVR )
	 
#Transcoder
SET (dirFiles_transcoder
    VistaOpenVRDriverTranscoder.cpp
)
SOURCE_GROUP ("Source Files" FILES ${dirFiles_transcoder})
ADD_LIBRARY (VistaOpenVRDriverTranscoder
    ${dirFiles_transcoder}
)
TARGET_LINK_LIBRARIES (VistaOpenVRDriverTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaOpenVRDriverTranscoder )
vista_install( VistaOpenVRDriverTranscoder "VistaDeviceDrivers/VistaOpenVRDriver" "DriverPlugins" )	 
set_target_properties( VistaOpenVRDriverTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaOpenVRDriverTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 

# Actual Driver

set( dirFiles_driver

	VistaOpenVRDriver.h
	VistaOpenVRDriver.cpp
)

include_directories( ${OPENVR_INCLUDE_DIRS})

source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaOpenVRDriver
	${dirFiles_driver}
)

target_link_libraries( VistaOpenVRDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
	${OPENVR_LIBRARY_DIRS}/${OPENVR_LIBRARIES}
)

vista_configure_lib( VistaOpenVRDriver )
vista_install( VistaOpenVRDriver "VistaDeviceDrivers/VistaOpenVRDriver" "DriverPlugins" )
set_target_properties( VistaOpenVRDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaOpenVRDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaOpenVRDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver


# Driver Plugin

set( dirFiles_plugin
	VistaOpenVRDriverPlugin.cpp
	VistaOpenVRDriverConfig.h
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaOpenVRDriverPlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaOpenVRDriverPlugin
	VistaOpenVRDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaOpenVRDriverPlugin )
vista_install( VistaOpenVRDriverPlugin "VistaDeviceDrivers/VistaOpenVRDriver" "DriverPlugins" )
set_target_properties( VistaOpenVRDriverPlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaOpenVRDriverPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )


#################

IF (WIN32)
	SET_TARGET_PROPERTIES( VistaOpenVRDriver PROPERTIES COMPILE_DEFINITIONS "VISTAOPENVR_EXPORTS" )
	SET_TARGET_PROPERTIES( VistaOpenVRDriverTranscoder PROPERTIES COMPILE_DEFINITIONS "VISTAOPENVRDRIVERTRANSCODER_EXPORTS" )
    # SET_TARGET_PROPERTIES( VistaOpenVRDriverPlugin PROPERTIES COMPILE_DEFINITIONS "VISTAOPENVRDRIVERPLUGIN_EXPORTS" )
	# SET_TARGET_PROPERTIES( VistaOpenVRDriverPlugin PROPERTIES COMPILE_DEFINITIONS "VISTAOPENVRDRIVERPLUGINDLL" )
	# SET_TARGET_PROPERTIES( VistaOpenVRDriverPlugin PROPERTIES COMPILE_DEFINITIONS "VISTAOPENVRDRIVERPLUGINDLL_EXPORTS" )
	target_compile_definitions(VistaOpenVRDriverPlugin PRIVATE VISTAOPENVRDRIVERPLUGINDLL_EXPORTS=1 VISTAOPENVRDRIVERPLUGINDLL=1)

ENDIF(WIN32)