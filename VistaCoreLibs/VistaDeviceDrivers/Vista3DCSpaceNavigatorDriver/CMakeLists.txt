		 
# Transcoder

add_library(Vista3DCSpaceNavigatorTranscoder
		Vista3DCSpaceNavigatorTranscoder.cpp
)
target_link_libraries(Vista3DCSpaceNavigatorTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
vista_configure_lib(Vista3DCSpaceNavigatorTranscoder)
vista_install(Vista3DCSpaceNavigatorTranscoder "VistaDeviceDrivers/Vista3DCSpaceNavigatorDriver" "DriverPlugins")
set_target_properties(Vista3DCSpaceNavigatorTranscoder PROPERTIES FOLDER "Drivers")
vista_set_outdir(Vista3DCSpaceNavigatorTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

if (WIN32)
    vista_use_package(3dcsdk QUIET)
    if (NOT 3DCSDK_FOUND)
        message(STATUS "3DC SDK not found (ROOT dir set?), going for COM based 3DC driver")
        try_compile(_CAN_BUILD_COMBASED_DRIVER "${CMAKE_CURRENT_BINARY_DIR}/CMakeTest" "${CMAKE_CURRENT_SOURCE_DIR}/Vista3DCSpaceNavigatorCMakeTest.cpp")
        if (NOT _CAN_BUILD_COMBASED_DRIVER)
            message(STATUS "COM-based Vista3DCSpaceNavigatorDriver requires running access to installed driver and ATL, either of them is not available.")
        else (NOT _CAN_BUILD_COMBASED_DRIVER)
            set(_CAN_BUILD_DRIVER 1)
        endif (NOT _CAN_BUILD_COMBASED_DRIVER)
    else (NOT 3DCSDK_FOUND)
        message(STATUS "3dxsdk installed. Building driver based on 3DC-SDK.")
        set(_CAN_BUILD_DRIVER 1)
        set(_CAN_BUILD_3DXSDK_DRIVER 1)
        include_directories(${3DXSDK_INCLUDE_DIRS})
    endif (NOT 3DCSDK_FOUND)

else (WIN32)
    set(_CAN_BUILD_DRIVER 1)
endif (WIN32)


if (_CAN_BUILD_DRIVER)
    add_library(Vista3DCSpaceNavigatorPlugin
            Vista3DCSpaceNavigatorPlugin.cpp
    )
    target_link_libraries(Vista3DCSpaceNavigatorPlugin
            Vista3DCSpaceNavigatorDriver
            VistaDeviceDriversBase
    )
    vista_configure_lib(Vista3DCSpaceNavigatorPlugin)
    vista_install(Vista3DCSpaceNavigatorPlugin "VistaDeviceDrivers/Vista3DCSpaceNavigatorDriver" "DriverPlugins")
    set_target_properties(Vista3DCSpaceNavigatorPlugin PROPERTIES FOLDER "Drivers")
    vista_set_outdir(Vista3DCSpaceNavigatorPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")

    # Actual Driver
    add_library(Vista3DCSpaceNavigatorDriver
            Vista3DCSpaceNavigatorDriver.h
            $<$<BOOL:${_CAN_BUILD_3DXSDK_DRIVER}>:Vista3DCSpaceNavigatorDriver_sdk.cpp>
            $<$<NOT:$<BOOL:${_CAN_BUILD_3DXSDK_DRIVER}>>:Vista3DCSpaceNavigatorDriver.cpp>
    )

    target_link_libraries(Vista3DCSpaceNavigatorDriver
            VistaBase
            VistaAspects
            VistaInterProcComm
            VistaDeviceDriversBase
            VistaMath
            ${VISTA_USE_PACKAGE_LIBRARIES}
    )
    vista_configure_lib(Vista3DCSpaceNavigatorDriver)
    vista_install(Vista3DCSpaceNavigatorDriver "VistaDeviceDrivers/Vista3DCSpaceNavigatorDriver" "DriverPlugins")
    set_target_properties(Vista3DCSpaceNavigatorDriver PROPERTIES FOLDER "Drivers")
    vista_set_outdir(Vista3DCSpaceNavigatorDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins")
    set_property(TARGET Vista3DCSpaceNavigatorDriver PROPERTY LINK_INTERFACE_LIBRARIES "") # prevents auto-includion of linked libraries to other targets linking the driver

endif (_CAN_BUILD_DRIVER)