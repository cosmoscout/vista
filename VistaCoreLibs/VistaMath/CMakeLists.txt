file(GLOB_RECURSE ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(VistaMath ${ProjectSources})
target_link_libraries(VistaMath
        VistaBase
)

target_compile_definitions(VistaMath PRIVATE
        $<$<PLATFORM_ID:Windows>:$<$<BOOL:${BUILD_SHARED_LIBS}>:VISTAMATH_EXPORTS>>
        $<$<PLATFORM_ID:Windows>:$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:VISTAMATH_STATIC>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
    target_precompile_headers(VistaMath PRIVATE precompiled.pch)
endif ()

vista_configure_lib(VistaMath)
vista_install(VistaMath VistaMath)