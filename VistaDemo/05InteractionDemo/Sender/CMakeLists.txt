cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(05InteractionDemoSender)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

add_executable(05InteractionDemoSender sender.cpp)
target_link_libraries(05InteractionDemoSender
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(05InteractionDemoSender)

set_target_properties(05InteractionDemoSender PROPERTIES FOLDER "VistaDemo")
add_dependencies(05InteractionDemoSender VistaKernel)


