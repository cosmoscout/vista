

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VTETGEN_FOUND )

	vista_find_package_root( tetgen include/tetgen.h NAMES tetgen TETGEN TetGen )

	if( TETGEN_ROOT_DIR )
		vista_find_library_uncached( NAMES tetgen tet PATHS "${TETGEN_ROOT_DIR}/lib" NO_DEFAULT_PATH )
		set( TETGEN_LIBRARY_RELEASE "${VISTA_UNCACHED_LIBRARY}" )
		vista_find_library_uncached( TETGEN_LIBRARY_DEBUG NAMES tetgenD tetD PATHS "${TETGEN_ROOT_DIR}/lib" NO_DEFAULT_PATH )
		set( TETGEN_LIBRARY_DEBUG "${VISTA_UNCACHED_LIBRARY}" )
		
		if( TETGEN_LIBRARY_RELEASE AND ( TETGEN_LIBRARY_DEBUG OR NOT WIN32 ) )
			if( TETGEN_LIBRARY_DEBUG )
				set( TETGEN_LIBRARIES optimized "${TETGEN_LIBRARY_RELEASE}" debug "${TETGEN_LIBRARY_DEBUG}" )
			else()
				set( TETGEN_LIBRARIES "${TETGEN_LIBRARY_RELEASE}" )
			endif()
			set( TETGEN_INCLUDE_DIRS "${TETGEN_ROOT_DIR}/include" )
			set( TETGEN_LIBRARY_DIRS "${TETGEN_ROOT_DIR}/lib" )
		endif()
	endif()

endif()

find_package_handle_standard_args( Vtetgen "tetgen could not be found" TETGEN_ROOT_DIR )

