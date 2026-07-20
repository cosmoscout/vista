		 
# Transcoder

add_library(VistaLeapMotionTranscoder
        VistaLeapMotionTranscoder.cpp
        VistaLeapMotionCommonShare.h
)
target_link_libraries(VistaLeapMotionTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(VistaLeapMotionTranscoder)
vista_install(VistaLeapMotionTranscoder "VistaDeviceDrivers/VistaLeapMotionDriver" "DriverPlugins")
set_target_properties(VistaLeapMotionTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaLeapMotionTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package(LeapSDK 2.1+ QUIET)

if (NOT LEAPSDK_FOUND)
    message(STATUS "VistaLeapMotionDriver requires LeapSDK v2.1 or greater, which is not available - only transcoder will be built")
else ()

    # Driver Plugin

    add_library(VistaLeapMotionPlugin
            VistaLeapMotionPlugin.cpp
    )
    target_link_libraries(VistaLeapMotionPlugin
            VistaLeapMotionDriver
            VistaDeviceDriversBase
    )
    vista_configure_lib(VistaLeapMotionPlugin)
    vista_install(VistaLeapMotionPlugin "VistaDeviceDrivers/VistaLeapMotionDriver" "DriverPlugins")
    set_target_properties(VistaLeapMotionPlugin PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaLeapMotionPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

    # Actual Driver

    vista_use_package(LeapSDK REQUIRED)

    add_library(VistaLeapMotionDriver
            VistaLeapMotionDriver.h
            VistaLeapMotionDriver.cpp
            VistaLeapMotionCommonShare.h
    )

    target_link_libraries(VistaLeapMotionDriver
            VistaBase
            VistaAspects
            VistaInterProcComm
            VistaDeviceDriversBase
            VistaMath
            ${VISTA_USE_PACKAGE_LIBRARIES}
    )
    vista_configure_lib(VistaLeapMotionDriver)
    vista_install(VistaLeapMotionDriver "VistaDeviceDrivers/VistaLeapMotionDriver" "DriverPlugins")
    set_target_properties(VistaLeapMotionDriver PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaLeapMotionDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
    set_property(TARGET VistaLeapMotionDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver
endif ()
