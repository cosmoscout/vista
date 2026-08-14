add_executable(03TextDemo
    main.cpp
    TextDemoAppl.cpp
    TextDemoAppl.h
)

target_link_libraries(03TextDemo PRIVATE Vista::Kernel)
install(TARGETS 03TextDemo)
