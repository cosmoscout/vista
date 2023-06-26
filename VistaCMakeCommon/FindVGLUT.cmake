


include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VGLUT_FOUND )

	if( NOT GLUT_ROOT_DIR )
		# Glut was not found yet in prior config runs
		# first, check if we find the root dir on our own
		
		vista_find_package_root( GLUT "include/GL/freeglut.h" NAMES freeglut glut OpenSG)
		if( NOT GLUT_ROOT_DIR )
			vista_find_package_root( GLUT "include/GL/glut.h" NAMES freeglut glut OpenSG)
		endif()
		
		unset( GLUT_LIBRARIES )
		find_library( GLUT_LIBRARIES NAMES freeglut freeglut-msvc90x86 glut glut32
						PATHS "${GLUT_ROOT_DIR}/lib" "${GLUT_ROOT_DIR}/lib/opt"
						NO_DEFAULT_PATH
						CACHE "Glut/freeglut library" )

		if( GLUT_ROOT_DIR AND GLUT_LIBRARIES )
		
			set( GLUT_INCLUDE_DIRS "${GLUT_ROOT_DIR}/include" )
			get_filename_component( GLUT_LIBRARY_DIRS ${GLUT_LIBRARIES} PATH )
			set( GLUT_FOUND_BY_DEFAULT_MODULE FALSE CACHE INTERNAL "" FORCE )

		else( GLUT_ROOT_DIR AND GLUT_LIBRARIES )
		
			find_package( GLUT )

			if( GLUT_FOUND )
				set( GLUT_INCLUDE_DIRS "${GLUT_INCLUDE_DIR}" )
				set( GLUT_ROOT_DIR "${GLUT_INCLUDE_DIRS}" )
				set( GLUT_LIBRARIES "${GLUT_glut_LIBRARY}" )
				get_filename_component( GLUT_LIBRARY_DIRS "${GLUT_LIBRARIES}" PATH )
				set( GLUT_FOUND_BY_DEFAULT_MODULE TRUE CACHE INTERNAL "" FORCE )
			endif( GLUT_FOUND )

		endif( GLUT_ROOT_DIR AND GLUT_LIBRARIES )
		
	else()
		# we already found glut on prior config run -> GLUT_ROOT_DIR is set
		# we now have to differentiate between custom found, or with native find_glut
		if( NOT GLUT_FOUND_BY_DEFAULT_MODULE )
			if( NOT GLUT_LIBRARIES )
				set( GLUT_LIBRARIES "GLUT_LIBRARIES-NOTFOUND" )
			endif()
			find_library( GLUT_LIBRARIES NAMES freeglut freeglut-msvc90x86 glut glut32
						PATHS "${GLUT_ROOT_DIR}/lib" "${GLUT_ROOT_DIR}/lib/opt"
						NO_DEFAULT_PATH
						CACHE "Glut/freeglut library" )
						
			set( GLUT_INCLUDE_DIRS "${GLUT_ROOT_DIR}/include" )
			get_filename_component( GLUT_LIBRARY_DIRS "${GLUT_LIBRARIES}" PATH )
			
		else( NOT GLUT_FOUND_BY_DEFAULT_MODULE )
		
			find_package( GLUT )

			if( GLUT_FOUND )
				set( GLUT_INCLUDE_DIRS "${GLUT_INCLUDE_DIR}" )
				set( GLUT_ROOT_DIR "${GLUT_INCLUDE_DIRS}" )
				set( GLUT_LIBRARIES "${GLUT_glut_LIBRARY}" )
				get_filename_component( GLUT_LIBRARY_DIRS "${GLUT_LIBRARIES}" PATH )
				set( GLUT_FOUND_BY_DEFAULT_MODULE TRUE CACHE INTERNAL "" FORCE )
			endif( GLUT_FOUND )
		endif( NOT GLUT_FOUND_BY_DEFAULT_MODULE )
			
	endif( NOT GLUT_ROOT_DIR )
	
	# check if we do have a real glut
	if( NOT EXISTS "${GLUT_INCLUDE_DIRS}/GL/freeglut.h" )
		set( GLUT_DEFINITIONS "-DUSE_NATIVE_GLUT" )
	else()
		set( GLUT_DEFINITIONS "" )
	endif( NOT EXISTS "${GLUT_INCLUDE_DIRS}/GL/freeglut.h" )

endif( NOT VGLUT_FOUND )

find_package_handle_standard_args( VGLUT "glut/Freeglut could not be found" GLUT_ROOT_DIR GLUT_LIBRARIES )

