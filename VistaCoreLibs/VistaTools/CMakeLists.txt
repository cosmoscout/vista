
file(GLOB_RECURSE ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(VistaTools ${ProjectSources})
target_link_libraries(VistaTools
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaMath
        $<$<PLATFORM_ID:Windows>:PowrProf dbghelp Psapi>
        $<$<PLATFORM_ID:Linux>:dl>
)

target_compile_definitions(VistaTools PRIVATE
        $<$<PLATFORM_ID:Windows>:$<$<BOOL:${BUILD_SHARED_LIBS}>:VISTATOOLS_EXPORTS>>
        $<$<PLATFORM_ID:Windows>:$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:VISTATOOLS_STATIC>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
    target_precompile_headers(VistaTools PRIVATE precompiled.pch)
endif ()

vista_configure_lib(VistaTools)
vista_install(VistaTools VistaTools)
