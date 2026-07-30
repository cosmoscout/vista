		 
# Transcoder

add_library(VistaSpaceMouseTranscoder
        VistaSpaceMouseTranscoder.cpp
        VistaSpaceMouseCommonShare.h
)
target_link_libraries(VistaSpaceMouseTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(VistaSpaceMouseTranscoder)
vista_install(VistaSpaceMouseTranscoder "VistaDeviceDrivers/VistaSpaceMouseDriver" "DriverPlugins")
set_target_properties(VistaSpaceMouseTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaSpaceMouseTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Driver Plugin

add_library(VistaSpaceMousePlugin
        VistaSpaceMousePlugin.cpp
)
target_link_libraries(VistaSpaceMousePlugin
        VistaSpaceMouseDriver
        VistaDeviceDriversBase
)
vista_configure_lib(VistaSpaceMousePlugin)
vista_install(VistaSpaceMousePlugin "VistaDeviceDrivers/VistaSpaceMouseDriver" "DriverPlugins")
set_target_properties(VistaSpaceMousePlugin PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaSpaceMousePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Actual Driver

add_library(VistaSpaceMouseDriver
        VistaSpaceMouseDriver.h
        VistaSpaceMouseDriver.cpp
        VistaSpaceMouseCommonShare.h
)

target_link_libraries(VistaSpaceMouseDriver
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
        VistaMath
        ${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib(VistaSpaceMouseDriver)
vista_install(VistaSpaceMouseDriver "VistaDeviceDrivers/VistaSpaceMouseDriver" "DriverPlugins")
set_target_properties(VistaSpaceMouseDriver PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaSpaceMouseDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
set_property(TARGET VistaSpaceMouseDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver

