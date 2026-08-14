add_library(VistaOpenVRTranscoder SHARED
    VistaOpenVRDriverTranscoder.cpp
)

target_link_libraries(VistaOpenVRTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaOpenVRTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAOPENVRDRIVERTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaOpenVRTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenVRDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(OpenVR QUIET)

if (NOT OPENVR_FOUND)
  message(STATUS "VistaOpenVRDriver requires OpenVR, which is not available - only transcoder will be built")
else ()
  add_library(VistaOpenVRPlugin SHARED
      VistaOpenVRDriverPlugin.h
      VistaOpenVRDriverPlugin.cpp
  )

  target_link_libraries(VistaOpenVRPlugin
    PUBLIC
      VistaOpenVRDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaOpenVRPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENVRDRIVERPLUGINDLL_EXPORTS>
    PUBLIC
      $<$<PLATFORM_ID:Windows>:VISTAOPENVRDRIVERPLUGINDLL>
  )

  add_library(VistaOpenVRDriver SHARED
      VistaOpenVRDriver.cpp
  )

  target_sources(VistaOpenVRDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaOpenVRDriver.h
        VistaOpenVRDriverConfig.h
  )

  target_compile_definitions(VistaOpenVRDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENVR_EXPORTS>
  )

  target_link_libraries(VistaOpenVRDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      OpenVR::API
  )

  install(
      TARGETS VistaOpenVRPlugin VistaOpenVRDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenVRDriver"
  )
endif ()
