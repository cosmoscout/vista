add_library(VistaJswJoystickTranscoder SHARED
    VistaJswJoystickTranscoder.cpp
)

target_link_libraries(VistaJswJoystickTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaJswJoystickTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAJSWJOYSTICKTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaJswJoystickTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaJswJoystickDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(JSW QUIET)

if (NOT JSW_FOUND)
  message(STATUS "VistaJswJoystickDriver requires JSW, which is not available - only transcoder will be built")
else ()
  add_library(VistaJswJoystickPlugin SHARED
      VistaJswJoystickPlugin.cpp
  )

  target_link_libraries(VistaJswJoystickPlugin
    PUBLIC
      VistaJswJoystickDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaJswJoystickPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAJSWJOYSTICKPLUGIN_EXPORTS>
  )

  add_library(VistaJswJoystickDriver SHARED
      VistaJswJoystickDriver.cpp
  )

  target_sources(VistaJswJoystickDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaJswJoystickDriver.h
  )

  target_compile_definitions(VistaJswJoystickDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAJSWJOYSTICKDRIVER_EXPORTS>
  )

  target_link_libraries(VistaJswJoystickDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      JSW::JSW
  )

  install(
      TARGETS VistaJswJoystickPlugin VistaJswJoystickDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaJswJoystickDriver"
  )
endif ()
