cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(10DisplayDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

file(GLOB ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(10DisplayDemo ${ProjectSources})
target_link_libraries(10DisplayDemo
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(10DisplayDemo)
vista_install(10DisplayDemo)

if (VISTA_COMMON_BUILD)
    set_target_properties(10DisplayDemo PROPERTIES FOLDER "VistaDemo")
    add_dependencies(10DisplayDemo VistaKernel)
endif ()

