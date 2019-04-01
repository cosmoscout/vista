cmake_minimum_required( VERSION 2.8 )
project( Vista )

# Add VistaCmakeCommon directory to CMakeModulePath
file( TO_CMAKE_PATH "$ENV{VISTA_CMAKE_COMMON}" _CMAKE_PATH_ENV )
if( EXISTS "${_CMAKE_PATH_ENV}" )
	list( APPEND CMAKE_MODULE_PATH "${_CMAKE_PATH_ENV}" )
elseif( EXISTS "${CMAKE_SOURCE_DIR}/VistaCoreLibs/VistaCMakeCommon" )
	list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/VistaCoreLib/VistaCMakeCommon" )
elseif( EXISTS "${CMAKE_SOURCE_DIR}/VistaCMakeCommon" )
	list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/VistaCMakeCommon" )
else()
	find_file( VISTACORELIBS_CMAKE_COMMON_TEST_FILE "VistaCommon.cmake" 
				PATH ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH}
				[NO_DEFAULT_PATH] )
	set( VISTACORELIBS_CMAKE_COMMON_TEST_FILE ${VISTACORELIBS_CMAKE_COMMON_TEST_FILE} CACHE INTERNAL "" FORCE )
	if( NOT VISTACORELIBS_CMAKE_COMMON_TEST_FILE )
		message( FATAL_ERROR 
			"ERROR: Folder VistaCMakeCommon not found, but is required for successful build. "
			"Please make the VistaCMakeCommon folder available using one of the following methods:\n"
			"\t - define VISTA_CMAKE_COMMON environment var pointing to the folder\n"
			"\t - add folder to environment variables CMAKE_MODULE_PATH and CMAKE_PREFIX_PATH\n"
			"\t - place VistaCMakeCommon as subdirectory of top-level ViSTA directory"
			"\t - place VistaCMakeCommon as subdirectory of VistaCoreLibs directory"			
		)
	endif()
endif()
include( VistaCommon )

set( VISTA_COMMON_BUILD TRUE )

add_subdirectory( VistaCoreLibs )
set( VISTADEMO_ENABLED TRUE CACHE BOOL "Enable building of VistaDemo" )
if( VISTADEMO_ENABLED )
	add_subdirectory( VistaDemo )
endif()
