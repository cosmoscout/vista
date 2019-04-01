

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )
include( VistaHWArchSettings )

if( NOT VOPENCL_FOUND )

	# cl.h marks the cl distribution
	vista_find_package_root( OpenCL include/CL/cl.h )

	if( OPENCL_ROOT_DIR )
		#Provide specific libraries for release and debug build here
		set( OPENCL_LIBRARIES 
                                     optimized OpenCL
                                     debug OpenCL #vendor does not supply debug version
                   )

		set( OPENCL_INCLUDE_DIRS ${OPENCL_ROOT_DIR}/include )
		# Set platform specific OpenCL path here
		if( UNIX ) #UNIX is easy, as usual
			set( OPENCL_LIBRARY_DIRS ${OPENCL_ROOT_DIR}/lib )

		elseif(WIN32)
         		set( OPENCL_LIBRARY_DIRS ${OPENCL_ROOT_DIR}/lib)
         		#message( ${OPENCL_ROOT_DIR}/lib)
		endif(UNIX)
	endif( OPENCL_ROOT_DIR )

endif( NOT VOPENCL_FOUND )

find_package_handle_standard_args( VOPENCL "OPENCL could not be found" OPENCL_ROOT_DIR )
