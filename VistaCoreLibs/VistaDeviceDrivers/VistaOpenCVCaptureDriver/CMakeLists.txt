add_library(VistaOpenCVCaptureTranscoder SHARED
    VistaOpenCVCaptureTranscoder.cpp
)

target_link_libraries(VistaOpenCVCaptureTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaOpenCVCaptureTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAOPENCVCAPTURETRANSCODER_EXPORTS>
)

install(
    TARGETS VistaOpenCVCaptureTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenCVCaptureDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(OpenCV QUIET)

if (NOT OPENCV_FOUND)
  message(STATUS "VistaOpenCVCaptureDriver requires OpenCV, which is not available - only transcoder will be built")
else ()
  add_library(VistaOpenCVCapturePlugin SHARED
      VistaOpenCVCapturePlugin.cpp
  )

  target_link_libraries(VistaOpenCVCapturePlugin
    PUBLIC
      VistaOpenCVCaptureDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaOpenCVCapturePlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENCVCAPTUREPLUGIN_EXPORTS>
  )

  add_library(VistaOpenCVCaptureDriver SHARED
      VistaOpenCVCaptureDriver.cpp
  )

  target_sources(VistaOpenCVCaptureDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaOpenCVCaptureDriver.h
  )

  target_compile_definitions(VistaOpenCVCaptureDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENCVCAPTUREDRIVER_EXPORTS>
  )

  target_link_libraries(VistaOpenCVCaptureDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      opencv_videoio
  )

  install(
      TARGETS VistaOpenCVCapturePlugin VistaOpenCVCaptureDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenCVCaptureDriver"
  )
endif ()
