		 
# Transcoder

add_library(VistaSixenseTranscoder
        VistaSixenseCommonShare.h
        VistaSixenseTranscoder.cpp
)
target_link_libraries(VistaSixenseTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(VistaSixenseTranscoder)
vista_install(VistaSixenseTranscoder "VistaDeviceDrivers/VistaSixenseDriver" "DriverPlugins")
set_target_properties(VistaSixenseTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaSixenseTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# Driver Plugin
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package(SixenseSDK QUIET)

if (NOT SIXENSESDK_FOUND)
    message(STATUS "VistaOpenCVCaptureDriver requires OpenCV, which is not available - only transcoder will be built")
else ()

    add_library(VistaSixensePlugin
            VistaSixensePlugin.cpp
    )
    target_link_libraries(VistaSixensePlugin
            VistaSixenseDriver
            VistaDeviceDriversBase
    )
    vista_configure_lib(VistaSixensePlugin)
    vista_install(VistaSixensePlugin "VistaDeviceDrivers/VistaSixenseDriver" "DriverPlugins")
    set_target_properties(VistaSixensePlugin PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaSixensePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

    # Actual Driver
    vista_use_package(SixenseSDK REQUIRED)

    add_library(VistaSixenseDriver
            VistaSixenseCommonShare.h
            VistaSixenseDriver.h
            VistaSixenseDriver.cpp
    )

    target_link_libraries(VistaSixenseDriver
            VistaBase
            VistaAspects
            VistaInterProcComm
            VistaDeviceDriversBase
            VistaMath
            ${VISTA_USE_PACKAGE_LIBRARIES}
    )
    vista_configure_lib(VistaSixenseDriver)
    vista_install(VistaSixenseDriver "VistaDeviceDrivers/VistaSixenseDriver" "DriverPlugins")
    set_target_properties(VistaSixenseDriver PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaSixenseDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
    set_property(TARGET VistaSixenseDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver
endif ()

