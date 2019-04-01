
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 02GeometryDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )


# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 02GeometryDemo ${ProjectSources} )
target_link_libraries( 02GeometryDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 02GeometryDemo )
vista_install( 02GeometryDemo )
vista_create_default_info_file( 02GeometryDemo )
if( VISTA_COMMON_BUILD )
	set_target_properties( 02GeometryDemo PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 02GeometryDemo VistaKernel )
endif()
