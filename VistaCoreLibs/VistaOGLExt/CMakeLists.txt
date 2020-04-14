

vista_adopt_version( VistaOGLExt VistaCoreLibs )

set( VISTAOGLEXT_INTERNAL_DEPENDENCIES 
			VistaBase 
			VistaAspects
			VistaTools
			VistaKernel )
set( VISTAOGLEXT_INTERNAL_DEPENDENCIES ${VISTAOGLEXT_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

vista_use_package( GLEW REQUIRED )
vista_use_package( OpenGL REQUIRED )


if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAOGLEXT_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAOGLEXT_STATIC )
	endif( BUILD_SHARED_LIBS )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaOGLExt ${ProjectSources} )
target_link_libraries( VistaOGLExt
	${VISTAOGLEXT_INTERNAL_DEPENDENCIES}
	${VISTA_USE_PACKAGE_LIBRARIES}
)

if (${VISTA_ENABLE_PCH})
	target_precompile_headers(VistaOGLExt PRIVATE precompiled.pch)
endif()

vista_find_shader_dirs( VistaOGLExt )

set( VISTACORELIBS_SHADER_DIRS ${VISTAOGLEXT_SHADER_DIRS} PARENT_SCOPE )

vista_configure_lib( VistaOGLExt )
vista_install( VistaOGLExt VistaOGLExt )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaOGLExt )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaOGLExt )