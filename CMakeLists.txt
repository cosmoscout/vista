cmake_minimum_required(VERSION 3.28)
project(Vista)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/VistaCMakeCommon")
include(VistaCMakeCommon/VistaCommon.cmake)

add_subdirectory(VistaCoreLibs)

set(VISTADEMO_ENABLED TRUE CACHE BOOL "Enable building of VistaDemo")
if (VISTADEMO_ENABLED)
    add_subdirectory(VistaDemo)
endif ()
