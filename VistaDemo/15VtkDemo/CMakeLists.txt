
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 15VtkDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )
vista_use_package( VTK           REQUIRED             FIND_DEPENDENCIES )


# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 15VtkDemo ${ProjectSources} )
target_link_libraries( 15VtkDemo
	${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_app( 15VtkDemo )
vista_install( 15VtkDemo )
vista_create_default_info_file( 15VtkDemo )
if( VISTA_COMMON_BUILD )
	set_target_properties( 15VtkDemo PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 15VtkDemo VistaKernel )
endif()

