

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VCGAL_FOUND )
	
	vista_find_package_root( CGAL "include/CGAL/AABB_tree.h" )	
	
	if( CGAL_ROOT_DIR )
		# @TODO: boost_all_dyn_link not nice here...
		set( CGAL_DEFINITIONS "-DCGAL_USE_MPFR" "-DCGAL_USE_GMP" "-DBOOST_PARAMETER_MAX_ARITY=12" )
		set( CGAL_INCLUDE_DIRS "${CGAL_ROOT_DIR}/include" )
		if( UNIX )
			file( GLOB CGAL_LIBDIR_ENTRIES "${CGAL_ROOT_DIR}/lib/*.so" )
		else()
			file( GLOB CGAL_LIBDIR_ENTRIES "${CGAL_ROOT_DIR}/lib/*.lib" )
		endif()
		if( CGAL_LIBDIR_ENTRIES )
			set( CGAL_LIBRARY_DIRS "${CGAL_ROOT_DIR}/lib" "${CGAL_ROOT_DIR}/bin" )
			if( UNIX )
				set( CGAL_LIBRARIES CGAL CGAL_Core )
			endif()
		else()
			list( APPEND CGAL_DEFINITIONS "-DCGAL_HEADER_ONLY" )
		endif()
		
	endif()
	
	vista_add_package_dependency( CGAL Boost REQUIRED system thread )
	vista_add_package_dependency( CGAL gmp REQUIRED )
	vista_add_package_dependency( CGAL mpfr REQUIRED )
	
endif( NOT VCGAL_FOUND )

find_package_handle_standard_args( VCGAL "CGAL could not be found" CGAL_INCLUDE_DIRS )



