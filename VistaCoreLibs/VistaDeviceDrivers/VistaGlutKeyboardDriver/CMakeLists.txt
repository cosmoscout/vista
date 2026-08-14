add_library(VistaGlutKeyboardTranscoder SHARED
    VistaGlutKeyboardTranscoder.cpp
)

target_link_libraries(VistaGlutKeyboardTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaGlutKeyboardTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAGLUTKEYBOARDTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaGlutKeyboardTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaGlutKeyboardDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(GLUT QUIET)

if (NOT GLUT_FOUND)
  message(STATUS "VistaGlutKeyboardDriver requires GLUT, which is not available - only transcoder will be built")
else ()
  add_library(VistaGlutKeyboardPlugin SHARED
      VistaGlutKeyboardPlugin.cpp
  )

  target_link_libraries(VistaGlutKeyboardPlugin
    PUBLIC
      VistaGlutKeyboardDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaGlutKeyboardPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAGLUTKEYBOARDPLUGIN_EXPORTS>
  )

  add_library(VistaGlutKeyboardDriver SHARED
      VistaGlutKeyboardDriver.cpp
  )

  target_sources(VistaGlutKeyboardDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaGlutKeyboardDriver.h
  )

  target_compile_definitions(VistaGlutKeyboardDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAGLUTKEYBOARDDRIVER_EXPORTS>
  )

  target_link_libraries(VistaGlutKeyboardDriver
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
      TARGETS VistaGlutKeyboardPlugin VistaGlutKeyboardDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaGlutKeyboardDriver"
  )
endif ()
