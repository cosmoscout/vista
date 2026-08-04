add_library(VistaTools SHARED)
add_library(Vista::Tools ALIAS VistaTools)

set_target_properties(VistaTools
  PROPERTIES
    EXPORT_NAME Tools
)

file(GLOB_RECURSE ProjectSources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE ProjectHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(VistaTools
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

target_link_libraries(VistaTools
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaMath
    $<$<PLATFORM_ID:Windows>:PowrProf dbghelp Psapi>
    $<$<PLATFORM_ID:Linux>:dl>
)

target_compile_definitions(VistaTools
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTATOOLS_EXPORTS>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaTools PRIVATE precompiled.pch)
endif ()

install(
    TARGETS VistaTools
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaTools"
)
