file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(21ClusterModeDemo ${ProjectSources})
target_link_libraries(21ClusterModeDemo PRIVATE Vista::Kernel)
install(TARGETS 21ClusterModeDemo)
