
vista_find_package( OPENVR QUIET )
	 
#Transcoder
SET (dirFiles_transcoder
    VistaViveDriverTranscoder.cpp
)
SOURCE_GROUP ("Source Files" FILES ${dirFiles_transcoder})
ADD_LIBRARY (VistaViveDriverTranscoder
    ${dirFiles_transcoder}
)
TARGET_LINK_LIBRARIES (VistaViveDriverTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaViveDriverTranscoder )
vista_install( VistaViveDriverTranscoder "VistaDeviceDrivers/VistaViveDriver" "DriverPlugins" )	 
set_target_properties( VistaViveDriverTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaViveDriverTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 

# Actual Driver

set( dirFiles_driver

	VistaViveDriver.h
	VistaViveDriver.cpp
)

include_directories( ${OPENVR_ROOT_DIR}/headers)

source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaViveDriver
	${dirFiles_driver}
)

target_link_libraries( VistaViveDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
	${OPENVR_LIBRARY_DIRS}/${OPENVR_LIBRARIES}
)

vista_configure_lib( VistaViveDriver )
vista_install( VistaViveDriver "VistaDeviceDrivers/VistaViveDriver" "DriverPlugins" )
set_target_properties( VistaViveDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaViveDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaViveDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver


# Driver Plugin

set( dirFiles_plugin
	VistaViveDriverPlugin.cpp
	VistaViveDriverConfig.h
)
source_group( "Source Files" FILES ${dirFiles_plugin} )
add_library( VistaViveDriverPlugin
	${dirFiles_plugin}
) 
target_link_libraries( VistaViveDriverPlugin
	VistaViveDriver
	VistaDeviceDriversBase
)
vista_configure_lib( VistaViveDriverPlugin )
vista_install( VistaViveDriverPlugin "VistaDeviceDrivers/VistaViveDriver" "DriverPlugins" )
set_target_properties( VistaViveDriverPlugin PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaViveDriverPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )


#################

IF (WIN32)
	SET_TARGET_PROPERTIES( VistaViveDriver PROPERTIES COMPILE_DEFINITIONS "VISTAVIVE_EXPORTS" )
	SET_TARGET_PROPERTIES( VistaViveDriverTranscoder PROPERTIES COMPILE_DEFINITIONS "VISTAVIVEDRIVERTRANSCODER_EXPORTS" )
    # SET_TARGET_PROPERTIES( VistaViveDriverPlugin PROPERTIES COMPILE_DEFINITIONS "VISTAVIVEDRIVERPLUGIN_EXPORTS" )
	# SET_TARGET_PROPERTIES( VistaViveDriverPlugin PROPERTIES COMPILE_DEFINITIONS "VISTAVIVEDRIVERPLUGINDLL" )
	# SET_TARGET_PROPERTIES( VistaViveDriverPlugin PROPERTIES COMPILE_DEFINITIONS "VISTAVIVEDRIVERPLUGINDLL_EXPORTS" )
	target_compile_definitions(VistaViveDriverPlugin PRIVATE VISTAVIVEDRIVERPLUGINDLL_EXPORTS=1 VISTAVIVEDRIVERPLUGINDLL=1)

ENDIF(WIN32)