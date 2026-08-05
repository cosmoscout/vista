file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(17MsgPortAlice ${ProjectSources})
target_link_libraries(17MsgPortAlice PRIVATE Vista::Kernel)
install(TARGETS 17MsgPortAlice)
