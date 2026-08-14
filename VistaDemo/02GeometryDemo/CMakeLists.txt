file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(02GeometryDemo ${ProjectSources})
target_link_libraries(02GeometryDemo PRIVATE Vista::Kernel)
install(TARGETS 02GeometryDemo)
