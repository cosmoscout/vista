
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 14DataFlowNetDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )	
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 14DataFlowNetDemo ${ProjectSources} )
target_link_libraries( 14DataFlowNetDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

set_target_properties( 14DataFlowNetDemo PROPERTIES UNITY_BUILD Off )

vista_configure_app( 14DataFlowNetDemo )
vista_install( 14DataFlowNetDemo )
vista_create_default_info_file( 14DataFlowNetDemo )
if( VISTA_COMMON_BUILD )
	set_target_properties( 14DataFlowNetDemo PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 14DataFlowNetDemo VistaKernel )
endif()

