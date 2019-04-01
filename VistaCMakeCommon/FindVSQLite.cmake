

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VSQLITE_FOUND )
	vista_find_package_root( SQLITE include/sqlite3.h )

	if( SQLITE_ROOT_DIR )
		find_library( SQLITE_LIBRARIES sqlite3
				PATHS ${SQLITE_ROOT_DIR}/lib
				CACHE "SQLite library" )
		mark_as_advanced( SQLITE_LIBRARIES )

		set( SQLITE_INCLUDE_DIRS ${SQLITE_ROOT_DIR}/include )
		#set( SQLITE_LIBRARY_DIRS ${SQLITE_ROOT_DIR}/lib )
		get_filename_component( SQLITE_LIBRARY_DIRS ${SQLITE_LIBRARIES} PATH )
	endif( SQLITE_ROOT_DIR )
endif( NOT VSQLITE_FOUND )

find_package_handle_standard_args( VSQLITE "SQLite could not be found" SQLITE_ROOT_DIR )

