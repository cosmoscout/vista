add_library(VistaAspects SHARED)
add_library(Vista::Aspects ALIAS VistaAspects)

set_target_properties(VistaAspects
  PROPERTIES
    EXPORT_NAME Aspects
)

file(GLOB_RECURSE ProjectSources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE ProjectHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(VistaAspects
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

target_link_libraries(VistaAspects
  PUBLIC
    VistaBase
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaAspects PRIVATE precompiled.pch)
endif ()

target_compile_definitions(VistaAspects
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAASPECTS_EXPORTS>
)

install(
    TARGETS VistaAspects
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaAspects"
)
