add_library(Vista3DCSpaceNavigatorTranscoder SHARED
    Vista3DCSpaceNavigatorTranscoder.cpp
)
target_link_libraries(Vista3DCSpaceNavigatorTranscoder
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

target_compile_definitions(Vista3DCSpaceNavigatorTranscoder
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTA3DCSPACENAVIGATORTRANSCODER_EXPORTS>
)

install(
    TARGETS Vista3DCSpaceNavigatorTranscoder
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/Vista3DCSpaceNavigatorDriver"
)

if (WIN32)
  find_package(3DCSDK QUIET)

  if (NOT 3DCSDK_FOUND)
    message(STATUS "3DC SDK not found (ROOT dir set?), going for COM based 3DC driver")
    try_compile(_CAN_BUILD_COMBASED_DRIVER "${CMAKE_CURRENT_BINARY_DIR}/CMakeTest" "${CMAKE_CURRENT_SOURCE_DIR}/Vista3DCSpaceNavigatorCMakeTest.cpp")
    if (NOT _CAN_BUILD_COMBASED_DRIVER)
      message(STATUS "COM-based Vista3DCSpaceNavigatorDriver requires running access to installed driver and ATL, either of them is not available.")
    else ()
      set(_CAN_BUILD_DRIVER 1)
    endif ()
  else ()
    message(STATUS "3dcsdk installed. Building driver based on 3DC-SDK.")
    set(_CAN_BUILD_DRIVER 1)
    set(_CAN_BUILD_3DXSDK_DRIVER 1)
    include_directories(${3DXSDK_INCLUDE_DIRS})
  endif (NOT 3DCSDK_FOUND)
else (WIN32)
  set(_CAN_BUILD_DRIVER 1)
endif (WIN32)

if (_CAN_BUILD_DRIVER)
  add_library(Vista3DCSpaceNavigatorPlugin SHARED
      Vista3DCSpaceNavigatorPlugin.cpp
  )

  target_link_libraries(Vista3DCSpaceNavigatorPlugin
    PUBLIC
      Vista3DCSpaceNavigatorDriver
      VistaDeviceDriversBase
  )

  target_compile_definitions(Vista3DCSpaceNavigatorPlugin
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTA3DCSPACENAVIGATORPLUGIN_EXPORTS>
  )

  add_library(Vista3DCSpaceNavigatorDriver SHARED
      $<$<BOOL:${_CAN_BUILD_3DXSDK_DRIVER}>:Vista3DCSpaceNavigatorDriver_sdk.cpp>
      $<$<NOT:$<BOOL:${_CAN_BUILD_3DXSDK_DRIVER}>>:Vista3DCSpaceNavigatorDriver.cpp>
  )

  target_sources(Vista3DCSpaceNavigatorDriver
      PUBLIC
      FILE_SET HEADERS
      FILES Vista3DCSpaceNavigatorDriver.h
  )

  target_compile_definitions(Vista3DCSpaceNavigatorDriver
    PRIVATE
      $<$<PLATFORM_ID:Windows>:VISTA3DCSPACENAVIGATORDRIVER_EXPORTS>
  )

  target_link_libraries(Vista3DCSpaceNavigatorDriver
    PUBLIC
      VistaBase
      VistaAspects
      VistaInterProcComm
      VistaDeviceDriversBase
      VistaMath
      3DCSDK::Base
  )

  install(
      TARGETS Vista3DCSpaceNavigatorPlugin Vista3DCSpaceNavigatorDriver
      EXPORT  VistaTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/DriverPlugins
      FILE_SET HEADERS DESTINATION "include/VistaDeviceDrivers/Vista3DCSpaceNavigatorDriver"
  )
endif ()