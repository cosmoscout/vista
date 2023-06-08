

vista_adopt_version( VistaKernel VistaCoreLibs )

set( VISTAKERNEL_INTERNAL_DEPENDENCIES 
			VistaBase
			VistaAspects
			VistaInterProcComm
			VistaMath
			VistaTools
			VistaDeviceDriversBase
			VistaDataFlowNet )
set( VISTAKERNEL_INTERNAL_DEPENDENCIES ${VISTAKERNEL_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

vista_use_package( OpenGL REQUIRED )
vista_use_package( OpenSG 1.8 REQUIRED SYSTEM_HEADERS )
vista_use_package( GLEW REQUIRED )

if( VISTACORELIBS_BUILD_WINDOWIMP_GLUT )
	vista_use_package( GLUT REQUIRED )
	add_definitions( -DVISTA_WITH_GLUT )
	
	set( VISTACORELIBS_GLUT_WINDOW_NOTIFY_FRAME_EXCEPTIONS FALSE CACHE BOOL "If enabled, the whole vista frame processing will be surrounded by a try-catch to cacth any std::exception. If an exception is caught, an error is printed to the error stream, and the exception is re-thrown. This is useful for 64bit Windows version, where some kernel function calls (as the window callback used in glut) silently drop any unhandled exceptions" )
	if( VISTACORELIBS_GLUT_WINDOW_NOTIFY_FRAME_EXCEPTIONS )
		add_definitions( -DVISTA_GLUT_NOTIFY_FRAME_EXCEPTIONS )
	endif()
	set( VISTACORELIBS_GLUT_WINDOW_ABORT_ON_FRAME_EXCEPTIONS FALSE CACHE BOOL "If enabled, the whole vista frame processing will be surrounded by a try-catch to cacth any std::exception. If an exception is caught, and error is printed and VistaSytem::Quit() is called. This is useful for 64bit Windows version, where some kernel function calls (as the window callback used in glut) silently drop any unhandled exceptions" )
	if( VISTACORELIBS_GLUT_WINDOW_ABORT_ON_FRAME_EXCEPTIONS )
		add_definitions( -DVISTA_GLUT_ABORT_ON_FRAME_EXCEPTIONS )
	endif()
endif()
if( VISTACORELIBS_BUILD_WINDOWIMP_OSG )
	vista_use_package( OpenSceneGraph 3+ REQUIRED osgViewer osgGA )
	vista_use_package( GLUT REQUIRED ) # currently still needed for text
	add_definitions( -DVISTA_WITH_OSG )
endif()

vista_find_package( ZMQ COMPONENTS PGM QUIET )
set( VISTACORELIBS_USE_ZEROMQ ${ZMQ_FOUND} CACHE BOOL "if activated, zeromq-specific stuff will be built" )
if( VISTACORELIBS_USE_ZEROMQ )
	vista_use_package( ZMQ REQUIRED PGM )
	add_definitions( -DVISTA_WITH_ZEROMQ )
endif()

set( VISTACORELIBS_USE_OCULUSSDK OFF CACHE BOOL "if activated, the OculusSDK viewport will be available" )
if( VISTACORELIBS_USE_OCULUSSDK )
	vista_use_package( OculusSDK REQUIRED 0.5  )
	add_definitions( -DVISTA_WITH_OCULUSSDK )
endif()

set( VISTACORELIBS_USE_OPENVR OFF CACHE BOOL "if activated, the OpenVR viewport will be available" )
if( VISTACORELIBS_USE_OPENVR )
	vista_use_package( OpenVR REQUIRED 0.5  )
	add_definitions( -DVISTA_WITH_OPENVR )
endif()

set( VISTACORELIBS_USE_INFINITE_REVERSE_PROJECTION OFF CACHE BOOL "if activated, matrices will be modified to use infinite reverse projection" )
if( VISTACORELIBS_USE_INFINITE_REVERSE_PROJECTION )
	add_definitions( -DVISTA_OPENVR_INFINITE_REVERSE_PROJECTION )
endif()

if( NOT VISTACORELIBS_BUILD_WINDOWIMP_GLUT AND NOT VISTACORELIBS_BUILD_WINDOWIMP_OSG )
	message( WARNING "No Windowing Toolkit will be built!" )
endif()

add_definitions( -DVISTA_SYS_OPENSG -DVISTAKERNEL_USE_PROFILING )

include( "./_SourceFiles.cmake" )

add_library( VistaKernel ${ProjectSources} )
target_link_libraries( VistaKernel
	${VISTAKERNEL_INTERNAL_DEPENDENCIES}
	${VISTA_USE_PACKAGE_LIBRARIES}
)

if (VISTA_USE_PRECOMPILED_HEADERS)
	target_precompile_headers(VistaKernel PRIVATE precompiled.pch)
endif()

vista_configure_lib( VistaKernel )
vista_install( VistaKernel VistaKernel )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaKernel )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaKernel )
set_property( TARGET VistaKernel PROPERTY LINK_INTERFACE_LIBRARIES "" )