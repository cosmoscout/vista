cmake_minimum_required(VERSION 3.28)

if (NOT VISTADEMO_COMMON_BUILD)
    project(20PythonDFNDemo)

    list(APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}")
    include(VistaCommon)
endif ()

vista_use_package(VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES)

# get boost
#set(Boost_USE_STATIC_LIBS   ON)
#set(Boost_USE_MULTITHREADED ON)
vista_use_package(Boost COMPONENTS
        system
        thread
        python
        REQUIRED)

# get python
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${PYTHON_LIBRARIES})

file(GLOB ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(20PythonDFNDemo ${ProjectSources})
target_link_libraries(20PythonDFNDemo
        ${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
        ${PYTHON_LIBRARIES}
)

vista_configure_app(20PythonDFNDemo)
vista_install(20PythonDFNDemo)

if (VISTA_COMMON_BUILD)
    set_target_properties(20PythonDFNDemo PROPERTIES FOLDER "VistaDemo")
    add_dependencies(20PythonDFNDemo VistaKernel)
endif ()

