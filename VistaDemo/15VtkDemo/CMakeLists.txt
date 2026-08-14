find_package(VTK REQUIRED)

file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(15VtkDemo ${ProjectSources})
target_link_libraries(15VtkDemo PRIVATE Vista::Kernel ${VTK_LIBRARIES})
install(TARGETS 15VtkDemo)
