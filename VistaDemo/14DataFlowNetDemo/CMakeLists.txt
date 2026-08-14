file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(14DataFlowNetDemo ${ProjectSources})
set_target_properties(14DataFlowNetDemo PROPERTIES UNITY_BUILD OFF)
target_link_libraries(14DataFlowNetDemo PRIVATE Vista::Kernel)
install(TARGETS 14DataFlowNetDemo)
