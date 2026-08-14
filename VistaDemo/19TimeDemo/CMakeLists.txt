add_executable(19TimeDemo main.cpp)
target_link_libraries(19TimeDemo PRIVATE Vista::Kernel)
install(TARGETS 19TimeDemo)
