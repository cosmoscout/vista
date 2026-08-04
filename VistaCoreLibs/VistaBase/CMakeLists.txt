add_library(VistaBase SHARED)
add_library(Vista::Base ALIAS VistaBase)

set_target_properties(VistaBase
  PROPERTIES
    EXPORT_NAME Base
)

file(GLOB_RECURSE ProjectSources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE ProjectHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(VistaBase
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaBase PRIVATE precompiled.pch)
endif ()

target_compile_definitions(VistaBase
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTABASE_EXPORTS>
)

install(
    TARGETS VistaBase
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaBase"
)