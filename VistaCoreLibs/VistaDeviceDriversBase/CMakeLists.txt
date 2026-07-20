option(VISTADEVICEDRIVERS_USE_ATOMICS "Specifies if the Device Driver History uses atomics" ON)

file(GLOB_RECURSE ProjectSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(VistaDeviceDriversBase ${ProjectSources})
target_link_libraries(VistaDeviceDriversBase
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaTools
        VistaMath
        $<$<PLATFORM_ID:Windows>:setupapi>
)

target_compile_definitions(VistaDeviceDriversBase PRIVATE
        $<$<BOOL:${VISTADEVICEDRIVERS_USE_ATOMICS}>:VISTA_USE_ATOMICS>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
    target_precompile_headers(VistaDeviceDriversBase PRIVATE precompiled.pch)
endif ()

vista_configure_lib(VistaDeviceDriversBase)
vista_install(VistaDeviceDriversBase VistaDeviceDriversBase)
