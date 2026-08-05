file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(10DisplayDemo ${ProjectSources})
target_link_libraries(10DisplayDemo PRIVATE Vista::Kernel)
install(TARGETS 10DisplayDemo)
