file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(VRMLVisualizer ${ProjectSources})
target_link_libraries(VRMLVisualizer PRIVATE Vista::Kernel)
install(TARGETS VRMLVisualizer)
