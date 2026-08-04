option(VISTACORELIBS_IPC_USE_EXCEPTIONS "Specifies whether or not the IPC throws exceptions when errors occur" ON)

add_library(VistaInterProcComm SHARED)
add_library(Vista::InterProcComm ALIAS VistaInterProcComm)

set_target_properties(VistaInterProcComm
  PROPERTIES
    EXPORT_NAME InterProcComm
)

file(GLOB_RECURSE ProjectSources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE ProjectHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(VistaInterProcComm
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

target_link_libraries(VistaInterProcComm
  PUBLIC
    VistaBase
    VistaAspects
    $<$<PLATFORM_ID:Windows>:winmm ws2_32>
    $<$<PLATFORM_ID:Linux>:pthread>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaInterProcComm PRIVATE precompiled.pch)
endif ()

target_compile_definitions(VistaInterProcComm
  PRIVATE
    $<$<PLATFORM_ID:Windows>:_USE_WIN32_MUTEX>
    $<$<PLATFORM_ID:Windows>:VISTAINTERPROCCOMM_EXPORTS>
    $<$<BOOL:${VISTACORELIBS_IPC_USE_EXCEPTIONS}>:VISTA_IPC_USE_EXCEPTIONS>
)

install(
    TARGETS VistaInterProcComm
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaInterProcComm"
)
