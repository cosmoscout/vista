add_library(VistaIRManTranscoder SHARED
    VistaIRManTranscoder.cpp
)

target_link_libraries(VistaIRManTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaIRManTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAIRMANTRANSCODER_EXPORTS>
)

add_library(VistaIRManPlugin SHARED
    VistaIRManPlugin.cpp
)

target_link_libraries(VistaIRManPlugin
  PUBLIC
    VistaIRManDriver
    VistaDeviceDriversBase
)

target_compile_definitions(VistaIRManPlugin
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAIRMANPLUGIN_EXPORTS>
)

add_library(VistaIRManDriver SHARED
    VistaIRManDriver.cpp
)

target_sources(VistaIRManDriver
  PUBLIC
    FILE_SET HEADERS
    FILES VistaIRManDriver.h
)

target_compile_definitions(VistaIRManDriver
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAIRMANDRIVER_EXPORTS>
)

target_link_libraries(VistaIRManDriver
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    VistaMath
)

install(
    TARGETS VistaIRManTranscoder VistaIRManPlugin VistaIRManDriver
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaIRManDriver"
)
