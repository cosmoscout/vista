

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VOPENAL_FOUND )
	# try three options: once with include subdir AL, once without, and - if both fail - a general FindOpenAL file
	
	vista_find_package_root( OpenAL "include/AL/al.h" NAMES "OpenAL" "OpenALSoft" "OpenAL 1.1 SDK" )
	vista_find_package_root( OpenAL "include/al.h" NAMES "OpenAL" "OpenALSoft" "OpenAL 1.1 SDK" )

	if( OPENAL_ROOT_DIR )
		if( EXISTS "${OPENAL_ROOT_DIR}/include/AL/al.h" )
			set( OPENAL_INCLUDE_DIRS ${OPENAL_ROOT_DIR}/include/AL )
		elseif( EXISTS "${OPENAL_ROOT_DIR}/include/al.h" )
			set( OPENAL_INCLUDE_DIRS ${OPENAL_ROOT_DIR}/include )
		endif( EXISTS "${OPENAL_ROOT_DIR}/include/AL/al.h" )

		set( OPENAL_LIBRARIES ${OPENAL_LIBRARIES} CACHE INTERNAL "" FORCE )
		
		if( VISTA_64BIT )
			vista_find_library_uncached(
				NAMES OpenAL al openal OpenAL32
				PATH_SUFFIXES lib lib64 libs64 libs/Win64
				PATHS
				${OPENAL_ROOT_DIR}
			)
		else()
			vista_find_library_uncached(
				NAMES OpenAL al openal OpenAL32
				PATH_SUFFIXES lib libs libs/Win32
				PATHS
				${OPENAL_ROOT_DIR}
			)
		endif()
		if( VISTA_UNCACHED_LIBRARY )
			set( OPENAL_LIBRARIES ${VISTA_UNCACHED_LIBRARY} )
			get_filename_component( OPENAL_LIBRARY_DIRS ${VISTA_UNCACHED_LIBRARY} PATH )
			# look for binary folder
			set( SEARCH_OPENAL_DLL OPENAL_DLL-NOTFOUND CACHE INTERNAL "" FORCE )
			if( VISTA_64BIT )
				find_file( SEARCH_OPENAL_DLL NAMES OpenAL.dll OpenAL32.dll 
						PATHS "${OPENAL_ROOT_DIR}" "${OPENAL_ROOT_DIR}/bin" "${OPENAL_ROOT_DIR}/bin/Win64"
								"${OPENAL_ROOT_DIR}/lib" "${OPENAL_ROOT_DIR}/lib64" "${OPENAL_ROOT_DIR}/lib/Win64" "${OPENAL_ROOT_DIR}/libs/Win64" )
			else()
				find_file( SEARCH_OPENAL_DLL NAMES OpenAL.dll OpenAL32.dll 
						PATHS "${OPENAL_ROOT_DIR}" "${OPENAL_ROOT_DIR}/bin" "${OPENAL_ROOT_DIR}/bin/Win64"
								"${OPENAL_ROOT_DIR}/lib" "${OPENAL_ROOT_DIR}/lib64" "${OPENAL_ROOT_DIR}/lib/Win64" "${OPENAL_ROOT_DIR}/libs/Win64" )
			endif()
			if( SEARCH_OPENAL_DLL )
				get_filename_component( OPENAL_BINARY_DIRS ${SEARCH_OPENAL_DLL} PATH )
				list( APPEND OPENAL_LIBRARY_DIRS ${OPENAL_BINARY_DIRS} )
			endif()
		endif( VISTA_UNCACHED_LIBRARY )
	else( OPENAL_ROOT_DIR )
		# try using a general FindOpenAL.cmake
		find_package( OpenAL )
		if( OPENAL_FOUND )
			set( OPENAL_LIBRARIES ${OPENAL_LIBRARY} )
			set( OPENAL_LIBRARY ${OPENAL_LIBRARY} CACHE INTERNAL "" FORCE )
			set( OPENAL_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR} )
			get_filename_component( _DIR ${OPENAL_INCLUDE_DIRS} PATH  )
			set( OPENAL_ROOT_DIR ${_DIR} CACHE PATH "OpenAL package rot dir" )
        else ( OPENAL_FOUND )
            message( WARNING "vista_find_package_root - File $(VISTA_CMAKE_COMMON)/OpenAL 1.1 SDK/include/[AL]/al.h  not found" )	
		endif( OPENAL_FOUND )
	endif( OPENAL_ROOT_DIR )
endif( NOT VOPENAL_FOUND )

find_package_handle_standard_args( VOpenAL "OpenAL could not be found" OPENAL_ROOT_DIR OPENAL_LIBRARIES )
