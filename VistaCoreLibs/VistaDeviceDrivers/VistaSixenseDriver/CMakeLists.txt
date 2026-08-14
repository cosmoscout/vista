add_library(VistaSixenseTranscoder SHARED
    VistaSixenseTranscoder.cpp
    VistaSixenseCommonShare.h
)

target_link_libraries(VistaSixenseTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaSixenseTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTASIXENSETRANSCODER_EXPORTS>
)

install(
    TARGETS VistaSixenseTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSixenseDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(SixenseSDK QUIET)

if (NOT SIXENSESDK_FOUND)
  message(STATUS "VistaSixenseDriver requires SixenseSDK, which is not available - only transcoder will be built")
else ()
  add_library(VistaSixensePlugin SHARED
      VistaSixensePlugin.cpp
  )

  target_link_libraries(VistaSixensePlugin
    PUBLIC
      VistaSixenseDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaSixensePlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASIXENSEPLUGIN_EXPORTS>
  )

  add_library(VistaSixenseDriver SHARED
      VistaSixenseDriver.cpp
  )

  target_sources(VistaSixenseDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaSixenseDriver.h
        VistaSixenseCommonShare.h
  )

  target_compile_definitions(VistaSixenseDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTASIXENSEDRIVER_EXPORTS>
  )

  target_link_libraries(VistaSixenseDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      Sixense::SDK
      Sixense::Utils
  )

  install(
      TARGETS VistaSixensePlugin VistaSixenseDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaSixenseDriver"
  )
endif ()
