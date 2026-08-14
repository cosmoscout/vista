add_library(VistaDepthSenseTranscoder SHARED
    VistaDepthSenseTranscoder.cpp
)

target_link_libraries(VistaDepthSenseTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaDepthSenseTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTADEPTHSENSETRANSCODER_EXPORTS>
)

install(
    TARGETS VistaDepthSenseTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaDepthSenseDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(DepthSenseSDK QUIET)

if (NOT DEPTHSENSESDK_FOUND)
  message(STATUS "VistaDepthSenseDriver requires DepthSenseSDK, which is not available - only transcoder will be built")
else ()
  add_library(VistaDepthSensePlugin SHARED
      VistaDepthSensePlugin.cpp
  )

  target_link_libraries(VistaDepthSensePlugin
    PUBLIC
      VistaDepthSenseDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaDepthSensePlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTADEPTHSENSEPLUGIN_EXPORTS>
  )

  add_library(VistaDepthSenseDriver SHARED
      VistaDepthSenseDriver.cpp
  )

  target_sources(VistaDepthSenseDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaDepthSenseDriver.h
  )

  target_compile_definitions(VistaDepthSenseDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTADEPTHSENSEDRIVER_EXPORTS>
  )

  target_link_libraries(VistaDepthSenseDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      DepthSense::SDK
  )

  install(
      TARGETS VistaDepthSensePlugin VistaDepthSenseDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaDepthSenseDriver"
  )
endif ()
