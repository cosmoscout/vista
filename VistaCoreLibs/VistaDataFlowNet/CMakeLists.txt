
option(VISTADATAFLOWNET_TRANSMIT_INCREMENTAL_HISTORIES "experimental incemental history transmission" OFF)

file(GLOB_RECURSE ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(VistaDataFlowNet ${ProjectSources})
target_link_libraries(VistaDataFlowNet
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaTools
        VistaMath
        VistaDeviceDriversBase
)

target_compile_definitions(VistaDataFlowNet PRIVATE
    $<$<BOOL:${VISTADATAFLOWNET_TRANSMIT_INCREMENTAL_HISTORIES}>:VISTA_TRANSMIT_INCREMENTAL_HISTORIES>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
    target_precompile_headers(VistaDataFlowNet PRIVATE precompiled.pch)
endif ()

vista_configure_lib(VistaDataFlowNet)
vista_install(VistaDataFlowNet VistaDataFlowNet)
