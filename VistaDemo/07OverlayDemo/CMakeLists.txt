add_executable(07OverlayDemo main.cpp)
target_link_libraries(07OverlayDemo PRIVATE Vista::Kernel)
install(TARGETS 07OverlayDemo)
