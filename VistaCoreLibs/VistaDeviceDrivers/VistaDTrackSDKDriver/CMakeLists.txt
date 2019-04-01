		 
#Transcoder
SET (dirFiles_transcoder
    VistaDTrackSDKDriverTranscoder.cpp
)
SOURCE_GROUP ("Source Files" FILES ${dirFiles_transcoder})
ADD_LIBRARY (VistaDTrackSDKTranscoder
    ${dirFiles_transcoder}
)
TARGET_LINK_LIBRARIES (VistaDTrackSDKTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaDTrackSDKTranscoder )
vista_install( VistaDTrackSDKTranscoder "VistaDeviceDrivers/VistaDTrackSDKDriver" "DriverPlugins" )	 
set_target_properties( VistaDTrackSDKTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaDTrackSDKTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 

# Actual Driver

set( dirFiles_driver

	VistaDTrackSDKDriver.h
	VistaDTrackSDKDriver.cpp
	
	3rdParty/DTrack.cpp
	3rdParty/DTrack.hpp
)

include_directories( ./3rdParty )

source_group( "Source Files" FILES ${dirFiles_driver} )
add_library( VistaDTrackSDKDriver
	${dirFiles_driver}
)

target_link_libraries( VistaDTrackSDKDriver
	VistaBase
	VistaAspects
	VistaInterProcComm
	VistaDeviceDriversBase
	VistaMath
	${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib( VistaDTrackSDKDriver )
vista_install( VistaDTrackSDKDriver "VistaDeviceDrivers/VistaDTrackSDKDriver" "DriverPlugins" )
set_target_properties( VistaDTrackSDKDriver PROPERTIES FOLDER "Drivers" )
vista_set_outdir( VistaDTrackSDKDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
set_property( TARGET VistaDTrackSDKDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

