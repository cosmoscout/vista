cmake_minimum_required(VERSION 3.28)
project(Vista)

include(VistaCMakeCommon/VistaCommon.cmake)

set(VISTA_COMMON_BUILD TRUE)

add_subdirectory(VistaCoreLibs)

set(VISTADEMO_ENABLED TRUE CACHE BOOL "Enable building of VistaDemo")
if (VISTADEMO_ENABLED)
    add_subdirectory(VistaDemo)
endif ()
