


cmake_minimum_required( VERSION 2.8 )

project( OpenCVFaceTrackDemo )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
include( VistaCommon )


vista_use_package( VistaCoreLibs "HEAD" REQUIRED VistaKernel FIND_DEPENDENCIES )
vista_use_package( OpenCV REQUIRED ) # so that we find the libs when loading the dlls
vista_use_package( CLEye QUIET )


add_executable( OpenCVFaceTrackDemo main.cpp configfiles/vista.ini configfiles/ucp_facetrack.xml configfiles/keyboard.xml )
target_link_libraries( OpenCVFaceTrackDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( OpenCVFaceTrackDemo )
vista_create_default_info_file( OpenCVFaceTrackDemo )

