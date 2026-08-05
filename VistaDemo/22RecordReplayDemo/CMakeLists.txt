add_executable(22RecordReplayDemo main.cpp)
target_link_libraries(22RecordReplayDemo PRIVATE Vista::Kernel)
install(TARGETS 22RecordReplayDemo)
