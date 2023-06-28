

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VGLFW_FOUND )

	vista_find_package_root( GLFW include/GLFW/glfw3.h )

	if( GLFW_ROOT_DIR )
		find_library( GLFW_LIBRARIES NAMES glfw glfw3 GLFW
					PATHS ${GLFW_ROOT_DIR}/lib 
					CACHE "GLFW library" )
		mark_as_advanced( GLFW_LIBRARIES )

		set( GLFW_INCLUDE_DIRS ${GLFW_ROOT_DIR}/include )
		set( GLFW_LIBRARY_DIRS ${GLFW_ROOT_DIR}/lib  )
		get_filename_component( GLFW_LIBRARY_DIRS ${GLFW_LIBRARIES} PATH )

	endif( GLFW_ROOT_DIR )

endif( NOT VGLFW_FOUND )

find_package_handle_standard_args( VGLFW "GLFW could not be found" GLFW_ROOT_DIR )

