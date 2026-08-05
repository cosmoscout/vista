add_executable(04LoadDemo main.cpp)
target_link_libraries(04LoadDemo PRIVATE Vista::Kernel)
install(TARGETS 04LoadDemo)
