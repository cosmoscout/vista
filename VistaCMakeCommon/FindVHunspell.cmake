

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VHUNSPELL_FOUND )

	vista_find_package_root( HUNSPELL include/hunspell.h ) # Arbitrary header chosen

	if( HUNSPELL_ROOT_DIR )
		set( HUNSPELL_LIBRARIES 
			optimized hunspell
			debug hunspellD			
		)
		mark_as_advanced( HUNSPELL_LIBRARIES )

		set( HUNSPELL_INCLUDE_DIRS ${HUNSPELL_ROOT_DIR}/include )
		set( HUNSPELL_LIBRARY_DIRS ${HUNSPELL_ROOT_DIR}/lib )
	endif( HUNSPELL_ROOT_DIR )

endif( NOT VHUNSPELL_FOUND )

find_package_handle_standard_args( VHUNSPELL "HUNSPELL could not be found" HUNSPELL_ROOT_DIR )

