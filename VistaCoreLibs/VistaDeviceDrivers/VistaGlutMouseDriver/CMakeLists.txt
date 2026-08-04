add_library(VistaGlutMouseTranscoder SHARED
    VistaGlutMouseTranscoder.cpp
)

target_link_libraries(VistaGlutMouseTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaGlutMouseTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAGLUTMOUSETRANSCODER_EXPORTS>
)

install(
    TARGETS VistaGlutMouseTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaGlutMouseDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(GLUT QUIET)

if (NOT GLUT_FOUND)
  message(STATUS "VistaGlutMouseDriver requires GLUT, which is not available - only transcoder will be built")
else ()
  add_library(VistaGlutMousePlugin SHARED
      VistaGlutMousePlugin.cpp
  )

  target_link_libraries(VistaGlutMousePlugin
    PUBLIC
      VistaGlutMouseDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaGlutMousePlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAGLUTMOUSEPLUGIN_EXPORTS>
  )

  add_library(VistaGlutMouseDriver SHARED
      VistaGlutMouseDriver.cpp
  )

  target_sources(VistaGlutMouseDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaGlutMouseDriver.h
  )

  target_compile_definitions(VistaGlutMouseDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAGLUTMOUSEDRIVER_EXPORTS>
  )

  target_link_libraries(VistaGlutMouseDriver
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
      TARGETS VistaGlutMousePlugin VistaGlutMouseDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaGlutMouseDriver"
  )
endif ()
