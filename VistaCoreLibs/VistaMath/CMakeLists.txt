add_library(VistaMath SHARED)
add_library(Vista::Math ALIAS VistaMath)

set_target_properties(VistaMath
  PROPERTIES
    EXPORT_NAME Math
)

file(GLOB_RECURSE ProjectSources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE ProjectHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(VistaMath
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

target_link_libraries(VistaMath
  PUBLIC
    VistaBase
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaMath PRIVATE precompiled.pch)
endif ()

target_compile_definitions(VistaMath
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAMATH_EXPORTS>
)

install(
    TARGETS VistaMath
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaMath"
)