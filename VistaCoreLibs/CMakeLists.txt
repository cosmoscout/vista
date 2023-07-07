if( NOT VISTA_COMMON_BUILD )
	cmake_minimum_required( VERSION 2.8 )
	project( VistaCoreLibs )
endif()

# For the CoreLibs, we verify that the VistaCMakeCommon really exist
file( TO_CMAKE_PATH "$ENV{VISTA_CMAKE_COMMON}" _CMAKE_PATH_ENV )
if( EXISTS "${_CMAKE_PATH_ENV}" )
	list( APPEND CMAKE_MODULE_PATH "${_CMAKE_PATH_ENV}" )
elseif( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VistaCMakeCommon" )
	list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/VistaCMakeCommon" )
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
			"\t - place VistaCMakeCommon as subdirectory of VistaCoreLibs directory"
			"\t - place VistaCMakeCommon as subdirectory of top-level ViSTA directory (if building from top-level cmake file, together with demos)"
		)
	endif()
endif()

include( VistaCommon )

# Self include
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

vista_enable_most_compiler_warnings()

set( VISTACORELIBS_VERSION_MAJOR "1" CACHE STRING "Major package version" )
set( VISTACORELIBS_VERSION_MINOR "0" CACHE STRING "Minor package version" )
set( VISTACORELIBS_VERSION_PATCH "0" CACHE STRING "Patch package version" )
# the version type can be either HEAD, RELASE, BRANCH, or TAG
vista_set_version( VistaCoreLibs HEAD HEAD )
# vista_set_version( VistaCoreLibs BRANCH IMAGES ${VISTACORELIBS_VERSION_MAJOR} ${VISTACORELIBS_VERSION_MINOR} ${VISTACORELIBS_VERSION_PATCH} )


set( VISTACORELIBS_USE_SDL2_WINDOWIMP OFF CACHE BOOL "Determines if the SDL2-based WindowToolit Implementation will be prefered" )

vista_find_package( OpenGL QUIET )
vista_find_package( OpenSG 1.8 QUIET )
vista_find_package( GLEW QUIET )

if ( VISTACORELIBS_USE_SDL2_WINDOWIMP )
	vista_find_package( SDL2 QUIET )
	vista_find_package( SDL2_ttf QUIET )
endif()

set( VISTACORELIBS_USE_GLUT_WINDOWIMP ON CACHE BOOL "Determines if the GLUT-based WindowToolit Implementation will be prefered" )

if ( VISTACORELIBS_USE_GLUT_WINDOWIMP )
	vista_find_package( GLUT QUIET )
endif()

set( BUILD_KERNEL_DEFAULT OFF )

if( FIRST_CONFIGURE_RUN )
	if( NOT OPENGL_FOUND )
		message( "OpenGL not found - VistaKernel will be disabled by default" )
	elseif( NOT GLUT_FOUND AND (NOT SDL2_FOUND OR NOT SDL2_TTF_FOUND))
		message( "Glut and SDL2 not found - VistaKernel will be disabled by default" )
	elseif( NOT OPENSG_FOUND )
		message( "OpenSG not found - VistaKernel will be disabled by default" )
	else()
		set( BUILD_KERNEL_DEFAULT ON )
	endif( NOT OPENGL_FOUND )
	
endif( FIRST_CONFIGURE_RUN )

set( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY OFF CACHE BOOL "If set to true, a <Package>Config.cmake file will be created and installed for each library, not just one for the CoreLibs" )

set( VISTACORELIBS_BUILD_WINDOWIMP_GLUT ${GLUT_FOUND} CACHE BOOL "Determines if the GLUT-based WindowToolit Implementation will be built" )
set( VISTACORELIBS_BUILD_WINDOWIMP_SDL2 ${SDL2_FOUND} CACHE BOOL "Determines if the SDL2-based WindowToolit Implementation will be built" )
set( VISTACORELIBS_BUILD_WINDOWIMP_OSG FALSE CACHE BOOL "Determines if the OpenSceneGraph-based WindowToolit Implementation will be built" )


set( 64BIT_ATOMICS_FLAG FALSE )
if( WIN32 )
	# For now, we always disable by default to ensure WinXP compability
	# if( "${CMAKE_SYSTEM_VERSION}" VERSION_LESS 5.2 )
		## WIN XP and below don't support InterlockedExchange64
		# set( 64BIT_ATOMICS_FLAG FALSE )
	# endif()
	# set( _ATOMCS64_DOC_TEXT "Toggle between using the Win32-native InterlockedExchange64, or a Semaphore lock when using 64-bit-atomics. Windows XP and below don't support InterlockedExchange64, so when compiling on or for such a machine, disable this flag." )
elseif( UNIX )	
	if( NOT VISTA_64BIT )
		set( 64BIT_ATOMICS_FLAG FALSE )		
	endif()
	set( _ATOMCS64_DOC_TEXT "Toggle between using native support or a Semaphore lock when using 64-bit-atomics. Gcc under 32bit linux, as well as 64bit-linux while compiling 32bit-binaries (with -m32) don't support native 64bit atomics." )
endif()
set( VISTACORELIBS_SUPPORT_NATIVE_64BIT_ATOMICS ${64BIT_ATOMICS_FLAG} CACHE BOOL "${_ATOMCS64_DOC_TEXT}" )
if( NOT VISTACORELIBS_SUPPORT_NATIVE_64BIT_ATOMICS )
	add_definitions( -DVISTA_NATIVE64BITATOMICS_NOT_AVAILABLE )
endif( NOT VISTACORELIBS_SUPPORT_NATIVE_64BIT_ATOMICS )

# Enable C++11 for all sub-modules on *NIX systems
# if( UNIX )
	# unix_require_c11()
# endif()

if (${CMAKE_VERSION} GREATER_EQUAL 3.16)
	option(VISTA_USE_PRECOMPILED_HEADERS "Turn on generation of precompiled headers" OFF)
	set(CMAKE_UNITY_BUILD_BATCH_SIZE 20)
	if (WIN32)
		add_definitions(
				/bigobj
		)
	endif()
endif()

add_definitions(
	-DNOMINMAX
)

# Standard package config
vista_configure_cpack( VistaCoreLibs )

# Let the user decide which libraries to build
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_BASE 				VistaBase				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_ASPECTS				VistaAspects			ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_MATH 				VistaMath				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_INTERPROCCOMM		VistaInterProcComm		ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_TOOLS				VistaTools				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_DEVICEDRIVERSBASE	VistaDeviceDriversBase	ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_DATAFLOWNET			VistaDataFlowNet		ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_KERNEL				VistaKernel				${BUILD_KERNEL_DEFAULT}  ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_KERNELOPENSGEXT		VistaKernelOpenSGExt	${BUILD_KERNEL_DEFAULT}  ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_OGLEXT				VistaOGLExt				${BUILD_KERNEL_DEFAULT}  ADD_TO_LIST VISTACORELIBS_COMPONENTS )

add_subdirectory( VistaDeviceDrivers )

set( VISTACORELIBS_BUILD_TESTS OFF CACHE BOOL "If set to true, tests to verify the correctness of some of the CoreLib's functions will be performed" )
if( VISTACORELIBS_BUILD_TESTS )
	enable_testing()
	add_subdirectory( Tests )
endif()


set( VISTACORELIBS_LIBRARY_OUTDIR "${CMAKE_BINARY_DIR}/lib" "${CMAKE_BINARY_DIR}/lib/DriverPlugins" )
set( VISTACORELIBS_LIBRARY_INSTALLDIR "${CMAKE_INSTALL_PREFIX}/lib" "${CMAKE_INSTALL_PREFIX}/lib/DriverPlugins" )
set( VISTACORELIBS_CONFIG_DEFINITIONS "${VISTA_PLATFORM_DEFINE}" )

# Conflict with archive Packagers
if( NOT ${VISTA_BUILD_PACKS} )
	vista_create_cmake_configs( VistaCoreLibs 
							"${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/VistaCoreLibsConfig-build.cmake_proto"
							"${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/VistaCoreLibsConfig-install.cmake_proto" )
	vista_create_uninstall_target()
else()
	# VistaDrivers in lib-subdir
	# install is not known to CPack, everything has to be prepared here.
	if( ${VISTA_CPACK_GENERATOR} STREQUAL "RPM" AND UNIX )
		if( VISTA_64BIT )
			file( WRITE "${CMAKE_CURRENT_SOURCE_DIR}/vistadrivers.conf" "${CMAKE_INSTALL_PREFIX}/lib64/DriverPlugins" )
		else()
			file( WRITE "${CMAKE_CURRENT_SOURCE_DIR}/vistadrivers.conf" "${CMAKE_INSTALL_PREFIX}/lib/DriverPlugins" )
		endif()
		install( FILES "${CMAKE_CURRENT_SOURCE_DIR}/vistadrivers.conf"
			 DESTINATION "/etc/ld.so.conf.d"
			 PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC} )
	endif()
endif()

vista_create_doxygen_target( "${CMAKE_CURRENT_SOURCE_DIR}/Documentation/Doxyfile" )

# install license files
install( FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.LESSER" "README" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/VistaCoreLibs" )
