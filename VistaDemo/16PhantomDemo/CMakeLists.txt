find_package(OpenHaptics REQUIRED)

file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(16PhantomDemo ${ProjectSources})
target_link_libraries(16PhantomDemo
  PRIVATE
    Vista::Kernel
    OpenHaptics::HD
    OpenHaptics::HDU
)
install(TARGETS 16PhantomDemo)
