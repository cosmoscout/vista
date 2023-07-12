
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 05InteractionDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )

#include the sener sub-app into the build
add_subdirectory( Sender )

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 05InteractionDemo ${ProjectSources} )
target_link_libraries( 05InteractionDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 05InteractionDemo )
vista_install( 05InteractionDemo )
vista_create_default_info_file( 05InteractionDemo )
if( VISTA_COMMON_BUILD )
	set_target_properties( 05InteractionDemo PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 05InteractionDemo VistaKernel )
endif()
