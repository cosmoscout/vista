# $Id: FindVSKETCHUP.cmake 21495 2011-05-25 07:52:18Z dr165799 $

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VSKETCHUP_FOUND )
	vista_find_package_root( SKETCHUP "include/slapi/slapi.h" )	

	if( SKETCHUP_ROOT_DIR )
		set( SKETCHUP_INCLUDE_DIRS "${SKETCHUP_ROOT_DIR}/include" )
		set( SKETCHUP_LIBRARIES "slapi" "sketchup" )
		set( SKETCHUP_LIBRARY_DIRS "${SKETCHUP_ROOT_DIR}/lib" "${SKETCHUP_ROOT_DIR}/bin" )
	else( SKETCHUP_ROOT_DIR )
		message( WARNING "vista_find_package_root - File named slapi.h not found" )	
	endif( SKETCHUP_ROOT_DIR )
endif( NOT VSKETCHUP_FOUND )

find_package_handle_standard_args( VSketchUp "SketchUp API could not be found" SKETCHUP_ROOT_DIR )

