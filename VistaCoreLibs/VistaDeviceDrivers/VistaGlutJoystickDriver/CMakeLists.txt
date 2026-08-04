add_library(VistaGlutJoystickTranscoder SHARED
    VistaGlutJoystickTranscoder.cpp
)

target_link_libraries(VistaGlutJoystickTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaGlutJoystickTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAGLUTJOYSTICKTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaGlutJoystickTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaGlutJoystickDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(GLUT QUIET)

if (NOT GLUT_FOUND)
  message(STATUS "VistaGlutJoystickDriver requires GLUT, which is not available - only transcoder will be built")
else ()
  add_library(VistaGlutJoystickPlugin SHARED
      VistaGlutJoystickPlugin.cpp
  )

  target_link_libraries(VistaGlutJoystickPlugin
    PUBLIC
      VistaGlutJoystickDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaGlutJoystickPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAGLUTJOYSTICKPLUGIN_EXPORTS>
  )

  add_library(VistaGlutJoystickDriver SHARED
      VistaGlutJoystickDriver.cpp
  )

  target_sources(VistaGlutJoystickDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaGlutJoystickDriver.h
  )

  target_compile_definitions(VistaGlutJoystickDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAGLUTJOYSTICKDRIVER_EXPORTS>
  )

  target_link_libraries(VistaGlutJoystickDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      GLUT::GLUT
  )

  install(
      TARGETS VistaGlutJoystickPlugin VistaGlutJoystickDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaGlutJoystickDriver"
  )
endif ()
