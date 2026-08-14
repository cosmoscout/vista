file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(13KernelOpenSGExtShadowDemo ${ProjectSources})
target_link_libraries(13KernelOpenSGExtShadowDemo PRIVATE Vista::Kernel Vista::KernelOpenSGExt)
install(TARGETS 13KernelOpenSGExtShadowDemo)
