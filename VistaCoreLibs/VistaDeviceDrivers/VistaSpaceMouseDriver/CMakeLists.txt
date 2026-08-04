add_library(VistaSpaceMouseTranscoder SHARED
    VistaSpaceMouseCommonShare.h
    VistaSpaceMouseTranscoder.cpp
)

target_link_libraries(VistaSpaceMouseTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaSpaceMouseTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTASPACEMOUSETRANSCODER_EXPORTS>
)

add_library(VistaSpaceMousePlugin SHARED
    VistaSpaceMousePlugin.cpp
)

target_link_libraries(VistaSpaceMousePlugin
  PUBLIC
    VistaSpaceMouseDriver
    VistaDeviceDriversBase
)

target_compile_definitions(VistaSpaceMousePlugin
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTASPACEMOUSEPLUGIN_EXPORTS>
)

add_library(VistaSpaceMouseDriver SHARED
    VistaSpaceMouseDriver.cpp
)

target_sources(VistaSpaceMouseDriver
  PUBLIC
    FILE_SET HEADERS
    FILES
      VistaSpaceMouseCommonShare.h
      VistaSpaceMouseDriver.h
)

target_compile_definitions(VistaSpaceMouseDriver
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTASPACEMOUSEDRIVER_EXPORTS>
)

target_link_libraries(VistaSpaceMouseDriver
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    VistaMath
)

install(
    TARGETS VistaSpaceMouseTranscoder VistaSpaceMousePlugin VistaSpaceMouseDriver
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSpaceMouseDriver"
)
