
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 09EventDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 09EventDemo ${ProjectSources} )
target_link_libraries( 09EventDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 09EventDemo )
vista_install( 09EventDemo )
vista_create_default_info_file( 09EventDemo )
if( VISTA_COMMON_BUILD )
	set_target_properties( 09EventDemo PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 09EventDemo VistaKernel )
endif()
