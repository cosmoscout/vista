add_library(VistaSDL2TextInputTranscoder SHARED
    VistaSDL2TextInputTranscoder.cpp
)

target_link_libraries(VistaSDL2TextInputTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaSDL2TextInputTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTASDL2TEXTINPUTTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaSDL2TextInputTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2TextInputDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(SDL2 QUIET)

if (NOT SDL2_FOUND)
  message(STATUS "VistaSDL2TextInputDriver requires SDL2, which is not available - only transcoder will be built")
else ()
  add_library(VistaSDL2TextInputPlugin SHARED
      VistaSDL2TextInputPlugin.cpp
  )

  target_link_libraries(VistaSDL2TextInputPlugin
    PUBLIC
      VistaSDL2TextInputDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaSDL2TextInputPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2TEXTINPUTPLUGIN_EXPORTS>
  )

  add_library(VistaSDL2TextInputDriver SHARED
      VistaSDL2TextInputDriver.cpp
  )

  target_sources(VistaSDL2TextInputDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaSDL2TextInputDriver.h
  )

  target_compile_definitions(VistaSDL2TextInputDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2TEXTINPUTDRIVER_EXPORTS>
  )

  target_link_libraries(VistaSDL2TextInputDriver
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
      TARGETS VistaSDL2TextInputPlugin VistaSDL2TextInputDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2TextInputDriver"
  )
endif ()
