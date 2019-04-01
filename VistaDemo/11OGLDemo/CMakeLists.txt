
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 11OGLDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 11OGLDemo ${ProjectSources} )
target_link_libraries( 11OGLDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 11OGLDemo )
vista_install( 11OGLDemo )
vista_create_default_info_file( 11OGLDemo )
if( VISTA_COMMON_BUILD )
	set_target_properties( 11OGLDemo PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 11OGLDemo VistaKernel )
endif()

