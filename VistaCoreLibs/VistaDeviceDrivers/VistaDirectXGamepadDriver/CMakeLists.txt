add_library(VistaDirectXGamepadTranscoder SHARED
    VistaDirectXGamepadTranscoder.cpp
    VistaDirectXGamepadSensorMeasures.h
)

target_link_libraries(VistaDirectXGamepadTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaDirectXGamepadTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTADIRECTXGAMEPADTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaDirectXGamepadTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaDirectXGamepadDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(directx-headers QUIET)

if (NOT directx-headers_FOUND)
  message(STATUS "VistaDirectXGamepadDriver requires directx-headers, which is not available - only transcoder will be built")
else ()
  add_library(VistaDirectXGamepadPlugin SHARED
      VistaDirectXGamepadPlugin.cpp
  )

  target_link_libraries(VistaDirectXGamepadPlugin
    PUBLIC
      VistaDirectXGamepadDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaDirectXGamepadPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTADIRECTXGAMEPADPLUGIN_EXPORTS>
  )

  add_library(VistaDirectXGamepadDriver SHARED
      VistaDirectXGamepadDriver.cpp
  )

  target_sources(VistaDirectXGamepadDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaDirectXGamepadDriver.h
        VistaDirectXGamepadSensorMeasures.h
  )

  target_compile_definitions(VistaDirectXGamepadDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTADIRECTXGAMEPADDRIVER_EXPORTS>
  )

  target_link_libraries(VistaDirectXGamepadDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      Microsoft::DirectX-Headers
      dinput8
      dxguid
      xinput
  )

  install(
      TARGETS VistaDirectXGamepadPlugin VistaDirectXGamepadDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaDirectXGamepadDriver"
  )
endif ()
