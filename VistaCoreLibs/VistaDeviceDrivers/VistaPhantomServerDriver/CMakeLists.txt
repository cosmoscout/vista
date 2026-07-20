		 
# Transcoder

add_library(VistaPhantomServerTranscoder
        VistaPhantomServerTranscoder.cpp
        VistaPhantomServerCommonShare.h
)
target_link_libraries(VistaPhantomServerTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(VistaPhantomServerTranscoder)
vista_install(VistaPhantomServerTranscoder "VistaDeviceDrivers/VistaPhantomServerDriver" "DriverPlugins")
set_target_properties(VistaPhantomServerTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaPhantomServerTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Driver Plugin

add_library(VistaPhantomServerPlugin
        VistaPhantomServerPlugin.cpp
)
target_link_libraries(VistaPhantomServerPlugin
        VistaPhantomServerDriver
        VistaDeviceDriversBase
)
vista_configure_lib(VistaPhantomServerPlugin)
vista_install(VistaPhantomServerPlugin "VistaDeviceDrivers/VistaPhantomServerDriver" "DriverPlugins")
set_target_properties(VistaPhantomServerPlugin PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaPhantomServerPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Actual Driver

add_library(VistaPhantomServerDriver
        VistaPhantomServerDriver.h
        VistaPhantomServerDriver.cpp
        VistaPhantomServerCommonShare.h
)

target_link_libraries(VistaPhantomServerDriver
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
        VistaMath
        ${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib(VistaPhantomServerDriver)
vista_install(VistaPhantomServerDriver "VistaDeviceDrivers/VistaPhantomServerDriver" "DriverPlugins")
set_target_properties(VistaPhantomServerDriver PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaPhantomServerDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
set_property(TARGET VistaPhantomServerDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver

