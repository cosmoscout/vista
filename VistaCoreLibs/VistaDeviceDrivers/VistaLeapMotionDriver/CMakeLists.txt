add_library(VistaLeapMotionTranscoder SHARED
    VistaLeapMotionCommonShare.h
    VistaLeapMotionTranscoder.cpp
)

target_link_libraries(VistaLeapMotionTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaLeapMotionTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTALEAPMOTIONTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaLeapMotionTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaLeapMotionDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(LEAPSDK QUIET)

if (NOT LEAPSDK_FOUND)
  message(STATUS "VistaLeapMotionDriver requires LeapSDK, which is not available - only transcoder will be built")
else ()
  add_library(VistaLeapMotionPlugin SHARED
      VistaLeapMotionPlugin.cpp
  )

  target_link_libraries(VistaLeapMotionPlugin
    PUBLIC
      VistaLeapMotionDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaLeapMotionPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTALEAPMOTIONPLUGIN_EXPORTS>
  )

  add_library(VistaLeapMotionDriver SHARED
      VistaLeapMotionDriver.cpp
  )

  target_sources(VistaLeapMotionDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaLeapMotionDriver.h
        VistaLeapMotionCommonShare.h
  )

  target_compile_definitions(VistaLeapMotionDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTALEAPMOTIONDRIVER_EXPORTS>
  )

  target_link_libraries(VistaLeapMotionDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      Leap::SDK
  )

  install(
      TARGETS VistaLeapMotionPlugin VistaLeapMotionDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaLeapMotionDriver"
  )
endif ()
