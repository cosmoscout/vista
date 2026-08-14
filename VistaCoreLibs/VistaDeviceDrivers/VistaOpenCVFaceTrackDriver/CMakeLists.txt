add_library(VistaOpenCVFaceTrackTranscoder SHARED
    VistaOpenCVFaceTrackTranscoder.cpp
)

target_link_libraries(VistaOpenCVFaceTrackTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(VistaOpenCVFaceTrackTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAOPENCVFACETRACKTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaOpenCVFaceTrackTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenCVFaceTrackDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(OpenCV QUIET)

if (NOT OPENCV_FOUND)
  message(STATUS "VistaOpenCVFaceTrackDriver requires OpenCV, which is not available - only transcoder will be built")
else ()
  add_library(VistaOpenCVFaceTrackPlugin SHARED
      VistaOpenCVFaceTrackPlugin.cpp
  )

  target_link_libraries(VistaOpenCVFaceTrackPlugin
    PUBLIC
      VistaOpenCVFaceTrackDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaOpenCVFaceTrackPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENCVFACETRACKPLUGIN_EXPORTS>
  )

  add_library(VistaOpenCVFaceTrackDriver SHARED
      VistaOpenCVFaceTrackDriver.cpp
  )

  target_sources(VistaOpenCVFaceTrackDriver
    PUBLIC
      FILE_SET HEADERS
      FILES VistaOpenCVFaceTrackDriver.h
  )

  target_compile_definitions(VistaOpenCVFaceTrackDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENCVFACETRACKDRIVER_EXPORTS>
  )

  target_link_libraries(VistaOpenCVFaceTrackDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      opencv_videoio
      opencv_objdetect
      opencv_highgui
  )

  install(
      TARGETS VistaOpenCVFaceTrackPlugin VistaOpenCVFaceTrackDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenCVFaceTrackDriver"
  )
endif ()
