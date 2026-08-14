add_library(VistaOpenNITranscoder SHARED
    VistaOpenNITranscoder.cpp
)

target_link_libraries(VistaOpenNITranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaOpenNITranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAOPENNITRANSCODER_EXPORTS>
)

install(
    TARGETS VistaOpenNITranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenNIDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(OpenNI QUIET)

if (NOT OPENNI_FOUND)
  message(STATUS "VistaOpenNIDriver requires OpenNI, which is not available - only transcoder will be built")
else ()
  add_library(VistaOpenNIPlugin SHARED
      VistaOpenNIPlugin.cpp
  )

  target_link_libraries(VistaOpenNIPlugin
    PUBLIC
      VistaOpenNIDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaOpenNIPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENNIPLUGIN_EXPORTS>
  )

  add_library(VistaOpenNIDriver SHARED
      VistaOpenNIDriver.cpp
  )

  target_sources(VistaOpenNIDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaOpenNIDriver.h
  )

  target_compile_definitions(VistaOpenNIDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENNIDRIVER_EXPORTS>
  )

  target_link_libraries(VistaOpenNIDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      OpenNI::OpenNI
  )

  install(
      TARGETS VistaOpenNIPlugin VistaOpenNIDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenNIDriver"
  )
endif ()
