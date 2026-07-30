#Transcoder

add_library(VistaDTrackSDKTranscoder
        VistaDTrackSDKDriverTranscoder.cpp
)

target_link_libraries(VistaDTrackSDKTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)

vista_configure_lib(VistaDTrackSDKTranscoder)
vista_install(VistaDTrackSDKTranscoder "VistaDeviceDrivers/VistaDTrackSDKDriver" "DriverPlugins")
set_target_properties(VistaDTrackSDKTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaDTrackSDKTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Actual Driver

include_directories(./3rdParty)

add_library(VistaDTrackSDKDriver
        VistaDTrackSDKDriver.h
        VistaDTrackSDKDriver.cpp
        3rdParty/DTrack.cpp
        3rdParty/DTrack.hpp
)

target_link_libraries(VistaDTrackSDKDriver
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
        VistaMath
        ${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_lib(VistaDTrackSDKDriver)
vista_install(VistaDTrackSDKDriver "VistaDeviceDrivers/VistaDTrackSDKDriver" "DriverPlugins")
set_target_properties(VistaDTrackSDKDriver PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaDTrackSDKDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
set_property(TARGET VistaDTrackSDKDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver

