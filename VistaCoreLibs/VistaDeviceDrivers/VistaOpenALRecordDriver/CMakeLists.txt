# the transcoder curently requires OpenAL, too - we might change this
find_package(OpenAL REQUIRED)

add_library(VistaOpenALRecordTranscoder SHARED
    VistaOpenALRecordTranscoder.cpp
)

target_link_libraries(VistaOpenALRecordTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    OpenAL::OpenAL
)

target_compile_definitions(VistaOpenALRecordTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAOPENALRECORDTRANSCODER_EXPORTS>
)

install(
    TARGETS VistaOpenALRecordTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenALRecordDriver"
)

# since it has third-party dependencies, we only build the driver and plugin if the package is available
find_package(FreeALUT QUIET)

if (NOT OPENAL_FOUND OR NOT FREEALUT_FOUND)
  message(STATUS "VistaOpenALRecordDriver requires OpenAL and ALUT, which is not available - only transcoder will be built")
else ()
  add_library(VistaOpenALRecordPlugin SHARED
      VistaOpenALRecordPlugin.cpp
  )

  target_link_libraries(VistaOpenALRecordPlugin
    PUBLIC
      VistaOpenALRecordDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaOpenALRecordPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENALRECORDPLUGIN_EXPORTS>
  )

  add_library(VistaOpenALRecordDriver SHARED
      VistaOpenALRecordDriver.cpp
      OpenALSoundContext.cpp
  )

  target_sources(VistaOpenALRecordDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaOpenALRecordDriver.h
        OpenALSoundContext.h
  )

  target_compile_definitions(VistaOpenALRecordDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAOPENALRECORDDRIVER_EXPORTS>
  )

  target_link_libraries(VistaOpenALRecordDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      OpenAL::OpenAL
      FreeALUT::alut
  )

  install(
      TARGETS VistaOpenALRecordPlugin VistaOpenALRecordDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaOpenALRecordDriver"
  )
endif ()
