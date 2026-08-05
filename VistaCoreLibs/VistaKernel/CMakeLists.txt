find_package(OpenGL REQUIRED)
find_package(GLEW   REQUIRED)
find_package(OpenSG REQUIRED)

option(VISTA_CORELIBS_USE_SDL2_WINDOWIMP "Determines if the SDL2-based WindowToolkit Implementation will be prefered" On)
if (VISTA_CORELIBS_USE_SDL2_WINDOWIMP)
  find_package(SDL2     REQUIRED)
  find_package(SDL2_ttf REQUIRED)
endif ()

option(VISTA_CORELIBS_USE_GLUT_WINDOWIMP "Determines if the GLUT-based WindowToolit Implementation will be prefered" Off)
if (VISTA_CORELIBS_USE_GLUT_WINDOWIMP)
  find_package(GLUT REQUIRED)

  option(VISTA_CORELIBS_GLUT_WINDOW_NOTIFY_FRAME_EXCEPTIONS "If enabled, the whole vista frame processing will be surrounded by a try-catch to cacth any std::exception. If an exception is caught, an error is printed to the error stream, and the exception is re-thrown. This is useful for 64bit Windows version, where some kernel function calls (as the window callback used in glut) silently drop any unhandled exceptions" FALSE)
  option(VISTA_CORELIBS_GLUT_WINDOW_ABORT_ON_FRAME_EXCEPTIONS "If enabled, the whole vista frame processing will be surrounded by a try-catch to cacth any std::exception. If an exception is caught, and error is printed and VistaSytem::Quit() is called. This is useful for 64bit Windows version, where some kernel function calls (as the window callback used in glut) silently drop any unhandled exceptions" FALSE)
endif ()

option(VISTA_CORELIBS_USE_ZEROMQ "if activated, zeromq-specific stuff will be built" Off)
if (VISTA_CORELIBS_USE_ZEROMQ)
  find_package(cppzmq REQUIRED)
endif ()

option(VISTA_CORELIBS_USE_OCULUSSDK "if activated, the OculusSDK viewport will be available" Off)
if (VISTA_CORELIBS_USE_OCULUSSDK)
  find_package(OVR REQUIRED)
endif ()

option(VISTA_CORELIBS_USE_OPENVR "if activated, the OpenVR viewport will be available" Off)
if (VISTA_CORELIBS_USE_OPENVR)
  find_package(OpenVR REQUIRED)
endif ()

option(VISTA_CORELIBS_USE_INFINITE_REVERSE_PROJECTION "if activated, matrices will be modified to use infinite reverse projection" On)

if (NOT VISTA_CORELIBS_USE_SDL2_WINDOWIMP AND NOT VISTA_CORELIBS_USE_GLUT_WINDOWIMP)
  message(WARNING "No Windowing Toolkit will be built!")
endif ()

add_library(VistaKernel SHARED)
add_library(Vista::Kernel ALIAS VistaKernel)

set_target_properties(VistaKernel
  PROPERTIES
    EXPORT_NAME Kernel
)

file(GLOB ProjectSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Cluster/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Cluster/Utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/EventManager/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/GraphicsManager/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/InteractionManager/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/InteractionManager/DfnNodes/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenSG/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Stuff/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Stuff/ProximityWarning/*.cpp"
)

if(VISTA_CORELIBS_USE_ZEROMQ)
  file(GLOB _srcs "${CMAKE_CURRENT_SOURCE_DIR}/Cluster/ZeroMQExt/*.cpp")
  list(APPEND ProjectSources ${_srcs})
endif()

if(VISTA_CORELIBS_USE_GLUT_WINDOWIMP)
  file(GLOB _srcs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/GlutWindowImp/*.cpp")
  list(APPEND ProjectSources ${_srcs})

  if(VISTA_CORELIBS_USE_OCULUSSDK)
    file(GLOB _srcs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/OculusGlutWindowImp/*.cpp")
    list(APPEND ProjectSources ${_srcs})
  endif()

  if(VISTA_CORELIBS_USE_OPENVR)
    file(GLOB _srcs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/OpenVRGlutWindowImp/*.cpp")
    list(APPEND ProjectSources ${_srcs})
  endif()
endif()

if(VISTA_CORELIBS_USE_SDL2_WINDOWIMP)
  file(GLOB _srcs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/SDL2WindowImp/*.cpp")
  list(APPEND ProjectSources ${_srcs})

  if(VISTA_CORELIBS_USE_OPENVR)
    file(GLOB _srcs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/OpenVRSDL2WindowImp/*.cpp")
    list(APPEND ProjectSources ${_srcs})
  endif()
endif()

file(GLOB ProjectHeaders
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Cluster/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Cluster/Utils/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/EventManager/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/GraphicsManager/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/InteractionManager/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/InteractionManager/DfnNodes/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenSG/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Stuff/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Stuff/ProximityWarning/*.h"
)

if(VISTA_CORELIBS_USE_ZEROMQ)
  file(GLOB _hdrs "${CMAKE_CURRENT_SOURCE_DIR}/Cluster/ZeroMQExt/*.h")
  list(APPEND ProjectHeaders ${_hdrs})
endif()

if(VISTA_CORELIBS_USE_GLUT_WINDOWIMP)
  file(GLOB _hdrs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/GlutWindowImp/*.h")
  list(APPEND ProjectHeaders ${_hdrs})

  if(VISTA_CORELIBS_USE_OCULUSSDK)
    file(GLOB _hdrs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/OculusGlutWindowImp/*.h")
    list(APPEND ProjectHeaders ${_hdrs})
  endif()

  if(VISTA_CORELIBS_USE_OPENVR)
    file(GLOB _hdrs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/OpenVRGlutWindowImp/*.h")
    list(APPEND ProjectHeaders ${_hdrs})
  endif()
endif()

if(VISTA_CORELIBS_USE_SDL2_WINDOWIMP)
  file(GLOB _hdrs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/SDL2WindowImp/*.h")
  list(APPEND ProjectHeaders ${_hdrs})

  if(VISTA_CORELIBS_USE_OPENVR)
    file(GLOB _hdrs "${CMAKE_CURRENT_SOURCE_DIR}/DisplayManager/OpenVRSDL2WindowImp/*.h")
    list(APPEND ProjectHeaders ${_hdrs})
  endif()
endif()

target_sources(VistaKernel
  PRIVATE
    ${ProjectSources}
  PUBLIC
    FILE_SET HEADERS
    FILES ${ProjectHeaders}
)

target_link_libraries(VistaKernel
  PUBLIC
    VistaBase
    VistaAspects
    VistaInterProcComm
    VistaMath
    VistaTools
    VistaDeviceDriversBase
    VistaDataFlowNet
    OpenGL::GL
    OpenGL::GLU
    GLEW::GLEW
    OpenSG::Base
    OpenSG::System
    $<$<BOOL:${VISTA_CORELIBS_USE_SDL2_WINDOWIMP}>:SDL2::SDL2 SDL2_ttf::SDL2_ttf>
    $<$<BOOL:${VISTA_CORELIBS_USE_GLUT_WINDOWIMP}>:GLUT::GLUT>
    $<$<BOOL:${VISTA_CORELIBS_USE_OPENVR}>:OpenVR::API>
    $<$<BOOL:${VISTA_CORELIBS_USE_OCULUSSDK}>:OVR::SDK>
    $<$<BOOL:${VISTA_CORELIBS_USE_ZEROMQ}>:cppzmq>
)

if (VISTA_USE_PRECOMPILED_HEADERS)
  target_precompile_headers(VistaKernel PRIVATE precompiled.pch)
endif ()

target_compile_definitions(VistaKernel
  PRIVATE
    VISTA_SYS_OPENSG
    $<$<PLATFORM_ID:Windows>:VISTAKERNEL_EXPORTS>
  PUBLIC
    VISTAKERNEL_USE_PROFILING
    $<$<BOOL:${VISTA_CORELIBS_USE_SDL2_WINDOWIMP}>:VISTA_WITH_SDL2>

    $<$<BOOL:${VISTA_CORELIBS_USE_GLUT_WINDOWIMP}>:VISTA_WITH_GLUT>
    $<$<BOOL:${VISTA_CORELIBS_GLUT_WINDOW_NOTIFY_FRAME_EXCEPTIONS}>:VISTA_GLUT_NOTIFY_FRAME_EXCEPTIONS>
    $<$<BOOL:${VISTA_CORELIBS_GLUT_WINDOW_ABORT_ON_FRAME_EXCEPTIONS}>:VISTA_GLUT_ABORT_ON_FRAME_EXCEPTIONS>

    $<$<BOOL:${VISTA_CORELIBS_USE_ZEROMQ}>:VISTA_WITH_ZEROMQ>
    $<$<BOOL:${VISTA_CORELIBS_USE_OCULUSSDK}>:VISTA_WITH_OCULUSSDK>
    $<$<BOOL:${VISTA_CORELIBS_USE_OPENVR}>:VISTA_WITH_OPENVR>
    $<$<BOOL:${VISTA_CORELIBS_USE_INFINITE_REVERSE_PROJECTION}>:VISTA_OPENVR_INFINITE_REVERSE_PROJECTION>
)

install(
    TARGETS VistaKernel
    EXPORT  VistaTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION "include/VistaKernel"
)
