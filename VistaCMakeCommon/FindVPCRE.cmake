include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VPCRE_FOUND )
    vista_find_package_root( pcre include/pcre.h ) # find dir containing 'pcre.h'
    if( PCRE_ROOT_DIR )
        set( PCRE_INCLUDE_DIRS "${PCRE_ROOT_DIR}/include" )
        set( PCRE_LIBRARY_DIRS "${PCRE_ROOT_DIR}/lib" )
		set( PCRE_LIBRARIES optimized pcre debug pcred optimized pcrecpp debug pcrecppd optimized pcreposix debug pcreposixd )
    endif( PCRE_ROOT_DIR )
endif( NOT VPCRE_FOUND )

find_package_handle_standard_args( VPCRE "PCRE could not be found" PCRE_ROOT_DIR )