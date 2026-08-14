find_package(OpenGL REQUIRED)
find_package(GLEW   REQUIRED)

add_library(VistaOGLExt SHARED)
add_library(Vista::OGLExt ALIAS VistaOGLExt)

set_target_properties(VistaOGLExt
  PROPERTIES
    EXPORT_NAME OGLExt
)

file(GLOB_RECURSE ProjectSources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE ProjectHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(VistaOGLExt
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

target_link_libraries(VistaOGLExt
  PUBLIC
    VistaBase
    VistaAspects
    VistaTools
    VistaKernel
    GLEW::GLEW
    OpenGL::GL
)

target_compile_definitions(VistaOGLExt
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAOGLEXT_EXPORTS>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaOGLExt PRIVATE precompiled.pch)
endif ()

install(
    TARGETS VistaOGLExt
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaOGLExt"
)

install(
  DIRECTORY
    shaders/
    Rendering/shaders/
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/shaders
)
