cmake_minimum_required(VERSION 3.28)
project(Vista VERSION 1.0.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(VistaCoreLibs)

option(VISTA_DEMO "Enable building of VistaDemo" ON)
if (VISTA_DEMO)
    add_subdirectory(VistaDemo)
endif ()
