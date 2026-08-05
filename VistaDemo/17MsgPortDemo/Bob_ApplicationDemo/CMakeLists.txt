file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(17MsgPortBob ${ProjectSources})
target_link_libraries(17MsgPortBob PRIVATE Vista::Kernel)
install(TARGETS 17MsgPortBob)
