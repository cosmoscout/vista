file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(18DebuggingToolsDemo ${ProjectSources})
target_link_libraries(18DebuggingToolsDemo PRIVATE Vista::Kernel)
install(TARGETS 18DebuggingToolsDemo)
