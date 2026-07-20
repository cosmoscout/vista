		 
# Transcoder

add_library(VistaSDL2RawKeyboardTranscoder
        VistaSDL2RawKeyboardTranscoder.cpp
)
target_link_libraries(VistaSDL2RawKeyboardTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(VistaSDL2RawKeyboardTranscoder)
vista_install(VistaSDL2RawKeyboardTranscoder "VistaDeviceDrivers/VistaSDL2RawKeyboardDriver" "DriverPlugins")
set_target_properties(VistaSDL2RawKeyboardTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaSDL2RawKeyboardTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package(SDL2 QUIET)

if (NOT SDL2_FOUND)
    message(STATUS "VistaSDL2RawKeyboardDriver requires SDL2, which is not available - only transcoder will be built")
else (NOT SDL2_FOUND)
    # Driver Plugin

    add_library(VistaSDL2RawKeyboardPlugin
            VistaSDL2RawKeyboardPlugin.cpp
    )
    target_link_libraries(VistaSDL2RawKeyboardPlugin
            VistaSDL2RawKeyboardDriver
            VistaDeviceDriversBase
    )
    vista_configure_lib(VistaSDL2RawKeyboardPlugin)
    vista_install(VistaSDL2RawKeyboardPlugin "VistaDeviceDrivers/VistaSDL2RawKeyboardDriver" "DriverPlugins")
    set_target_properties(VistaSDL2RawKeyboardPlugin PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaSDL2RawKeyboardPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

    # Actual Driver

    vista_use_package(SDL2 REQUIRED)

    add_library(VistaSDL2RawKeyboardDriver
            VistaSDL2RawKeyboardDriver.h
            VistaSDL2RawKeyboardDriver.cpp
    )

    target_link_libraries(VistaSDL2RawKeyboardDriver
            VistaBase
            VistaAspects
            VistaInterProcComm
            VistaDeviceDriversBase
            VistaMath
            ${VISTA_USE_PACKAGE_LIBRARIES}
    )
    vista_configure_lib(VistaSDL2RawKeyboardDriver)
    vista_install(VistaSDL2RawKeyboardDriver "VistaDeviceDrivers/VistaSDL2RawKeyboardDriver" "DriverPlugins")
    set_target_properties(VistaSDL2RawKeyboardDriver PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaSDL2RawKeyboardDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
    set_property(TARGET VistaSDL2RawKeyboardDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver
endif (NOT SDL2_FOUND)
