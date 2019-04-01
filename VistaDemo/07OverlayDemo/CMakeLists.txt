
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 07OverlayDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 07OverlayDemo ${ProjectSources} )
target_link_libraries( 07OverlayDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 07OverlayDemo )
vista_install( 07OverlayDemo )
vista_create_default_info_file( 07OverlayDemo )
if( VISTA_COMMON_BUILD )
	set_target_properties( 07OverlayDemo PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 07OverlayDemo VistaKernel )
endif()
