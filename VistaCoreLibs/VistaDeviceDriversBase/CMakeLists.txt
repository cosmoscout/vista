option(VISTADEVICEDRIVERS_USE_ATOMICS "Specifies if the Device Driver History uses atomics" ON)

add_library(VistaDeviceDriversBase SHARED)
add_library(Vista::DeviceDriversBase ALIAS VistaDeviceDriversBase)

set_target_properties(VistaDeviceDriversBase
  PROPERTIES
    EXPORT_NAME DeviceDriversBase
)

file(GLOB_RECURSE ProjectSources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE ProjectHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(VistaDeviceDriversBase
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

target_link_libraries(VistaDeviceDriversBase
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaTools
    VistaMath
    $<$<PLATFORM_ID:Windows>:setupapi>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaDeviceDriversBase PRIVATE precompiled.pch)
endif ()

target_compile_definitions(VistaDeviceDriversBase
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTADEVICEDRIVERSBASE_EXPORTS>
    $<$<BOOL:${VISTADEVICEDRIVERS_USE_ATOMICS}>:VISTA_USE_ATOMICS>
)

install(
    TARGETS VistaDeviceDriversBase
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaDeviceDriversBase"
)
