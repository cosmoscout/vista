cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(19TimeDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

add_executable(19TimeDemo main.cpp)
target_link_libraries(19TimeDemo
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(19TimeDemo)
vista_install(19TimeDemo)

set_target_properties(19TimeDemo PROPERTIES FOLDER "VistaDemo")
add_dependencies(19TimeDemo VistaKernel)
