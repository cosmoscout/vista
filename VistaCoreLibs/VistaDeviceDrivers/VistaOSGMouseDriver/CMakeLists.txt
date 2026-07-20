		 
# Transcoder

add_library(VistaOSGMouseTranscoder
        VistaOSGMouseTranscoder.cpp
)
target_link_libraries(VistaOSGMouseTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(VistaOSGMouseTranscoder)
vista_install(VistaOSGMouseTranscoder "VistaDeviceDrivers/VistaOSGMouseDriver" "DriverPlugins")
set_target_properties(VistaOSGMouseTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaOSGMouseTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package(OpenSceneGraph COMPONENTS osgViewer osgGA QUIET)

if (NOT OSG_FOUND)
    message(STATUS "VistaOSGMouseDriver requires OpenSceneGraph, which is not available - only transcoder will be built")
else (NOT OSG_FOUND)

    # Driver Plugin

    add_library(VistaOSGMousePlugin
            VistaOSGMousePlugin.cpp
    )
    target_link_libraries(VistaOSGMousePlugin
            VistaOSGMouseDriver
            VistaDeviceDriversBase
    )
    vista_configure_lib(VistaOSGMousePlugin)
    vista_install(VistaOSGMousePlugin "VistaDeviceDrivers/VistaOSGMouseDriver" "DriverPlugins")
    set_target_properties(VistaOSGMousePlugin PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaOSGMousePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

    # Actual Driver

    vista_use_package(OpenSceneGraph REQUIRED osgViewer osgGA)

    add_library(VistaOSGMouseDriver
            VistaOSGMouseDriver.h
            VistaOSGMouseDriver.cpp
    )

    target_link_libraries(VistaOSGMouseDriver
            VistaBase
            VistaAspects
            VistaInterProcComm
            VistaDeviceDriversBase
            VistaMath
            VistaKernel
            ${VISTA_USE_PACKAGE_LIBRARIES}
    )
    vista_configure_lib(VistaOSGMouseDriver)
    vista_install(VistaOSGMouseDriver "VistaDeviceDrivers/VistaOSGMouseDriver" "DriverPlugins")
    set_target_properties(VistaOSGMouseDriver PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaOSGMouseDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
    set_property(TARGET VistaOSGMouseDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver
endif (NOT OSG_FOUND)
