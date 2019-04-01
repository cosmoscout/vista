include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VLIBXML2_FOUND )

	vista_find_package_root( LIBXML2 "include/libxml/xmlIO.h" NAMES libxml2 LIBXML2 LibXml2)
	
	if( NOT LIBXML2_FOUND )	
		vista_find_package_root( LIBXML2 "include/libxml/xmlIO.h" NAMES libxml2 LIBXML2 LibXml2)
	endif(NOT LIBXML2_FOUND)

	if(LIBXML2_ROOT_DIR)
		find_library( LIBXML2_LIBRARIES NAMES libxml2
							PATHS "${LIBXML2_ROOT_DIR}/lib" 
							NO_DEFAULT_PATH
							CACHE "LibXml2 library" )
		if( NOT LIBXML2_INCLUDE_DIRS)
			set(LIBXML2_INCLUDE_DIRS "${LIBXML2_ROOT_DIR}/include")
		endif(NOT LIBXML2_INCLUDE_DIRS)
		
		set(LIBXML2_INCLUDE_DIRS "${LIBXML2_INCLUDE_DIRS}")
		set(LIBXML2_LIBRARIES "${LIBXML2_LIBRARIES}")
	endif(LIBXML2_ROOT_DIR)
	
endif( NOT VLIBXML2_FOUND )

find_package_handle_standard_args( VLIBXML2 "libxml2 could not be found" LIBXML2_LIBRARIES LIBXML2_INCLUDE_DIRS)
