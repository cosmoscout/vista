add_library(VistaSDL2MouseTranscoder SHARED
    VistaSDL2MouseTranscoder.cpp
)

target_link_libraries(VistaSDL2MouseTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaSDL2MouseTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTASDL2MOUSETRANSCODER_EXPORTS>
)

install(
    TARGETS VistaSDL2MouseTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2MouseDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(SDL2 QUIET)

if (NOT SDL2_FOUND)
  message(STATUS "VistaSDL2MouseDriver requires SDL2, which is not available - only transcoder will be built")
else ()
  add_library(VistaSDL2MousePlugin SHARED
      VistaSDL2MousePlugin.cpp
  )

  target_link_libraries(VistaSDL2MousePlugin
    PUBLIC
      VistaSDL2MouseDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaSDL2MousePlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2MOUSEPLUGIN_EXPORTS>
  )

  add_library(VistaSDL2MouseDriver SHARED
      VistaSDL2MouseDriver.cpp
  )

  target_sources(VistaSDL2MouseDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaSDL2MouseDriver.h
  )

  target_compile_definitions(VistaSDL2MouseDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2MOUSEDRIVER_EXPORTS>
  )

  target_link_libraries(VistaSDL2MouseDriver
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
      TARGETS VistaSDL2MousePlugin VistaSDL2MouseDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2MouseDriver"
  )
endif ()
