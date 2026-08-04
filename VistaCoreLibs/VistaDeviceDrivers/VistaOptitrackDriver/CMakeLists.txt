add_library(VistaOptitrackTranscoder SHARED
    VistaOptitrackTranscoder.cpp
    VistaOptitrackCommonShare.h
)

target_link_libraries(VistaOptitrackTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaOptitrackTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAOPTITRACKTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaOptitrackTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOptitrackDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(NatNetSDK QUIET)

if (NOT NATNETSDK_FOUND)
  message(STATUS "VistaOptitrackDriver requires NatNetSDK, which is not available - only transcoder will be built")
else ()
  add_library(VistaOptitrackPlugin SHARED
      VistaOptitrackPlugin.cpp
  )

  target_link_libraries(VistaOptitrackPlugin
    PUBLIC
      VistaOptitrackDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaOptitrackPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPTITRACKPLUGIN_EXPORTS>
  )

  add_library(VistaOptitrackDriver SHARED
      VistaOptitrackDriver.cpp
  )

  target_sources(VistaOptitrackDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaOptitrackDriver.h
        VistaOptitrackCommonShare.h
  )

  target_compile_definitions(VistaOptitrackDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPTITRACKDRIVER_EXPORTS>
  )

  target_link_libraries(VistaOptitrackDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      NatNet::SDK
  )

  install(
      TARGETS VistaOptitrackPlugin VistaOptitrackDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOptitrackDriver"
  )
endif ()
