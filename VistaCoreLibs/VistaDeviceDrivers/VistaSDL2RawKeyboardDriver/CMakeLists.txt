add_library(VistaSDL2RawKeyboardTranscoder SHARED
    VistaSDL2RawKeyboardTranscoder.cpp
)

target_link_libraries(VistaSDL2RawKeyboardTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaSDL2RawKeyboardTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTASDL2RAWKEYBOARDTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaSDL2RawKeyboardTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2RawKeyboardDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(SDL2 QUIET)

if (NOT SDL2_FOUND)
  message(STATUS "VistaSDL2RawKeyboardDriver requires SDL2, which is not available - only transcoder will be built")
else ()
  add_library(VistaSDL2RawKeyboardPlugin SHARED
      VistaSDL2RawKeyboardPlugin.cpp
  )

  target_link_libraries(VistaSDL2RawKeyboardPlugin
    PUBLIC
      VistaSDL2RawKeyboardDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaSDL2RawKeyboardPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2RAWKEYBOARDPLUGIN_EXPORTS>
  )

  add_library(VistaSDL2RawKeyboardDriver SHARED
      VistaSDL2RawKeyboardDriver.cpp
  )

  target_sources(VistaSDL2RawKeyboardDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaSDL2RawKeyboardDriver.h
  )

  target_compile_definitions(VistaSDL2RawKeyboardDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2RAWKEYBOARDDRIVER_EXPORTS>
  )

  target_link_libraries(VistaSDL2RawKeyboardDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      SDL2::SDL2
  )

  install(
      TARGETS VistaSDL2RawKeyboardPlugin VistaSDL2RawKeyboardDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2RawKeyboardDriver"
  )
endif ()
