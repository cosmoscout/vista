add_library(VistaVRPNTranscoder SHARED
    VistaVRPNTranscoder.cpp
)

target_link_libraries(VistaVRPNTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaVRPNTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAVRPNTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaVRPNTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaVRPNDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(VRPN QUIET)

if (NOT VRPN_FOUND)
  message(STATUS "VistaVRPNDriver requires VRPN, which is not available - only transcoder will be built")
else ()
  add_library(VistaVRPNPlugin SHARED
      VistaVRPNPlugin.cpp
  )

  target_link_libraries(VistaVRPNPlugin
    PUBLIC
      VistaVRPNDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaVRPNPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAVRPNPLUGIN_EXPORTS>
  )

  add_library(VistaVRPNDriver SHARED
      VistaVRPNDriver.cpp
  )

  target_sources(VistaVRPNDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaVRPNDriver.h
  )

  target_compile_definitions(VistaVRPNDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAVRPNDRIVER_EXPORTS>
  )

  target_link_libraries(VistaVRPNDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      VRPN::vrpn
  )

  install(
      TARGETS VistaVRPNPlugin VistaVRPNDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaVRPNDriver"
  )
endif ()
