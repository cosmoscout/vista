add_executable(06CameraControlDemo main.cpp)
target_link_libraries(06CameraControlDemo PRIVATE Vista::Kernel)
install(TARGETS 06CameraControlDemo)
