cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(17MsgPortBob)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

file(GLOB ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(17MsgPortBob ${ProjectSources})
target_link_libraries(17MsgPortBob
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(17MsgPortBob)
vista_install(17MsgPortBob)

set_target_properties(17MsgPortBob PROPERTIES FOLDER "VistaDemo")
add_dependencies(17MsgPortBob VistaKernel)

