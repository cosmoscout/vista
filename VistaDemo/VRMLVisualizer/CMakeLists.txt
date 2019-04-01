cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( VRMLVisualizer )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif( NOT VISTADEMO_COMMON_BUILD )

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( VRMLVisualizer ${ProjectSources} )
target_link_libraries( VRMLVisualizer
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( VRMLVisualizer )
vista_install( VRMLVisualizer )
vista_create_default_info_file( VRMLVisualizer )
if( VISTA_COMMON_BUILD )
	set_target_properties( VRMLVisualizer PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( VRMLVisualizer VistaKernel )
endif()

