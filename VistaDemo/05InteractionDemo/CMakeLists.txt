file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(05InteractionDemo ${ProjectSources})
target_link_libraries(05InteractionDemo PRIVATE Vista::Kernel)
install(TARGETS 05InteractionDemo)

add_subdirectory(Sender)
