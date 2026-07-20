		 
# Transcoder

add_library(VistaOpenCVCaptureTranscoder
        VistaOpenCVCaptureTranscoder.cpp
)
target_link_libraries(VistaOpenCVCaptureTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(VistaOpenCVCaptureTranscoder)
vista_install(VistaOpenCVCaptureTranscoder "VistaDeviceDrivers/VistaOpenCVCaptureDriver" "DriverPlugins")
set_target_properties(VistaOpenCVCaptureTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(VistaOpenCVCaptureTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package(OpenCV QUIET)

if (NOT OPENCV_FOUND)
    message(STATUS "VistaOpenCVCaptureDriver requires OpenCV, which is not available - only transcoder will be built")
else (NOT OPENCV_FOUND)

    # Driver Plugin

    add_library(VistaOpenCVCapturePlugin
            VistaOpenCVCapturePlugin.cpp
    )
    target_link_libraries(VistaOpenCVCapturePlugin
            VistaOpenCVCaptureDriver
            VistaDeviceDriversBase
    )
    vista_configure_lib(VistaOpenCVCapturePlugin)
    vista_install(VistaOpenCVCapturePlugin "VistaDeviceDrivers/VistaOpenCVCaptureDriver" "DriverPlugins")
    set_target_properties(VistaOpenCVCapturePlugin PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaOpenCVCapturePlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

    # Actual Driver

    vista_use_package(OpenCV REQUIRED)

    add_library(VistaOpenCVCaptureDriver
            VistaOpenCVCaptureDriver.h
            VistaOpenCVCaptureDriver.cpp
    )

    target_link_libraries(VistaOpenCVCaptureDriver
            VistaBase
            VistaAspects
            VistaInterProcComm
            VistaDeviceDriversBase
            VistaMath
            ${VISTA_USE_PACKAGE_LIBRARIES}
    )
    vista_configure_lib(VistaOpenCVCaptureDriver)
    vista_install(VistaOpenCVCaptureDriver "VistaDeviceDrivers/VistaOpenCVCaptureDriver" "DriverPlugins")
    set_target_properties(VistaOpenCVCaptureDriver PROPERTIES FOLDER "Drivers")
    vista_set_outdir(VistaOpenCVCaptureDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
    set_property(TARGET VistaOpenCVCaptureDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver
endif (NOT OPENCV_FOUND)
