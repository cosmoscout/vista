

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENSG_FOUND )
	vista_find_package_root( OpenSG include/OpenSG/OSGAction.h )

	if( OPENSG_ROOT_DIR )
		if( UNIX )
			set( OPENSG_LIBRARY_DIRS ${OPENSG_ROOT_DIR}/lib )
			set( OPENSG_LIBRARIES
				OSGSystem
				OSGBase
			)
		elseif( WIN32 )
			set( OPENSG_LIBRARY_DIRS ${OPENSG_ROOT_DIR}/lib )
			set( OPENSG_LIBRARIES
				optimized OSGSystem
				optimized OSGBase
				debug OSGSystemD
				debug OSGBaseD
			)
		endif( UNIX )
		set( OPENSG_INCLUDE_DIRS ${OPENSG_ROOT_DIR}/include ${OPENSG_ROOT_DIR}/include/OpenSG )
		set( OPENSG_DEFINITIONS -DOSG_WITH_GLUT -DOSG_WITH_GIF -DOSG_WITH_TIF -DOSG_WITH_JPG -DOSG_BUILD_DLL -D_OSG_HAVE_CONFIGURED_H_ -DFT2_LIB -DFONTCONFIG_LIB)
		set( OPENSG_DEPENDENCIES package GLUT REQUIRED )
		
		if( UNIX )
		  string( REGEX MATCH ".*icpc" _IS_ICPC_COMPILER ${CMAKE_CXX_COMPILER} )
		  if( _IS_ICPC_COMPILER )
			set( OPENSG_DEFINITIONS ${OPENSG_DEFINITIONS} -DOSG_ICC_GNU_COMPAT )
		  endif( _IS_ICPC_COMPILER )
		endif( UNIX )
	endif( OPENSG_ROOT_DIR )

endif( NOT VOPENSG_FOUND )

find_package_handle_standard_args( VOpenSG "OpenSG could not be found" OPENSG_ROOT_DIR )
