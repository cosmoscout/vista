vista_use_package(GLEW REQUIRED)
vista_use_package(OpenGL REQUIRED)

file(GLOB_RECURSE ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(VistaOGLExt ${ProjectSources})
target_link_libraries(VistaOGLExt
        VistaBase
        VistaAspects
        VistaTools
        VistaKernel
        ${VISTA_USE_PACKAGE_LIBRARIES}
)

target_compile_definitions(VistaOGLExt PRIVATE
        $<$<PLATFORM_ID:Windows>:$<BOOL:${BUILD_SHARED_LIBS}>:VISTAOGLEXT_EXPORTS>
        $<$<PLATFORM_ID:Windows>:$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:VISTAOGLEXT_STATIC>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
    target_precompile_headers(VistaOGLExt PRIVATE precompiled.pch)
endif ()

vista_find_shader_dirs(VistaOGLExt)

set(VISTACORELIBS_SHADER_DIRS ${VISTAOGLEXT_SHADER_DIRS} PARENT_SCOPE)

vista_configure_lib(VistaOGLExt)
vista_install(VistaOGLExt VistaOGLExt)
