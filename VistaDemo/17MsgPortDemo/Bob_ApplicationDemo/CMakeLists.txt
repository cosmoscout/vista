
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 17MsgPortBob )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 17MsgPortBob ${ProjectSources} )
target_link_libraries( 17MsgPortBob
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 17MsgPortBob )
vista_install( 17MsgPortBob )
vista_create_default_info_file( 17MsgPortBob )
if( VISTA_COMMON_BUILD )
	set_target_properties( 17MsgPortBob PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 17MsgPortBob VistaKernel )
endif()
