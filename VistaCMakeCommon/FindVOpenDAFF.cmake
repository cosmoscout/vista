include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENDAFF_FOUND )
    vista_find_package_root( OpenDAFF include/DAFF.h ) # find dir containing 'DAFF.h'
    if( OPENDAFF_ROOT_DIR )
        set( OPENDAFF_INCLUDE_DIRS "${OPENDAFF_ROOT_DIR}/include" )
        set( OPENDAFF_LIBRARY_DIRS "${OPENDAFF_ROOT_DIR}/lib" "${OPENDAFF_ROOT_DIR}/bin" )
		set( OPENDAFF_LIBRARIES optimized DAFF debug DAFFD )
    endif( OPENDAFF_ROOT_DIR )
endif( NOT VOPENDAFF_FOUND )

find_package_handle_standard_args( VOpenDAFF "OpenDAFF could not be found" OPENDAFF_ROOT_DIR )