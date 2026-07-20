cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(15VtkDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)
vista_use_package(VTK REQUIRED FIND_DEPENDENCIES)

file(GLOB ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(15VtkDemo ${ProjectSources})
target_link_libraries(15VtkDemo
        ${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_app(15VtkDemo)
vista_install(15VtkDemo)

if (VISTA_COMMON_BUILD)
    set_target_properties(15VtkDemo PROPERTIES FOLDER "VistaDemo")
    add_dependencies(15VtkDemo VistaKernel)
endif ()

