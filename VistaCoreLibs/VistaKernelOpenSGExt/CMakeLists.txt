find_package(OpenGL REQUIRED)
find_package(GLEW   REQUIRED)
find_package(OpenSG REQUIRED)

add_library(VistaKernelOpenSGExt SHARED)
add_library(Vista::KernelOpenSGExt ALIAS VistaKernelOpenSGExt)

set_target_properties(VistaKernelOpenSGExt
  PROPERTIES
    EXPORT_NAME KernelOpenSGExt
)

file(GLOB_RECURSE ProjectSources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE ProjectHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(VistaKernelOpenSGExt
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

target_link_libraries(VistaKernelOpenSGExt
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaMath
    VistaTools
    VistaDeviceDriversBase
    VistaDataFlowNet
    VistaKernel
    GLEW::GLEW
    OpenSG::Base
    OpenSG::System
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaKernelOpenSGExt PRIVATE precompiled.pch)
endif ()

target_compile_definitions(VistaKernelOpenSGExt
  PRIVATE
    $<$<PLATFORM_ID:Windows>:VISTAKERNELOPENSGEXT_EXPORTS>
)

install(
    TARGETS VistaKernelOpenSGExt
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaKernelOpenSGExt"
)
