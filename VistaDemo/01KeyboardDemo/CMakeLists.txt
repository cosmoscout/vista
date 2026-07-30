cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(01KeyboardDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

add_executable(01KeyboardDemo main.cpp)
target_link_libraries(01KeyboardDemo
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(01KeyboardDemo)
vista_install(01KeyboardDemo)

set_target_properties(01KeyboardDemo PROPERTIES FOLDER "VistaDemo")
add_dependencies(01KeyboardDemo VistaKernel)

