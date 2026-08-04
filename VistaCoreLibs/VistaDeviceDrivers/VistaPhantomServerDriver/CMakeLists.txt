add_library(VistaPhantomServerTranscoder SHARED
    VistaPhantomServerCommonShare.h
    VistaPhantomServerTranscoder.cpp
)

target_link_libraries(VistaPhantomServerTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaPhantomServerTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAPHANTOMSERVERTRANSCODER_EXPORTS>
)

add_library(VistaPhantomServerPlugin SHARED
    VistaPhantomServerPlugin.cpp
)

target_link_libraries(VistaPhantomServerPlugin
  PUBLIC
    VistaPhantomServerDriver
    VistaDeviceDriversBase
)

target_compile_definitions(VistaPhantomServerPlugin
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAPHANTOMSERVERPLUGIN_EXPORTS>
)

add_library(VistaPhantomServerDriver SHARED
    VistaPhantomServerDriver.cpp
)

target_sources(VistaPhantomServerDriver
  PUBLIC
    FILE_SET HEADERS
    FILES
      VistaPhantomServerCommonShare.h
      VistaPhantomServerDriver.h
)

target_compile_definitions(VistaPhantomServerDriver
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAPHANTOMSERVERDRIVER_EXPORTS>
)

target_link_libraries(VistaPhantomServerDriver
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    VistaMath
)

install(
    TARGETS VistaPhantomServerTranscoder VistaPhantomServerPlugin VistaPhantomServerDriver
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaPhantomServerDriver"
)
