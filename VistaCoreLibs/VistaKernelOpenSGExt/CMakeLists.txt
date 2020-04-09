

vista_adopt_version( VistaKernelOpenSGExt VistaCoreLibs )

set( VISTAKERNELOPENSGEXT_INTERNAL_DEPENDENCIES 
			VistaBase
			VistaAspects
			VistaInterProcComm
			VistaMath
			VistaTools
			VistaDeviceDriversBase
			VistaDataFlowNet
			VistaKernel )
set( VISTAKERNELOPENSGEXT_INTERNAL_DEPENDENCIES ${VISTAKERNELOPENSGEXT_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

vista_use_package( GLEW REQUIRED )
vista_use_package( OpenGL REQUIRED )
vista_use_package( GLUT REQUIRED )
vista_use_package( OpenSG 1.8 REQUIRED SYSTEM_HEADERS )

include( "./_SourceFiles.cmake" )

add_library( VistaKernelOpenSGExt ${ProjectSources} )
target_link_libraries( VistaKernelOpenSGExt
	${VISTAKERNELOPENSGEXT_INTERNAL_DEPENDENCIES}
	${VISTA_USE_PACKAGE_LIBRARIES}
)

if (${CMAKE_VERSION} GREATER_EQUAL 3.16)
	target_precompile_headers(VistaKernelOpenSGExt REUSE_FROM VistaBase)
endif()

vista_configure_lib( VistaKernelOpenSGExt )
vista_install( VistaKernelOpenSGExt VistaKernelOpenSGExt )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaKernelOpenSGExt )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaKernelOpenSGExt )

