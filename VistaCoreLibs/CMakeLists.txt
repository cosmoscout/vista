include_directories(${CMAKE_CURRENT_SOURCE_DIR})

option(VISTA_ENABLE_WARNINGS "Enable/Disable warnings" ON)
if (VISTA_ENABLE_WARNINGS)
  add_compile_options(
    $<$<CXX_COMPILER_ID:MSVC>:-W4>
    $<$<CXX_COMPILER_ID:MSVC>:-wd4244>
    $<$<CXX_COMPILER_ID:MSVC>:-wd4100>
    $<$<CXX_COMPILER_ID:MSVC>:-wd4512>
    $<$<CXX_COMPILER_ID:MSVC>:-wd4245>
    $<$<CXX_COMPILER_ID:MSVC>:-wd4389>

    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-reorder>
  )
endif ()

# This should be disabled for now. Ideally we replace ViSTA atomics in the future with std.
option(VISTA_SUPPORT_NATIVE_64BIT_ATOMICS "" OFF)
if (NOT VISTA_SUPPORT_NATIVE_64BIT_ATOMICS)
  add_compile_definitions(VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE)
endif ()

option(VISTA_USE_PRECOMPILED_HEADERS "Turn on generation of precompiled headers" OFF)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 20)

add_compile_options(
  $<$<CXX_COMPILER_ID:MSVC>:-bigobj>
)

add_compile_definitions(
  NOMINMAX
)

include(GNUInstallDirs)

add_subdirectory(VistaBase)
add_subdirectory(VistaAspects)
add_subdirectory(VistaMath)
add_subdirectory(VistaInterProcComm)
add_subdirectory(VistaTools)
add_subdirectory(VistaDeviceDriversBase)
add_subdirectory(VistaDataFlowNet)

option(VISTA_BUILD_KERNEL "Build the Vista Kernel module" ON)
if (VISTA_BUILD_KERNEL)
  add_subdirectory(VistaKernel)
  add_subdirectory(VistaKernelOpenSGExt)
  add_subdirectory(VistaOGLExt)
endif ()

add_subdirectory(VistaDeviceDrivers)

option(VISTA_TESTS "Build tests" OFF)
if (VISTA_TESTS)
  enable_testing()
  add_subdirectory(Tests)
endif ()

include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/cmake/VistaConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/VistaConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Vista
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/VistaConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMinorVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/VistaConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/VistaConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/Vista"
)

add_library(Vista INTERFACE)
target_link_libraries(Vista
  INTERFACE
    VistaBase
    VistaAspects
    VistaMath
    VistaInterProcComm
    VistaTools
    VistaDeviceDriversBase
    VistaDataFlowNet
)
if (VISTA_BUILD_KERNEL)
  target_link_libraries(Vista
    INTERFACE
      VistaKernel
      VistaKernelOpenSGExt
      VistaOGLExt
  )
endif()

install(EXPORT VistaTargets
  FILE VistaTargets.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/Vista
  NAMESPACE Vista::
)

# set(VISTACORELIBS_LIBRARY_OUTDIR "${CMAKE_BINARY_DIR}/lib" "${CMAKE_BINARY_DIR}/lib/DriverPlugins")
# set(VISTACORELIBS_LIBRARY_INSTALLDIR "${CMAKE_INSTALL_PREFIX}/lib" "${CMAKE_INSTALL_PREFIX}/lib/DriverPlugins")
# set(VISTACORELIBS_CONFIG_DEFINITIONS "${VISTA_PLATFORM_DEFINE}")
# 
# vista_create_cmake_configs(VistaCoreLibs
#         "${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/VistaCoreLibsConfig-build.cmake_proto"
#         "${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/VistaCoreLibsConfig-install.cmake_proto")
# 
# # install license files
# install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.LESSER" "README" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/VistaCoreLibs")
