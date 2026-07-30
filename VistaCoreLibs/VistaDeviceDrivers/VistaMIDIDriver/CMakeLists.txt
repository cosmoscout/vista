		 
# Transcoder

add_library(VistaMIDITranscoder
        VistaMIDICommonShare.h
        VistaMIDITranscoder.cpp
)
target_link_libraries(VistaMIDITranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(VistaMIDITranscoder)
vista_install(VistaMIDITranscoder "VistaDeviceDrivers/VistaMIDIDriver" "DriverPlugins")
set_target_properties(VistaMIDITranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaMIDITranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Driver Plugin

add_library(VistaMIDIPlugin
        VistaMIDIPlugin.cpp
)
target_link_libraries(VistaMIDIPlugin
        VistaMIDIDriver
        VistaDeviceDriversBase
)
vista_configure_lib(VistaMIDIPlugin)
vista_install(VistaMIDIPlugin "VistaDeviceDrivers/VistaMIDIDriver" "DriverPlugins")
set_target_properties(VistaMIDIPlugin PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaMIDIPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Actual Driver

add_library(VistaMIDIDriver
        VistaMIDICommonShare.h
        VistaMIDIDriver.h
        VistaMIDIDriver.cpp
)

target_link_libraries(VistaMIDIDriver
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
        VistaMath
        ${VISTA_USE_PACKAGE_LIBRARIES}
)
vista_configure_lib(VistaMIDIDriver)
vista_install(VistaMIDIDriver "VistaDeviceDrivers/VistaMIDIDriver" "DriverPlugins")
set_target_properties(VistaMIDIDriver PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaMIDIDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
set_property(TARGET VistaMIDIDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver

