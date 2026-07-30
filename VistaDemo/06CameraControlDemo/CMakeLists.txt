cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(06CameraControlDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

add_executable(06CameraControlDemo main.cpp)
target_link_libraries(06CameraControlDemo
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(06CameraControlDemo)
vista_install(06CameraControlDemo)


set_target_properties(06CameraControlDemo PROPERTIES FOLDER "VistaDemo")
add_dependencies(06CameraControlDemo VistaKernel)

