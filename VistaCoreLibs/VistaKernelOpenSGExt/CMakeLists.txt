vista_use_package(GLEW REQUIRED)
vista_use_package(OpenGL REQUIRED)
vista_use_package(OpenSG 1.8 REQUIRED SYSTEM_HEADERS)

file(GLOB_RECURSE ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(VistaKernelOpenSGExt ${ProjectSources})
target_link_libraries(VistaKernelOpenSGExt
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaMath
        VistaTools
        VistaDeviceDriversBase
        VistaDataFlowNet
        VistaKernel
        ${VISTA_USE_PACKAGE_LIBRARIES}
)

if (VISTA_USE_PRECOMPILED_HEADERS)
    target_precompile_headers(VistaKernelOpenSGExt PRIVATE precompiled.pch)
endif ()

vista_configure_lib(VistaKernelOpenSGExt)
vista_install(VistaKernelOpenSGExt VistaKernelOpenSGExt)

