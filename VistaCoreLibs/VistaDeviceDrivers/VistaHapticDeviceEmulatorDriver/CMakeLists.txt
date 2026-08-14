add_library(VistaHapticDeviceEmulatorTranscoder SHARED
    VistaHapticDeviceEmulatorCommonShare.h
    VistaHapticDeviceEmulatorTranscoder.cpp
)

target_link_libraries(VistaHapticDeviceEmulatorTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaHapticDeviceEmulatorTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAHAPTICDEVICEEMULATORTRANSCODER_EXPORTS>
)

add_library(VistaHapticDeviceEmulatorPlugin SHARED
    VistaHapticDeviceEmulatorPlugin.cpp
)

target_link_libraries(VistaHapticDeviceEmulatorPlugin
  PUBLIC
    VistaHapticDeviceEmulatorDriver
    VistaDeviceDriversBase
)

target_compile_definitions(VistaHapticDeviceEmulatorPlugin
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAHAPTICDEVICEEMULATORPLUGIN_EXPORTS>
)

add_library(VistaHapticDeviceEmulatorDriver SHARED
    VistaHapticDeviceEmulatorDriver.cpp
)

target_sources(VistaHapticDeviceEmulatorDriver
  PUBLIC
    FILE_SET HEADERS
    FILES
      VistaHapticDeviceEmulatorCommonShare.h
      VistaHapticDeviceEmulatorDriver.h
)

target_compile_definitions(VistaHapticDeviceEmulatorDriver
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAHAPTICDEVICEEMULATORDRIVER_EXPORTS>
)

target_link_libraries(VistaHapticDeviceEmulatorDriver
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    VistaMath
)

install(
    TARGETS VistaHapticDeviceEmulatorTranscoder VistaHapticDeviceEmulatorPlugin VistaHapticDeviceEmulatorDriver
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaHapticDeviceEmulatorDriver"
)
