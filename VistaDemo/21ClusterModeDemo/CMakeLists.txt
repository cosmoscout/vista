cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(21ClusterModeDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

file(GLOB ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(21ClusterModeDemo ${ProjectSources})
target_link_libraries(21ClusterModeDemo
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(21ClusterModeDemo)
vista_install(21ClusterModeDemo)

if (VISTA_COMMON_BUILD)
    set_target_properties(21ClusterModeDemo PROPERTIES FOLDER "VistaDemo")
    add_dependencies(21ClusterModeDemo VistaKernel)
endif ()

