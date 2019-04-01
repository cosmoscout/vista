

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VQWT_FOUND )
	vista_find_package_root( Qwt include/qwt.h )

	if( QWT_ROOT_DIR )
		find_library( QWT_LIBRARIES_OPT NAMES qwt
					PATHS ${QWT_ROOT_DIR}/lib
					CACHE "Qwt library" )
		find_library( QWT_LIBRARIES_DBG NAMES qwtd
					PATHS ${QWT_ROOT_DIR}/lib
					CACHE "Qwt library" )
		
		if(QWT_LIBRARIES_DBG)
			set( QWT_LIBRARIES
			optimized ${QWT_LIBRARIES_OPT}
			debug ${QWT_LIBRARIES_DBG} )
		else(QWT_LIBRARIES_DBG)
			set( QWT_LIBRARIES
			${QWT_LIBRARIES_OPT} )
		endif(QWT_LIBRARIES_DBG)


		set( QWT_INCLUDE_DIRS ${QWT_ROOT_DIR}/include )
		set( QWT_LIBRARY_DIRS ${QWT_ROOT_DIR}/lib )
		get_filename_component( QWT_LIBRARY_DIRS ${QWT_LIBRARIES_OPT} PATH )

	endif( QWT_ROOT_DIR )

endif( NOT VQWT_FOUND )

find_package_handle_standard_args( VQWT "Qwt could not be found" QWT_ROOT_DIR )

