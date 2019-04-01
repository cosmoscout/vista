

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VGLEW_FOUND )

	vista_find_package_root( GLEW include/GL/glew.h )

	if( GLEW_ROOT_DIR )
		find_library( GLEW_LIBRARIES NAMES glew glew32 glew64 GLEW
					PATHS ${GLEW_ROOT_DIR}/lib ${GLEW_ROOT_DIR}/lib/Release/Win32
					CACHE "GLEW library" )
		mark_as_advanced( GLEW_LIBRARIES )

		set( GLEW_INCLUDE_DIRS ${GLEW_ROOT_DIR}/include )
		set( GLEW_LIBRARY_DIRS ${GLEW_ROOT_DIR}/lib ${GLEW_ROOT_DIR}/lib/Release/Win32 )
		get_filename_component( GLEW_LIBRARY_DIRS ${GLEW_LIBRARIES} PATH )

	endif( GLEW_ROOT_DIR )

endif( NOT VGLEW_FOUND )

find_package_handle_standard_args( VGLEW "GLEW could not be found" GLEW_ROOT_DIR )

