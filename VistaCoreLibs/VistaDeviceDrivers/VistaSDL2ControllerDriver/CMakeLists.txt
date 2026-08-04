add_library(VistaSDL2ControllerTranscoder SHARED
    VistaSDL2ControllerTranscoder.cpp
)

target_link_libraries(VistaSDL2ControllerTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaSDL2ControllerTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTASDL2CONTROLLERTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaSDL2ControllerTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2ControllerDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(SDL2 QUIET)

if (NOT SDL2_FOUND)
  message(STATUS "VistaSDL2ControllerDriver requires SDL2, which is not available - only transcoder will be built")
else ()
  add_library(VistaSDL2ControllerPlugin SHARED
      VistaSDL2ControllerPlugin.cpp
  )

  target_link_libraries(VistaSDL2ControllerPlugin
    PUBLIC
      VistaSDL2ControllerDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaSDL2ControllerPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2CONTROLLERPLUGIN_EXPORTS>
  )

  add_library(VistaSDL2ControllerDriver SHARED
      VistaSDL2ControllerDriver.cpp
  )

  target_sources(VistaSDL2ControllerDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaSDL2ControllerDriver.h
        VistaSDL2ControllerState.h
  )

  target_compile_definitions(VistaSDL2ControllerDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2CONTROLLERDRIVER_EXPORTS>
  )

  target_link_libraries(VistaSDL2ControllerDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      SDL2::SDL2
  )

  install(
      TARGETS VistaSDL2ControllerPlugin VistaSDL2ControllerDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2ControllerDriver"
  )
endif ()
