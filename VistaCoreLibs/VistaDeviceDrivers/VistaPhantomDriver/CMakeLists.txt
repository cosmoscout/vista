add_library(VistaOpenHapticsTranscoder SHARED
    VistaOpenHapticsTranscoder.cpp
    VistaPhantomCommonShare.h
)

target_link_libraries(VistaOpenHapticsTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaOpenHapticsTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAOPENHAPTICSTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaOpenHapticsTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenHapticsDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(OpenHaptics QUIET)

if (NOT OPENHAPTICS_FOUND)
  message(STATUS "VistaOpenHapticsDriver requires OpenHaptics, which is not available - only transcoder will be built")
else ()
  add_library(VistaOpenHapticsPlugin SHARED
      VistaOpenHapticsPlugin.cpp
  )

  target_link_libraries(VistaOpenHapticsPlugin
    PUBLIC
      VistaOpenHapticsDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaOpenHapticsPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENHAPTICSPLUGIN_EXPORTS>
  )

  add_library(VistaOpenHapticsDriver SHARED
      VistaOpenHapticsDriver.cpp
  )

  target_sources(VistaOpenHapticsDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaOpenHapticsDriver.h
        VistaPhantomCommonShare.h
  )

  target_compile_definitions(VistaOpenHapticsDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENHAPTICSDRIVER_EXPORTS>
  )

  target_link_libraries(VistaOpenHapticsDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      OpenHaptics::HD
      OpenHaptics::HDU
  )

  install(
      TARGETS VistaOpenHapticsPlugin VistaOpenHapticsDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenHapticsDriver"
  )
endif ()
