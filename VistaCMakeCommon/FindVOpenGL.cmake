

include( FindPackageHandleStandardArgs )

if( NOT VOPENGL_FOUND )

	find_package( OpenGL )

	set( OPENGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR} )

endif( NOT VOPENGL_FOUND )

find_package_handle_standard_args( VOpenGL "OpenGL could not be found" OPENGL_LIBRARIES )

