file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(09EventDemo ${ProjectSources})
target_link_libraries(09EventDemo PRIVATE Vista::Kernel)
install(TARGETS 09EventDemo)
