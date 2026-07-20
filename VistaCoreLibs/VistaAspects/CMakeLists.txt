
file(GLOB_RECURSE ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(VistaAspects ${ProjectSources})
target_link_libraries(VistaAspects
        VistaBase
)

if (VISTA_USE_PRECOMPILED_HEADERS)
    target_precompile_headers(VistaAspects PRIVATE precompiled.pch)
endif ()

vista_configure_lib(VistaAspects)
vista_install(VistaAspects VistaAspects)
