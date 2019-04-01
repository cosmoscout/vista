

cmake_minimum_required( VERSION 2.8 )

project( KinectDemo )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
include( VistaCommon )

vista_use_package( OpenNI REQUIRED )
vista_use_package( VistaCoreLibs HEAD REQUIRED VistaKernel FIND_DEPENDENCIES )

add_executable( KinectDemo main.cpp KinectHandler.h KinectHandler.cpp configfiles/vista.ini configfiles/ucp_kinect.xml configfiles/spatial_kinect.xml )
source_group( "Source Files" FILES main.cpp KinectHandler.h KinectHandler.cpp )
target_link_libraries( KinectDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( KinectDemo )
vista_create_default_info_file( KinectDemo )

