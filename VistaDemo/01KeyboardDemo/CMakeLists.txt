add_executable(01KeyboardDemo main.cpp)
target_link_libraries(01KeyboardDemo PRIVATE Vista::Kernel)
install(TARGETS 01KeyboardDemo)
