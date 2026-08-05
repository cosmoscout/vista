add_executable(05InteractionDemoSender sender.cpp)
target_link_libraries(05InteractionDemoSender PRIVATE Vista::Kernel)
install(TARGETS 05InteractionDemoSender)
