cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(12IntentionSelectDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

add_executable(12IntentionSelectDemo main.cpp)
target_link_libraries(12IntentionSelectDemo
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app(12IntentionSelectDemo)
vista_install(12IntentionSelectDemo)

set_target_properties(12IntentionSelectDemo PROPERTIES FOLDER "VistaDemo")
add_dependencies(12IntentionSelectDemo VistaKernel)

