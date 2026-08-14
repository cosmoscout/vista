file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(11OGLDemo ${ProjectSources})
target_link_libraries(11OGLDemo PRIVATE Vista::Kernel)
install(TARGETS 11OGLDemo)
