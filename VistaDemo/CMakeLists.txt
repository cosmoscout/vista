if( NOT VISTA_COMMON_BUILD )
	cmake_minimum_required( VERSION 2.8 )
	project( VistaDemo )
endif()

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
file( TO_CMAKE_PATH "$ENV{VISTA_CMAKE_COMMON}" _CMAKE_PATH_ENV )

if( EXISTS "${_CMAKE_PATH_ENV}" )
	list( APPEND CMAKE_MODULE_PATH "${_CMAKE_PATH_ENV}" )
elseif( EXISTS "${VistaCoreLibs_SOURCE_DIR}/VistaCMakeCommon" )
	list( APPEND CMAKE_MODULE_PATH "${VistaCoreLibs_SOURCE_DIR}/VistaCMakeCommon" )
elseif( EXISTS "${CMAKE_SOURCE_DIR}/VistaCMakeCommon" )
	list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/VistaCMakeCommon" )
else()
	find_file( VISTACORELIBS_CMAKE_COMMON_TEST_FILE "VistaCommon.cmake" 
				PATH ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH}
				[NO_DEFAULT_PATH] )
	set( VISTACORELIBS_CMAKE_COMMON_TEST_FILE ${VISTACORELIBS_CMAKE_COMMON_TEST_FILE} CACHE INTERNAL "" FORCE )
	if( NOT VISTACORELIBS_CMAKE_COMMON_TEST_FILE )
		message( FATAL_ERROR 
			"ERROR: Folder VistaCMakeCommon not found, but is required for successfull build. "
			"Please make the VistaCMakeCommon folder available using one ofthe following methods:\n"
			"\t- define VISTA_CMAKE_COMMON environment var pointing to the folder\n"
			"\t- add folder to environment variables CMAKE_MODULE_PATH and CMAKE_PREFIX_PATH\n"
			"\t- place VistaCMakeCommon as subdirectory of VistaCoreLibs directory"
			"\t- place VistaCMakeCommon as subdirectory of top-level Vista ddirectory (if building from top-level cmake file, together with demos)"
		)
	endif()
endif()
include( VistaCommon )


vista_set_version( VistaDemo HEAD HEAD 1 0 0 )

if( VISTA_COMMON_BUILD )
	if( NOT VISTACORELIBS_BUILD_KERNEL )
		message( FATAL_ERROR "VistaDemo cannot be built - VistaCorelibs component VistaKernel is not activated" )
	endif()
	vista_use_package( VistaCoreLibs REQUIRED VistaKernel PATHS "${CMAKE_BINARY_DIR}/cmake" NO_DEFAULT_PATH FIND_DEPENDENCIES )
endif()
set( VISTADEMO_COMMON_BUILD TRUE )

# Standard package config
vista_configure_cpack( VistaDemo )

vista_conditional_add_subdirectory( VISTADEMO_BUILD_01KEYBOARDDEMO 01KeyboardDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_02GEOMETRYDEMO 02GeometryDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_03TEXTDEMO 03TextDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_04LOADDEMO 04LoadDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_05INTERACTIONDEMO 05InteractionDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_06CAMERACONTROLDEMO 06CameraControlDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_07OVERLAYDEMO 07OverlayDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_09EVENTDEMO 09EventDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_10DISPLAYDEMO 10DisplayDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_11OGLDEMO 11OGLDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_12INTENTIONSELECTDEMO 12IntentionSelectDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_13KERNELOPENSGEXTDEMO 13KernelOpenSGExtDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_14DATAFLOWNETDEMO 14DataFlowNetDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_15VTKDEMO 15VtkDemo OFF )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_16PHANTOMDEMO 16PhantomDemo OFF )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_17MSGPORTDEMO 17MsgPortDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_18DEBUGGINGTOOLSDEMO 18DebuggingToolsDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_19TIMEDEMO 19TimeDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_20PYTHONDFNDEMO 20PythonDFNDemo OFF )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_21CLUSTERMODEDEMO 21ClusterModeDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_22RECORDREPLAYDEMO 22RecordReplayDemo ON )

# additional package configuration
if( ${VISTA_BUILD_PACKS} )

	if( ${VISTA_CPACK_GENERATOR} STREQUAL "RPM" AND UNIX )
		if( VISTA_64BIT )
			set( VISTACORELIBS_DRIVER_PLUGIN_DIRS "/usr/lib64/DriverPlugins" )
		else()
			set( VISTACORELIBS_DRIVER_PLUGIN_DIRS "/usr/lib/DriverPlugins" )
		endif()

		file( GLOB _INTERACTION_INIS "${CMAKE_CURRENT_SOURCE_DIR}/configfiles/interaction*.ini" )
		foreach( _FILE ${_INTERACTION_INIS} )
			configure_file( ${_FILE} ${_FILE} )
		endforeach()

		install( DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/configfiles/"
			 DESTINATION "/etc/${CPACK_PACKAGE_NAME}-configfiles"
			 FILE_PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC}
			 PATTERN ".svn" EXCLUDE )
	
	else()
		install( DIRECTORY "${CMAKE_SOURCE_DIR}/configfiles"
			DESTINATION "${CMAKE_INSTALL_PREFIX}"
			FILE_PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC}
			PATTERN ".svn" EXCLUDE )

	endif()
	
	set( CPACK_TARGET_TYPE "APP" )
	
endif()

