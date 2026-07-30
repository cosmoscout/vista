		 
# Transcoder

add_library(VistaOSGKeyboardTranscoder
        VistaOSGKeyboardTranscoder.cpp
)
target_link_libraries(VistaOSGKeyboardTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(VistaOSGKeyboardTranscoder)
vista_install(VistaOSGKeyboardTranscoder "VistaDeviceDrivers/VistaOSGKeyboardDriver" "DriverPlugins")
set_target_properties(VistaOSGKeyboardTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaOSGKeyboardTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package(OpenSceneGraph COMPONENTS osgViewer osgGA QUIET)

if (NOT OSG_FOUND)
    message(STATUS "VistaOSGKeyboardDriver requires OSG, which is not available - only transcoder will be built")
else (NOT OSG_FOUND)
    # Driver Plugin

    add_library(VistaOSGKeyboardPlugin
            VistaOSGKeyboardPlugin.cpp
    )
    target_link_libraries(VistaOSGKeyboardPlugin
            VistaOSGKeyboardDriver
            VistaDeviceDriversBase
    )
    vista_configure_lib(VistaOSGKeyboardPlugin)
    vista_install(VistaOSGKeyboardPlugin "VistaDeviceDrivers/VistaOSGKeyboardDriver" "DriverPlugins")
    set_target_properties(VistaOSGKeyboardPlugin PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaOSGKeyboardPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

    # Actual Driver

    vista_use_package(OpenSceneGraph REQUIRED osgViewer osgGA)

    add_library(VistaOSGKeyboardDriver
            VistaOSGKeyboardDriver.h
            VistaOSGKeyboardDriver.cpp
    )

    target_link_libraries(VistaOSGKeyboardDriver
            VistaBase
            VistaAspects
            VistaInterProcComm
            VistaDeviceDriversBase
            VistaMath
            VistaKernel
            ${VISTA_USE_PACKAGE_LIBRARIES}
    )
    vista_configure_lib(VistaOSGKeyboardDriver)
    vista_install(VistaOSGKeyboardDriver "VistaDeviceDrivers/VistaOSGKeyboardDriver" "DriverPlugins")
    set_target_properties(VistaOSGKeyboardDriver PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaOSGKeyboardDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
    set_property(TARGET VistaOSGKeyboardDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver
endif (NOT OSG_FOUND)
