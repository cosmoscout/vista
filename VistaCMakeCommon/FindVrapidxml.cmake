# $Id: $

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VRAPIDXML_FOUND )
	vista_find_package_root( rapidxml "rapidxml.hpp" )

	if( RAPIDXML_ROOT_DIR )
		set( RAPIDXML_INCLUDE_DIRS "${RAPIDXML_ROOT_DIR}" )
	endif( RAPIDXML_ROOT_DIR )
endif( NOT VRAPIDXML_FOUND )

find_package_handle_standard_args( VRAPIDXML "rapidxml could not be found" RAPIDXML_ROOT_DIR )

