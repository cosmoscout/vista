
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 16PhantomDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )
vista_use_package( OpenHaptics REQUIRED ) # for dll's of phantom driver

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 16PhantomDemo ${ProjectSources} )
target_link_libraries( 16PhantomDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 16PhantomDemo )
vista_install( 16PhantomDemo )
vista_create_default_info_file( 16PhantomDemo )
if( VISTA_COMMON_BUILD )
	set_target_properties( 16PhantomDemo PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 16PhantomDemo VistaKernel )
endif()
