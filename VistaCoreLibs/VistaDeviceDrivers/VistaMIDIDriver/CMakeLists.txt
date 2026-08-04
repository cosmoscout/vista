add_library(VistaMIDITranscoder SHARED
    VistaMIDICommonShare.h
    VistaMIDITranscoder.cpp
)

target_link_libraries(VistaMIDITranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaMIDITranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAMIDITRANSCODER_EXPORTS>
)

add_library(VistaMIDIPlugin SHARED
    VistaMIDIPlugin.cpp
)

target_link_libraries(VistaMIDIPlugin
  PUBLIC
    VistaMIDIDriver
    VistaDeviceDriversBase
)

target_compile_definitions(VistaMIDIPlugin
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAMIDIPLUGIN_EXPORTS>
)

add_library(VistaMIDIDriver SHARED
    VistaMIDIDriver.cpp
)

target_sources(VistaMIDIDriver
  PUBLIC
    FILE_SET HEADERS
    FILES
      VistaMIDICommonShare.h
      VistaMIDIDriver.h
)

target_compile_definitions(VistaMIDIDriver
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAMIDIDRIVER_EXPORTS>
)

target_link_libraries(VistaMIDIDriver
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    VistaMath
)

install(
    TARGETS VistaMIDITranscoder VistaMIDIPlugin VistaMIDIDriver
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaMIDIDriver"
)
