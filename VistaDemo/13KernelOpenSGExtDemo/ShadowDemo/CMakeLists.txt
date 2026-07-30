cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(13KernelOpenSGExtShadowDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel VistaKernelOpenSGExt FIND_DEPENDENCIES)

file(GLOB ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(13KernelOpenSGExtShadowDemo ${ProjectSources})
target_link_libraries(13KernelOpenSGExtShadowDemo
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(13KernelOpenSGExtShadowDemo)
vista_install(13KernelOpenSGExtShadowDemo)

set_target_properties(13KernelOpenSGExtShadowDemo PROPERTIES FOLDER "VistaDemo")
add_dependencies(13KernelOpenSGExtShadowDemo VistaKernelOpenSGExt)

