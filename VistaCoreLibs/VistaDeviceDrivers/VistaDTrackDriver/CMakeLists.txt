		 
# Transcoder

add_library(VistaDTrackTranscoder
        VistaDTrackCommonShare.h
        VistaDTrackTranscoder.cpp
)

target_link_libraries(VistaDTrackTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)

vista_configure_lib(VistaDTrackTranscoder)
vista_install(VistaDTrackTranscoder "VistaDeviceDrivers/VistaDTrackDriver" "DriverPlugins")
set_target_properties(VistaDTrackTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaDTrackTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Driver Plugin

add_library(VistaDTrackPlugin
        VistaDTrackPlugin.cpp
)

target_link_libraries(VistaDTrackPlugin
        VistaDTrackDriver
        VistaDeviceDriversBase
)

vista_configure_lib(VistaDTrackPlugin)
vista_install(VistaDTrackPlugin "VistaDeviceDrivers/VistaDTrackDriver" "DriverPlugins")
set_target_properties(VistaDTrackPlugin PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaDTrackPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Actual Driver

add_library(VistaDTrackDriver
        VistaDTrackCommonShare.h
        VistaDTrackDriver.h
        VistaDTrackDriver.cpp
)

target_link_libraries(VistaDTrackDriver
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
        VistaMath
        ${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_lib(VistaDTrackDriver)
vista_install(VistaDTrackDriver "VistaDeviceDrivers/VistaDTrackDriver" "DriverPlugins")
set_target_properties(VistaDTrackDriver PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaDTrackDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
set_property(TARGET VistaDTrackDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver

