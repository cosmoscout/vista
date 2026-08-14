add_library(VistaHIDTranscoder SHARED
    VistaHIDCommonShare.h
    VistaHIDTranscoder.cpp
)

target_link_libraries(VistaHIDTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaHIDTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAHIDTRANSCODER_EXPORTS>
)

add_library(VistaHIDPlugin SHARED
    VistaHIDPlugin.cpp
)

target_link_libraries(VistaHIDPlugin
  PUBLIC
    VistaHIDDriver
    VistaDeviceDriversBase
)

target_compile_definitions(VistaHIDPlugin
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAHIDPLUGIN_EXPORTS>
)

add_library(VistaHIDDriver SHARED
    VistaHIDDriver.cpp
    VistaHIDDriverForceFeedbackAspect.cpp
)

target_sources(VistaHIDDriver
  PUBLIC
    FILE_SET HEADERS
    FILES
      VistaHIDCommonShare.h
      VistaHIDDriver.h
      VistaHIDDriverForceFeedbackAspect.h
)

target_compile_definitions(VistaHIDDriver
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAHIDDRIVER_EXPORTS>
)

target_link_libraries(VistaHIDDriver
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    VistaMath
)

install(
    TARGETS VistaHIDTranscoder VistaHIDPlugin VistaHIDDriver
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaHIDDriver"
)
