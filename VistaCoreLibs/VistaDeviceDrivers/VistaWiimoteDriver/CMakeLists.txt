find_package(wiiuse REQUIRED)

add_library(VistaWiimoteTranscoder SHARED
    VistaWiimoteTranscoder.cpp
    VistaWiimoteCommonShare.h
)

target_link_libraries(VistaWiimoteTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
    wiiuse::wiiuse
)

target_compile_definitions(VistaWiimoteTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAWIIMOTETRANSCODER_EXPORTS>
)

install(
    TARGETS VistaWiimoteTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaWiimoteDriver"
)

if (NOT WIIUSE_FOUND)
  message(STATUS "VistaWiimoteDriver requires wiiuse, which is not available - only transcoder will be built")
else ()
  add_library(VistaWiimotePlugin SHARED
      VistaWiimotePlugin.cpp
  )

  target_link_libraries(VistaWiimotePlugin
    PUBLIC
      VistaWiimoteDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(VistaWiimotePlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAWIIMOTEPLUGIN_EXPORTS>
  )

  add_library(VistaWiimoteDriver SHARED
      VistaWiimoteDriver.cpp
  )

  target_sources(VistaWiimoteDriver
    PUBLIC
      FILE_SET HEADERS
      FILES
        VistaWiimoteDriver.h
        VistaWiimoteCommonShare.h
  )

  target_compile_definitions(VistaWiimoteDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTAWIIMOTEDRIVER_EXPORTS>
  )

  target_link_libraries(VistaWiimoteDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      VistaKernel
      wiiuse::wiiuse
  )

  install(
      TARGETS VistaWiimotePlugin VistaWiimoteDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/VistaWiimoteDriver"
  )
endif ()
