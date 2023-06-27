

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VBOOST_FOUND )
	if( NOT BOOST_ROOT )
		set( Boost_NO_BOOST_CMAKE ON )
		vista_find_package_root( Boost boost/any.hpp NAMES Boost boost NO_CACHE )
		if( NOT BOOST_ROOT_DIR )
			vista_find_package_root( Boost include/boost/any.hpp NAMES Boost boost NO_CACHE )
		endif()

		if( BOOST_ROOT_DIR )
			set( BOOST_ROOT "${BOOST_ROOT_DIR}" )			
		endif( BOOST_ROOT_DIR )
	endif( NOT BOOST_ROOT )
	
	
	set( Boost_VERSION ) # this was set by vista_find_package_root, so we need to reset it
	vista_find_original_package( VBoost )
	
	set( BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} )
	set( BOOST_LIBRARY_DIRS ${Boost_LIBRARY_DIRS} )
	set( BOOST_LIBRARIES ${Boost_LIBRARIES} )

endif( NOT VBOOST_FOUND )

find_package_handle_standard_args( VBOOST "BOOST could not be found" BOOST_INCLUDE_DIRS )





