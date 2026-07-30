		 
# Transcoder

add_library(VistaCyberGloveTranscoder
        VistaCyberGloveCommonShare.h
        VistaCyberGloveTranscoder.cpp
)

target_link_libraries(VistaCyberGloveTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)

vista_configure_lib(VistaCyberGloveTranscoder)
vista_install(VistaCyberGloveTranscoder "VistaDeviceDrivers/VistaCyberGloveDriver" "DriverPlugins")
set_target_properties(VistaCyberGloveTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaCyberGloveTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Driver Plugin

add_library(VistaCyberGlovePlugin
        VistaCyberGlovePlugin.cpp
)

target_link_libraries(VistaCyberGlovePlugin
        VistaCyberGloveDriver
        VistaDeviceDriversBase
)

vista_configure_lib(VistaCyberGlovePlugin)
vista_install(VistaCyberGlovePlugin "VistaDeviceDrivers/VistaCyberGloveDriver" "DriverPlugins")
set_target_properties(VistaCyberGlovePlugin PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaCyberGlovePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Actual Driver

add_library(VistaCyberGloveDriver
        VistaCyberGloveCommonShare.h
        VistaCyberGloveDriver.h
        VistaCyberGloveDriver.cpp
)

target_link_libraries(VistaCyberGloveDriver
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
        VistaMath
        ${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_lib(VistaCyberGloveDriver)
vista_install(VistaCyberGloveDriver "VistaDeviceDrivers/VistaCyberGloveDriver" "DriverPlugins")
set_target_properties(VistaCyberGloveDriver PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaCyberGloveDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
set_property(TARGET VistaCyberGloveDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver

