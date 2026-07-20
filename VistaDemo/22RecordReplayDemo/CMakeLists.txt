cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(22RecordReplayDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

file(GLOB ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(22RecordReplayDemo ${ProjectSources})
target_link_libraries(22RecordReplayDemo
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(22RecordReplayDemo)
vista_install(22RecordReplayDemo)

if (VISTA_COMMON_BUILD)
    set_target_properties(22RecordReplayDemo PROPERTIES FOLDER "VistaDemo")
    add_dependencies(22RecordReplayDemo VistaKernel)
endif ()
