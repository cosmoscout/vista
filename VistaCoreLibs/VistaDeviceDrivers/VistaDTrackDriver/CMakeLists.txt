add_library(VistaDTrackTranscoder SHARED
    VistaDTrackCommonShare.h
    VistaDTrackTranscoder.cpp
)

target_link_libraries(VistaDTrackTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaDTrackTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTADTRACKTRANSCODER_EXPORTS>
)

add_library(VistaDTrackPlugin SHARED
    VistaDTrackPlugin.cpp
)

target_link_libraries(VistaDTrackPlugin
  PUBLIC
    VistaDTrackDriver
    VistaDeviceDriversBase
)

target_compile_definitions(VistaDTrackPlugin
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTADTRACKPLUGIN_EXPORTS>
)

add_library(VistaDTrackDriver SHARED
    VistaDTrackDriver.cpp
)

target_sources(VistaDTrackDriver
  PUBLIC
    FILE_SET HEADERS
    FILES
      VistaDTrackCommonShare.h
      VistaDTrackDriver.h
)

target_compile_definitions(VistaDTrackDriver
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTADTRACKDRIVER_EXPORTS>
)

target_link_libraries(VistaDTrackDriver
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    VistaMath
)

install(
    TARGETS VistaDTrackTranscoder VistaDTrackPlugin VistaDTrackDriver
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaDTrackDriver"
)
