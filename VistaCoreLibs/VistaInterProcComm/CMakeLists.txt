option(VISTACORELIBS_IPC_USE_EXCEPTIONS "Specifies wether or nor the IPC throws exceptions when errors occur" ON)

file(GLOB_RECURSE ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(VistaInterProcComm ${ProjectSources})
target_link_libraries(VistaInterProcComm
        VistaBase
        VistaAspects
        $<$<PLATFORM_ID:Windows>:winmm ws2_32>
        $<$<PLATFORM_ID:Linux>:pthread>
)

target_compile_definitions(VistaInterProcComm PRIVATE
        $<$<BOOL:${VISTACORELIBS_IPC_USE_EXCEPTIONS}>:VISTA_IPC_USE_EXCEPTIONS>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
    target_precompile_headers(VistaInterProcComm PRIVATE precompiled.pch)
endif ()

vista_configure_lib(VistaInterProcComm)
vista_install(VistaInterProcComm VistaInterProcComm)
