add_library(VistaSDL2EventKeyboardTranscoder SHARED
    VistaSDL2EventKeyboardTranscoder.cpp
)

target_link_libraries(VistaSDL2EventKeyboardTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaSDL2EventKeyboardTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTASDL2EVENTKEYBOARDTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaSDL2EventKeyboardTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2EventKeyboardDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(SDL2 QUIET)

if (NOT SDL2_FOUND)
  message(STATUS "VistaSDL2EventKeyboardDriver requires SDL2, which is not available - only transcoder will be built")
else ()
  add_library(VistaSDL2EventKeyboardPlugin SHARED
      VistaSDL2EventKeyboardPlugin.cpp
  )

  target_link_libraries(VistaSDL2EventKeyboardPlugin
    PUBLIC
      VistaSDL2EventKeyboardDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaSDL2EventKeyboardPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2EVENTKEYBOARDPLUGIN_EXPORTS>
  )

  add_library(VistaSDL2EventKeyboardDriver SHARED
      VistaSDL2EventKeyboardDriver.cpp
  )

  target_sources(VistaSDL2EventKeyboardDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaSDL2EventKeyboardDriver.h
  )

  target_compile_definitions(VistaSDL2EventKeyboardDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASDL2EVENTKEYBOARDDRIVER_EXPORTS>
  )

  target_link_libraries(VistaSDL2EventKeyboardDriver
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
      TARGETS VistaSDL2EventKeyboardPlugin VistaSDL2EventKeyboardDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSDL2EventKeyboardDriver"
  )
endif ()
