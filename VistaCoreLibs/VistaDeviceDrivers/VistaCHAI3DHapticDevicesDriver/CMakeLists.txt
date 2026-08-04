add_library(VistaCHAI3DHapticDevicesTranscoder SHARED
    VistaCHAI3DHapticDevicesTranscoder.cpp
    VistaCHAI3DHapticDevicesCommonShare.h
)

target_link_libraries(VistaCHAI3DHapticDevicesTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaCHAI3DHapticDevicesTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTACHAI3DHAPTICDEVICESTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaCHAI3DHapticDevicesTranscoder
    EXPORT VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaCHAI3DHapticDevicesDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(Chai3D QUIET)
find_package(OpenGL QUIET)

if (NOT SDL2_FOUND AND NOT OPENGL_FOUND)
  message(STATUS "VistaCHAI3DHapticDevicesDriver requires Chai3D and OpenGL, which are not available - only transcoder will be built")
else ()
  add_library(VistaCHAI3DHapticDevicesPlugin SHARED
      VistaCHAI3DHapticDevicesPlugin.cpp
  )

  target_link_libraries(VistaCHAI3DHapticDevicesPlugin
    PUBLIC
      VistaCHAI3DHapticDevicesDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaCHAI3DHapticDevicesPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTACHAI3DHAPTICDEVICESPLUGIN_EXPORTS>
  )

  add_library(VistaCHAI3DHapticDevicesDriver SHARED
      VistaCHAI3DHapticDevicesDriver.cpp
  )

  target_sources(VistaCHAI3DHapticDevicesDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaCHAI3DHapticDevicesDriver.h
        VistaCHAI3DHapticDevicesCommonShare.h
  )

  target_compile_definitions(VistaCHAI3DHapticDevicesDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTACHAI3DHAPTICDEVICESDRIVER_EXPORTS>
  )

  target_link_libraries(VistaCHAI3DHapticDevicesDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      OpenGL::GL
      chai3d
  )

  install(
      TARGETS VistaCHAI3DHapticDevicesPlugin VistaCHAI3DHapticDevicesDriver
      EXPORT VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaCHAI3DHapticDevicesDriver"
  )
endif ()
