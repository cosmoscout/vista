
cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 13KernelOpenSGExtParticlesDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif()

vista_use_package( VistaCoreLibs REQUIRED VistaKernel VistaKernelOpenSGExt FIND_DEPENDENCIES )
	
# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 13KernelOpenSGExtParticlesDemo ${ProjectSources} )
target_link_libraries( 13KernelOpenSGExtParticlesDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 13KernelOpenSGExtParticlesDemo )
vista_install( 13KernelOpenSGExtParticlesDemo )
vista_create_default_info_file( 13KernelOpenSGExtParticlesDemo )
if( VISTA_COMMON_BUILD )
	set_target_properties( 13KernelOpenSGExtParticlesDemo PROPERTIES FOLDER "VistaDemo" )
	add_dependencies( 13KernelOpenSGExtParticlesDemo VistaKernelOpenSGExt )
endif()
