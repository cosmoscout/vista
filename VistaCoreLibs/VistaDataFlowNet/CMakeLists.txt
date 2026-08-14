option(VISTADATAFLOWNET_TRANSMIT_INCREMENTAL_HISTORIES "experimental incremental history transmission" OFF)

add_library(VistaDataFlowNet SHARED)
add_library(Vista::DataFlowNet ALIAS VistaDataFlowNet)

set_target_properties(VistaDataFlowNet
  PROPERTIES
    EXPORT_NAME DataFlowNet
)

file(GLOB_RECURSE ProjectSources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE ProjectHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(VistaDataFlowNet
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

target_link_libraries(VistaDataFlowNet
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaTools
    VistaMath
    VistaDeviceDriversBase
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaDataFlowNet PRIVATE precompiled.pch)
endif ()

target_compile_definitions(VistaDataFlowNet
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTADATAFLOWNET_EXPORTS>
    $<$<BOOL:${VISTADATAFLOWNET_TRANSMIT_INCREMENTAL_HISTORIES}>:VISTA_TRANSMIT_INCREMENTAL_HISTORIES>
)

install(
    TARGETS VistaDataFlowNet
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaDataFlowNet"
)
