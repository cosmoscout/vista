add_library(VistaCyberGloveTranscoder SHARED
    VistaCyberGloveCommonShare.h
    VistaCyberGloveTranscoder.cpp
)

target_link_libraries(VistaCyberGloveTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaCyberGloveTranscoder
    PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTACYBERGLOVETRANSCODER_EXPORTS>
)

add_library(VistaCyberGlovePlugin SHARED
    VistaCyberGlovePlugin.cpp
)

target_link_libraries(VistaCyberGlovePlugin
  PUBLIC
    VistaCyberGloveDriver
    VistaDeviceDriversBase
)

target_compile_definitions(VistaCyberGlovePlugin
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTACYBERGLOVEPLUGIN_EXPORTS>
)

add_library(VistaCyberGloveDriver SHARED
    VistaCyberGloveDriver.cpp
)

target_sources(VistaCyberGloveDriver
  PUBLIC
    FILE_SET HEADERS
    FILES
      VistaCyberGloveCommonShare.h
      VistaCyberGloveDriver.h
)

target_compile_definitions(VistaCyberGloveDriver
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTACYBERGLOVEDRIVER_EXPORTS>
)

target_link_libraries(VistaCyberGloveDriver
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    VistaMath
)

install(
    TARGETS VistaCyberGloveTranscoder VistaCyberGlovePlugin VistaCyberGloveDriver
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaCyberGloveDriver"
)
