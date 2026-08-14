add_library(VistaFastrakTranscoder SHARED
    VistaFastrakCommonShare.h
    VistaFastrakTranscoder.cpp
)

target_link_libraries(VistaFastrakTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaFastrakTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAFASTRAKTRANSCODER_EXPORTS>
)

add_library(VistaFastrakPlugin SHARED
    VistaFastrakPlugin.cpp
)

target_link_libraries(VistaFastrakPlugin
  PUBLIC
    VistaFastrakDriver
    VistaDeviceDriversBase
)

target_compile_definitions(VistaFastrakPlugin
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAFASTRAKPLUGIN_EXPORTS>
)

add_library(VistaFastrakDriver SHARED
    VistaFastrakDriver.cpp
)

target_sources(VistaFastrakDriver
  PUBLIC
    FILE_SET HEADERS
    FILES
      VistaFastrakCommonShare.h
      VistaFastrakDriver.h
)

target_compile_definitions(VistaFastrakDriver
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAFASTRAKDRIVER_EXPORTS>
)

target_link_libraries(VistaFastrakDriver
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    VistaMath
)

install(
    TARGETS VistaFastrakTranscoder VistaFastrakPlugin VistaFastrakDriver
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaFastrakDriver"
)
