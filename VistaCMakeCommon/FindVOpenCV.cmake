


include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENCV_FOUND )

	# search config file
	vista_find_package_root( OpenCV OpenCVConfig.cmake NAMES OpenCV opencv share/OpenCV share/opencv ADVANCED )
	if( NOT OPENCV_ROOT_DIR )
		vista_find_package_root( OpenCV share/OpenCVConfig.cmake NAMES OpenCV opencv share/OpenCV share/opencv ADVANCED )
	endif()
	if( NOT OPENCV_ROOT_DIR )
		vista_find_package_root( OpenCV share/OpenCV/OpenCVConfig.cmake NAMES OpenCV opencv share/OpenCV share/opencv ADVANCED )
	endif()

	set( OPENCV_FOUND )
	set( OpenCV_FOUND )
	set( CMAKE_PREFIX_PATH "${OPENCV_ROOT_DIR}" ${CMAKE_PREFIX_PATH} )
	vista_find_original_package( VOpenCV )
	# an OpenCVConfig.cmake has been found and loaded
	
	unset ( OPENCV_FOUND CACHE )
	
	if( OpenCV_FOUND )
		set( OPENCV_LIBRARIES "${OpenCV_LIBS}" )
		set( OPENCV_LIBRARY_DIRS "${OpenCV_LIB_DIR}" "${OpenCV_LIB_DIR}/../bin" )
		set( OPENCV_INCLUDE_DIRS "${OpenCV_INCLUDE_DIRS}" )
		set( OPENCV_VERSION "${OpenCV_VERSION}" )
	endif()
	
endif( NOT VOPENCV_FOUND )


find_package_handle_standard_args( VOpenCV "OpenCV could not be found" OPENCV_LIBRARIES )

