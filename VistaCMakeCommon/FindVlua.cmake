

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VLUA_FOUND )
	vista_find_package_root( lua include/lua.h )

	if( LUA_ROOT_DIR )
		set( LUA_INCLUDE_DIRS "${LUA_ROOT_DIR}/include" )
		set( LUA_LIBRARY_DIRS "${LUA_ROOT_DIR}/lib" )
		vista_find_library_uncached_var( _RELEASE_LIB lua PATHS ${LUA_LIBRARY_DIRS} NO_DEFAULT_PATH )
		vista_find_library_uncached_var( _DEBUG_LIB lua PATHS ${LUA_LIBRARY_DIRS} NO_DEFAULT_PATH )
		if( _RELEASE_LIB AND _DEBUG_LIB )
			set( LUA_LIBRARIES optimized lua debug luaD )
		elseif( UNIX AND _RELEASE_LIB )
			set( LUA_LIBRARIES lua )
		endif()
	endif()
		
	if( NOT LUA_LIBRARIES )
		if( NOT Vlua_FIND_VERSION )
			message( "no version" )
			find_package( Lua51 )
		elseif( Vlua_FIND_VERSION VERSION_EQUAL 5.2 )
			find_package( Lua )
		elseif( Vlua_FIND_VERSION VERSION_EQUAL 5.1 )
			find_package( Lua51 )
		elseif( Vlua_FIND_VERSION VERSION_EQUAL 5.0 )
			find_package( Lua50 )
		endif( NOT Vlua_FIND_VERSION )
		set( LUA_INCLUDE_DIRS "${LUA_INCLUDE_DIR}" )
		get_filename_component( LUA_LIBRARY_DIRS "${LUA_LIBRARIES}" PATH )
	endif()

endif( NOT VLUA_FOUND )

find_package_handle_standard_args( Vlua "lua could not be found" LUA_ROOT_DIR LUA_LIBRARIES )

