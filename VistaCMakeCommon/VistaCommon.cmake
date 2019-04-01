

# This file contains common settings and macros for setting up Vista projects

# PACKAGE MACROS:
# vista_add_external_msvc_project_of_package( PACKAGE_NAME [SOLUTION_FOLDER] [DEPENDENT (DEPENDENT_TARGET)+ ] [ DEPENDS (DEPENDENT_TRAGET)+ ]  )
# vista_find_package( <package> [version] [EXACT] [QUIET] [[REQUIRED|COMPONENTS] [components...]] [NO_POLICY_SCOPE] [NO_MODULE] )
# vista_use_package( <package> [version] [EXACT] [QUIET] [[REQUIRED|COMPONENTS] [components...]] [NO_POLICY_SCOPE] [NO_MODULE] [FIND_DEPENDENCIES] [SYSTEM_HEADERS] )
# vista_find_shader_dirs( PACKAGE_NAME )
# vista_configure_app( PACKAGE_NAME [OUT_NAME] )
# vista_configure_lib( PACKAGE_NAME [OUT_NAME] )
# vista_install( TARGET [INCLUDE/BIN_SUBDIRECTORY [LIBRARY_SUBDIRECTORY] ] [NO_POSTFIX] )
# vista_install_files_by_extension( SEARCH_ROOT INSTALL_SUBDIR EXTENSION1 [EXTENSION2 ...] )
# vista_install_all_dlls( INSTALL_SUBDIR )
# vista_add_target_pathscript_dynamic_lib_path( _PACKAGE_NAME _VALUE [PATH_LIST] )
# vista_add_pathscript_dynamic_lib_path( _VALUE [PATH_LIST] )
# vista_add_target_pathscript_envvar( _PACKAGE_NAME _ENVVAR _VALUE [PATH_LIST] )
# vista_add_pathscript_envvar( _ENVVAR _VALUE [PATH_LIST] )
# vista_set_target_msvc_arguments( _PACKAGE_NAME _COMMANDLINE_VARS )
# vista_create_cmake_config_build( PACKAGE_NAME CONFIG_PROTO_FILE TARGET_DIR )
# vista_create_cmake_config_install( PACKAGE_NAME CONFIG_PROTO_FILE TARGET_DIR )
# vista_create_version_config( PACKAGE_NAME VERSION_PROTO_FILE )
# vista_create_cmake_configs( TARGET [CUSTOM_CONFIG_FILE_BUILD [CUSTOM_CONFIG_FILE_INSTALL] ] )
# vista_set_outdir( TARGET DIRECTORY [USE_CONFIG_SUBDIRS])
# vista_set_version( PACKAGE TYPE NAME [ MAJOR [ MINOR [ PATCH [ TWEAK ]]]] )
# vista_adopt_version( PACKAGE ADOPT_PARENT )
# vista_create_info_file( PACKAGE_NAME TARGET_DIR INSTALL_DIR )  included from VistaBuildInfo.cmake
# vista_delete_info_file( PACKAGE_NAME TARGET_DIR )              included from VistaBuildInfo.cmake
# vista_create_default_info_file( PACKAGE_NAME )
# vista_create_doxygen_target( DOXYFILE [WORKING_DIR] )
# vista_create_uninstall_target( [ON|OFF] )
# vista_set_install_permissions( OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE )

# UTILITY MACROS:
# require_vistacommon_version( SVN_REVISION )
# vista_set_defaultvalue( <cmake set syntax> )
# vista_add_files_to_sources( TARGET_LIST ROOT_DIR [SOURCE_GROUP group_name] EXTENSION1 [EXTENSION2 ...] )
# vista_conditional_add_subdirectory( VARIABLE_NAME DIRECTORY [ON|OFF] [ADVANCED [MSG string] )
# replace_svn_revision_tag( STRING )
# vista_enable_all_compiler_warnings()
# vista_enable_most_compiler_warnings()
# unix_require_cpp11()
# unix_require_cpp14()

# GENERAL SETTINGS
# adds info variables
#	VISTACMAKECOMMON_REVISION - current svn revision of the VistaCMakeCommon
#	FIRST_CONFIGURATION_RUN - true if this is the first configuration run
#   VISTA_HWARCH    - variable describing Hardware architecture, e.g. win32.vc9 or LINUX.X86
#   VISTA_COMPATIBLE_HWARCH - architectures that are compatible to the current HWARCH,
#                        e.g. for win32.vc9 this will be "win32.vc9 win32"
#   VISTA_64BIT     - set to true if the code is compiled for 64bit execution
#   VISTA_PLATFORM_DEFINE - compiler definition for the platform ( -DWIN32 or -DLINUX or -DDARWIN )
# adds some general flags/configurations
#	sets CMAKE_DEBUG_POSTFIX to "D"
#	enables global cmake property USE_FOLDERS - allows grouping of projects in msvc
#	conditionally adds DEBUG and OS definitions
#	some visual studio flags
#	VISTA_USE_RPATH cache flag to enable/disable use of RPATH
#	scans XYZConfig.cmake files in VISTA_CMAKE_COMMON/share, and deletes outdated ones


# we clean the CMAKE_MODULE_PATH - just in case there are some \ pathes in there
# this, we also do on multiple inclusions
set( _TMP_MODULE_PATH ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )
foreach( _PATH ${_TMP_MODULE_PATH} )
	file( TO_CMAKE_PATH ${_PATH} _CHANGED_PATH )
	list( APPEND CMAKE_MODULE_PATH ${_CHANGED_PATH} )
endforeach( _PATH )
list( REMOVE_DUPLICATES CMAKE_MODULE_PATH )
	
# avoid multiple includes of this file (for performance reasons )
if( NOT VISTA_COMMON_INCLUDED )

set( VISTA_COMMON_INCLUDED TRUE )

set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

if( CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12 )
	cmake_policy( SET CMP0022 OLD )
endif()

#this package sets the variables VISTA_HWARCH, VISTA_COMPATIBLE_HWARCH and VISTA_64BIT
include( VistaHWArchSettings )
include( VistaFindUtils )
include( VistaBuildInfo )

###########################
###   Utility macros    ###
###########################

# require_vistacommon_version( SVN_REVISION )
# macro to verify that the VistaCMakeCommon's svn revision is high enough (e.g. to ensure that bugfixes are
# used. If the VistaCMakeCommon's svn revisionis less than the provided number, a warning it emitted
macro( require_vistacommon_version _SVN_REVISION )
	if( ${VISTACMAKECOMMON_REVISION} LESS ${_SVN_REVISION} )
		message( WARNING "VistaCMakeCommon is requested to have at least revision ${_SVN_REVISION}, but is only "
						"Revision ${VISTACMAKECOMMON_REVISION} - please update your VistaCMakeCommon!" )
	endif( ${VISTACMAKECOMMON_REVISION} LESS ${_SVN_REVISION} )
endmacro( require_vistacommon_version )

# vista_set_defaultvalue( <cmake set() syntax> )
# macro for overriding default values of pre-initialized variables
# sets the variable only once
macro( vista_set_defaultvalue _VAR_NAME )
	if( NOT VISTA_${_VAR_NAME}_ALREADY_INITIALIZED )
		set( _ARGS )
		list( APPEND _ARGS ${ARGV} )
		list( FIND _ARGS FORCE _FORCE_FOUND )
		if( ${_FORCE_FOUND} EQUAL -1 )
			set( ${_ARGS} FORCE )
		else( ${_FORCE_FOUND} EQUAL -1 )
			set( ${_ARGS} )
		endif( ${_FORCE_FOUND} EQUAL -1 )
		set( VISTA_${_VAR_NAME}_ALREADY_INITIALIZED TRUE CACHE INTERNAL "" FORCE )
	endif()
endmacro( vista_set_defaultvalue )

# vista_add_files_to_sources( TARGET_LIST ROOT_DIR [NON_RECURSIVE] [SOURCE_GROUP group_name | SOURCE_GROUP_MIRROR_DIR prefix] EXTENSION1 [EXTENSION2 ...] )
# searches files with any of the passed extensions in the specified root_dir. These files are added to the
# passed list. If the source_group option is given, the files are also added to the specified source group.
# The root folder will be searched recursively unless the NON_RECURSIVE option is given
# IMPORTANT NOTE: due to cmake's string replacement hicka-di-hoo, if you want to use subfolders in your sourcegroups,
# you'll have to use 4(!) backslashes as separator (e.g. "folder\\\\subfolder")
macro( vista_add_files_to_sources _TARGET_LIST _SEARCH_ROOT )
	set( _RECURSIVE TRUE )	
	set( _EXTENSIONS ${ARGN} )
	set( _SOURCE_GROUP )
	set( _SOURCE_GROUP_MIRROR_PATH FALSE )
	
	if( "${ARGV2}" STREQUAL "NON_RECURSIVE" )
		set( _RECURSIVE FALSE )
		if( ${ARGV3} STREQUAL "SOURCE_GROUP" )
			set( _SOURCE_GROUP ${ARGV4} )
			list( REMOVE_AT _EXTENSIONS 0 1 2 )
		else()
			list( REMOVE_AT _EXTENSIONS 0 )
		endif()
		
	elseif( ${ARGV2} STREQUAL "SOURCE_GROUP" )
		set( _SOURCE_GROUP ${ARGV3} )
		if( ${ARGV4} STREQUAL "NON_RECURSIVE" )
			set( _RECURSIVE FALSE )
			list( REMOVE_AT _EXTENSIONS 0 1 2 )
		else()
			list( REMOVE_AT _EXTENSIONS 0 1 )
		endif()
		
	elseif( ${ARGV2} STREQUAL "SOURCE_GROUP_MIRROR_DIR" )
		set( _SOURCE_GROUP ${ARGV3} )
		set( _SOURCE_GROUP_MIRROR_PATH TRUE )
		if( ${ARGV4} STREQUAL "NON_RECURSIVE" )
			set( _RECURSIVE FALSE )
			list( REMOVE_AT _EXTENSIONS 0 1 2 )
		else()
			list( REMOVE_AT _EXTENSIONS 0 1 )
		endif()
		
	endif()
	
	set( _FOUND_FILES )
	foreach( _EXT ${_EXTENSIONS} )
		if( _RECURSIVE )
			file( GLOB_RECURSE _FOUND_FILES "${_SEARCH_ROOT}/*.${_EXT}" "${_SEARCH_ROOT}/**/*.${_EXT}" )
		else()
			file( GLOB _FOUND_FILES "${_SEARCH_ROOT}/*.${_EXT}" )
		endif()
		list( APPEND ${_TARGET_LIST} ${_FOUND_FILES} )
		if( _SOURCE_GROUP_MIRROR_PATH )
			get_filename_component( _ABSOLUTE_SEARCH_ROOT "${_SEARCH_ROOT}" ABSOLUTE )
			foreach( _FILE ${_FOUND_FILES} )
				file( RELATIVE_PATH _REL_PATH "${_ABSOLUTE_SEARCH_ROOT}" "${_FILE}" )
				get_filename_component( _REL_PATH "${_REL_PATH}" PATH )
				set( _SOURCE_GROUP_NAME "${_SOURCE_GROUP}/${_REL_PATH}" )
				string( REPLACE "/" "\\" _SOURCE_GROUP_NAME "${_SOURCE_GROUP_NAME}" )
				source_group( "${_SOURCE_GROUP_NAME}" FILES "${_FILE}" )
			endforeach()
		elseif( _SOURCE_GROUP )
			source_group( ${_SOURCE_GROUP} FILES ${_FOUND_FILES} )
		endif()
	endforeach( _EXT ${_EXTENSIONS} )
endmacro( vista_add_files_to_sources )


# vista_conditional_add_subdirectory( VARIABLE_NAME DIRECTORY [ON|OFF] [ADVANCED [MSG string] )
# creates a cache bool variable with the specified name and cache message, initialized to the desired
# value (defaults to ON ). ADVANCED marks the cache variable as advanced. Nothing is done if the specified
# directory does not exist
macro( vista_conditional_add_subdirectory )
	if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV1}" )
		set( _VISTA_CONDITIONAL_SET_STATE ON )
		set( _VISTA_CONDITIONAL_SET_ADVANCED FALSE )
		set( _VISTA_CONDITIONAL_SET_MSG "Build the ${ARGV1} library" )
		set( _VISTA_CONDITIONAL_SET_MSG_NEXT FALSE )
		set( _VISTA_CONDITIONAL_ADD_TO_LIST_NEXT FALSE )
		set( _APPEND_TO_LIST "" )

		foreach( _ARG ${ARGV} )
			if( _VISTA_CONDITIONAL_SET_MSG_NEXT )
				set( _VISTA_CONDITIONAL_SET_MSG ${_ARG} ${ARGV0} )
				set( _VISTA_CONDITIONAL_SET_MSG_NEXT FALSE )
			elseif( _VISTA_CONDITIONAL_ADD_TO_LIST_NEXT )
				set( _VISTA_CONDITIONAL_ADD_TO_LIST_NEXT FALSE )
				set( _APPEND_TO_LIST ${_ARG} )
			elseif( ${_ARG} STREQUAL "ON" )
				set( _VISTA_CONDITIONAL_SET_STATE ON )
			elseif( ${_ARG} STREQUAL "OFF" )
				set( _VISTA_CONDITIONAL_SET_STATE OFF )
			elseif( ${_ARG} STREQUAL "ADVANCED" )
				set( _VISTA_CONDITIONAL_SET_ADVANCED TRUE )
			elseif( ${_ARG} STREQUAL "MSG" )
				set( _VISTA_CONDITIONAL_SET_MSG_NEXT TRUE )
			elseif( ${_ARG} STREQUAL "ADD_TO_LIST" )
				set( _VISTA_CONDITIONAL_ADD_TO_LIST_NEXT TRUE )
			endif( _VISTA_CONDITIONAL_SET_MSG_NEXT )
		endforeach( _ARG ${ARGV} )

		set( ${ARGV0} ${_VISTA_CONDITIONAL_SET_STATE} CACHE BOOL "${_VISTA_CONDITIONAL_SET_MSG}" )
		if( _VISTA_CONDITIONAL_SET_ADVANCED )
			mark_as_advanced( ${ARGV0} )
		endif( _VISTA_CONDITIONAL_SET_ADVANCED )

		if( ${ARGV0} )
			add_subdirectory( "${ARGV1}" )
			if( _APPEND_TO_LIST )
				list( APPEND ${_APPEND_TO_LIST} ${ARGV1} )
			endif( _APPEND_TO_LIST )
		endif( ${ARGV0} )
	endif( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV1}" )
endmacro( vista_conditional_add_subdirectory )



# local macro, for use in this file only
function( local_clean_old_config_references _PACKAGE_NAME _PACKAGE_TARGET_FILE _EXCLUDE_DIR )
	string( TOUPPER _PACKAGE_NAME_UPPER ${_PACKAGE_NAME} )
		
	# we only want to do this if the version changes! (and only if a version exists at all
	if( NOT "${VISTA_${_PACKAGE_NAME_UPPER}_INTERNAL_LAST_CHECK_VERSION}" STREQUAL "${${_PACKAGE_NAME_UPPER}_VERSION_EXT}" )
		set( VISTA_${_PACKAGE_NAME_UPPER}_INTERNAL_LAST_CHECK_VERSION "${${_PACKAGE_NAME_UPPER}_VERSION_EXT}" CACHE INTERNAL "" FORCE )
		
		set( PACKAGE_REFERENCE_EXISTS_TEST TRUE )

		set( _OWN_FILE "${_EXCLUDE_DIR}/${_PACKAGE_NAME}Config.cmake" )
		file( GLOB_RECURSE _ALL_VERSION_FILES "${VISTA_CMAKE_COMMON}/share/${_PACKAGE_NAME}*/${_PACKAGE_NAME}Config.cmake" )
		foreach( _FILE ${_ALL_VERSION_FILES} )
			file( TO_CMAKE_PATH "${_FILE}" _FILE )
			if( NOT _FILE STREQUAL _OWN_FILE )
				set( PACKAGE_REFERENCE_OUTDATED FALSE )
				include( "${_FILE}" )				
				if( PACKAGE_REFERENCE_OUTDATED OR "${_PACKAGE_TARGET_FILE}" STREQUAL "${${_PACKAGE_NAME_UPPER}_REFERENCED_FILE}" )
					string( REGEX MATCH "(${VISTA_CMAKE_COMMON}/share/.+)/.*" _MATCHED ${_FILE} )
					if( _MATCHED )
						set( _DIR "${CMAKE_MATCH_1}" )
						message( STATUS "Removing old config reference copied to \"${_DIR}\"" )
						file( REMOVE_RECURSE "${_DIR}" )
					endif( _MATCHED )
				endif( PACKAGE_REFERENCE_OUTDATED OR "${_PACKAGE_TARGET_FILE}" STREQUAL "${${_PACKAGE_NAME_UPPER}_REFERENCED_FILE}" )
			endif( NOT _FILE STREQUAL _OWN_FILE )
		endforeach( _FILE ${_ALL_VERSION_FILES} )

		set( PACKAGE_REFERENCE_EXISTS_TEST )
	endif()
endfunction()

# local macro, for use in this file only
function( local_use_existing_config_libs _NAME _ROOT_DIR _CONFIG_FILE _LIBRARY_DIR_LIST )
	get_filename_component( _ROOT_DIR "${_ROOT_DIR}" REALPATH )
	string( TOUPPER ${_NAME} _NAME_UPPER )
	if( EXISTS "${_CONFIG_FILE}" )
		include( "${_CONFIG_FILE}" )
		get_filename_component( ${_NAME_UPPER}_ROOT_DIR "${${_NAME_UPPER}_ROOT_DIR}" REALPATH )
		if( "${${_NAME_UPPER}_ROOT_DIR}" STREQUAL "${_ROOT_DIR}" )
			if( ${_NAME_UPPER}_LIBRARY_DIRS )
				list( INSERT ${_LIBRARY_DIR_LIST} 0 "${${_NAME_UPPER}_LIBRARY_DIRS}" )
				list( REMOVE_DUPLICATES ${_LIBRARY_DIR_LIST} )
			endif()
		endif()
	endif()
	set( ${_LIBRARY_DIR_LIST} ${${_LIBRARY_DIR_LIST}} PARENT_SCOPE )
endfunction()

# local macro, for use in this file only
function( local_get_existing_config_filename _NAME _ROOT_DIR _CONFIG_FILE _FILENAME )
	get_filename_component( _ROOT_DIR "${_ROOT_DIR}" REALPATH )
	string( TOUPPER ${_NAME} _NAME_UPPER )
	set( PACKAGE_REFERENCE_EXISTS_TEST 1 )
	if( EXISTS "${_CONFIG_FILE}" )
		include( "${_CONFIG_FILE}" )
		get_filename_component( ${_NAME_UPPER}_ROOT_DIR "${${_NAME_UPPER}_ROOT_DIR}" REALPATH )
		if( "${${_NAME_UPPER}_ROOT_DIR}" STREQUAL "${_ROOT_DIR}" )
			if( ${_NAME_UPPER}_LIBRARY_DIRS )
				list( INSERT ${_LIBRARY_DIR_LIST} 0 "${${_NAME_UPPER}_LIBRARY_DIRS}" )
				list( REMOVE_DUPLICATES ${_LIBRARY_DIR_LIST} )
			endif()
		endif()
	endif()
	unset( PACKAGE_REFERENCE_EXISTS_TEST )
	set( ${_FILENAME} ${${_NAME_UPPER}_REFERENCED_FILE} PARENT_SCOPE )
endfunction()

# vista_enable_all_compiler_warnings()
# Enables all compiler warnings, excluding some (subjectively less important) ones
macro( vista_enable_all_compiler_warnings )
	if( NOT "${VISTA_SHOW_ALL_WARNINGS_EXECUTED}" )
		set( VISTA_SHOW_ALL_WARNINGS_EXECUTED TRUE CACHE INTERNAL "" )
		if( MSVC )
			set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4" CACHE STRING "Flags used by the compiler during all build types." FORCE )
		elseif( UNIX )
			set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra" CACHE STRING "Flags used by the compiler during all build types." FORCE )
		endif()
	endif()
endmacro()


# vista_enable_most_compiler_warnings()
# Enables most compilerwarnings, excluding some (subjectively less important) ones
macro( vista_enable_most_compiler_warnings )
	if( NOT "${VISTA_SHOW_MOST_WARNINGS_EXECUTED}" )
		set( VISTA_SHOW_MOST_WARNINGS_EXECUTED TRUE CACHE INTERNAL "" )
		if( MSVC )
			set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4244 /wd4100 /wd4512 /wd4245 /wd4389" CACHE STRING "Flags used by the compiler during all build types." FORCE )
		elseif( UNIX )
			set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-reorder" CACHE STRING "Flags used by the compiler during all build types." FORCE )
		endif()
	endif()
endmacro()

macro( unix_require_cpp11 )
	if( UNIX )
		# check if there is already a c++ flag present
		if( NOT "${CMAKE_CXX_FLAGS}" MATCHES ".*-std=.+" )
			message( "C++11 features required - adding compiler flag -std=c++11 to CMAKE_CXX_FLAGS" )
			set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" CACHE STRING "Flags used by the compiler during all build types." FORCE )
		endif()
	endif()
endmacro()

macro( unix_require_cpp14 )
	if( UNIX )
		# check if there is already a c++ flag present
		if( NOT "${CMAKE_CXX_FLAGS}" MATCHES ".*-std=.+" )
			message( "C++14 features required - adding compiler flag -std=c++14 to CMAKE_CXX_FLAGS" )
			set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" CACHE STRING "Flags used by the compiler during all build types." FORCE )
		endif()
	endif()
endmacro()

###########################
###   Package macros    ###
###########################


# vista_add_external_msvc_project_of_package( PACKAGE_NAME [SOLUTION_FOLDER] [DEPENDENT (DEPENDENT_TARGET)+ ] [ DEPENDS (DEPENDENT_TRAGET)+ ]  [SILENT | WARNING | ERROR] )
# Adds msvc projects that were exported by a package to the solution (Visual Studio only)
# note: the targets will NOT be named by their original name, but
# instead external_NAME, to prevent name clashes (e.g. with included libraries)
# will only work if a project exists, and other wise fails silently (note: deployments, i.e. installed libs, have no projects and will hence always fail)
# Parameters:
#    - PACKAGE_NAME: name of the package (from vista_find|use_package) whose projects should become included
#	 - [SILENT | WARNING | ERROR] (optional, default is WARNING): SILENT will silently ignore all warnings and
#                            suppress information output, WARNING will emit warning but continues cmake execution,
#                            ERROR emits warning and stops cmake execution
#    - SOLUTION_FOLDER (optional): Visual Studio solution folder where the project(s) should be put
#    - DEPENDENT (targets)* (optional): list of targets that should depend on the added projects
#                            Note: for this to work, the macro has to be called AFTER defining the dependent targets
#    - DEPENDS (targets)* (optional): list of targets on which the loaded targets depend. These can be either manually added targets,
#                            or other targets loaded with vista_add_external_msvc_project_of_package.
macro( vista_add_external_msvc_project_of_package _PACKAGE_NAME )	
	if( MSVC )
		set( VISTA_ALLOW_ADDING_OF_EXTERNAL_MSVC_PROJECTS ON CACHE BOOL "If enabled, external msvc projects listed in vista_add_external_msvc_project_of_package will be added" )
		mark_as_advanced( VISTA_ALLOW_ADDING_OF_EXTERNAL_MSVC_PROJECTS )
		if( VISTA_ALLOW_ADDING_OF_EXTERNAL_MSVC_PROJECTS )
			string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
			set( _NEXT_IS_NAME FALSE )
			set( _NEXT_IS_PROJ FALSE )
			set( _NEXT_IS_DEP FALSE )
			set( _NAME "" )
			set( _POSSIBLE_DEPENDENCIES "" )
			set( _FOLDER "" )
			set( _DEPENDS_TARGETS "" )
			set( _DEPENDENT_TARGETS "" )
			set( _WARNING_LEVEL "WARNING" )
			
			set( _ARGUMENTS ${ARGV} )
			list( REMOVE_AT _ARGUMENTS 0 )


			# Extract all parameters and DEPENDS and DEPENDENT targets.
			set( _MODE -1 ) # -1 = first, 0 = nothing, 1 = Depends, 2 = Dependent
			foreach( _ARG ${_ARGUMENTS} )
				if( "${_ARG}" STREQUAL "DEPENDS" )
					set( _MODE 1 )
				elseif( "${_ARG}" STREQUAL "DEPENDENT" )
					set( _MODE 2 )
				elseif( "${_ARG}" STREQUAL "SILENT" )
					set( _WARNING_LEVEL "SILENT" )
					set( _MODE 0 )
				elseif( "${_ARG}" STREQUAL "WARNING" )
					set( _WARNING_LEVEL "WARNING" )
					set( _MODE 0 )
				elseif( "${_ARG}" STREQUAL "ERROR" )
					set( _WARNING_LEVEL "SEND_ERROR" )
					set( _MODE 0 )
				elseif( _MODE STREQUAL -1 )
					# its the first after the name -> Folder
					set( _FOLDER "${_ARG}" )
					set( _MODE 0 )
				elseif( _MODE STREQUAL 1 )
					get_property( _EXTERNALLY_ADDED_PROJECTS GLOBAL PROPERTY VISTA_EXTERNALLY_ADDED_PROJECTS )
					list( FIND _EXTERNALLY_ADDED_PROJECTS "${_ARG}" _VAL )
					if( _VAL GREATER -1 )
						list( APPEND _DEPENDS_TARGETS "external_${_ARG}" )
					elseif( TARGET)
						list( APPEND _DEPENDS_TARGETS ${_ARG} )
					else()
						message( STATUS "vista_add_external_msvc_project_of_package( ${_PACKAGE_NAME_UPPER} ) - dependency to non-existing target ${_ARG} ignored" )
					endif()			
				elseif( _MODE STREQUAL 2 )
					list( APPEND _DEPENDENT_TARGETS ${_ARG} )
				else()
					message( WARNING "vista_add_external_msvc_project_of_package: unknown parameter \"${_ARG}\"" )
				endif()
			endforeach()
			
			if( "${${_PACKAGE_NAME_UPPER}_MSVC_PROJECT}" STREQUAL "" AND NOT "${_WARNING_LEVEL}" STREQUAL "SILENT" )
				# Sorry for the mess below. But CMake requires it to write a string like this if you want to make it multi-line and formatted...
				message( "${_WARNING_LEVEL}" 
	" vista_add_external_msvc_project_of_package - Project \"${_PACKAGE_NAME}\" not found. Check the following:
	   1. Check spelling of the project name, i.e. whether \"${_PACKAGE_NAME}\" is correct.
	   2. Check if the CMakeLists.txt includes an appropriate vista_add_package line, e.g. \"vista_use_package( ${_PACKAGE_NAME} )\".
	   3. Check that you are using the affected library in its \"build\" version and not the \"installed/deployed\" package." )
			endif()
			
			
			foreach( _ENTRY ${${_PACKAGE_NAME_UPPER}_MSVC_PROJECT} )
				if( "${_ENTRY}" STREQUAL "PROJ" )
					set( _NEXT_IS_NAME TRUE )
					set( _NEXT_IS_PROJ FALSE )			
					set( _NEXT_IS_DEP FALSE )
				elseif( "${_ENTRY}" STREQUAL "DEP" )
					set( _NEXT_IS_PROJ FALSE )
					set( _NEXT_IS_NAME FALSE )
					set( _NEXT_IS_DEP TRUE )
				elseif( _NEXT_IS_NAME )
					set( _NEXT_IS_NAME FALSE )
					set( _NEXT_IS_PROJ TRUE )
					set( _NEXT_IS_DEP FALSE )
					set( _NAME "${_ENTRY}" )
				elseif( _NEXT_IS_PROJ )
					set( _NEXT_IS_PROJ FALSE )
					set( _NEXT_IS_NAME FALSE )
					set( _NEXT_IS_DEP FALSE )
					# sanity check if project exists
					if( EXISTS "${_ENTRY}" )
						get_property( _EXTERNALLY_ADDED_PROJECTS GLOBAL PROPERTY VISTA_EXTERNALLY_ADDED_PROJECTS )
						# check if the project was already added before, e.g. in another subproject
						list( FIND _EXTERNALLY_ADDED_PROJECTS "${_NAME}" _PROJ_ALREADY_ADDED )
						if( _PROJ_ALREADY_ADDED EQUAL -1 )
							if( NOT "${_WARNING_LEVEL}" STREQUAL "SILENT" )
								message( STATUS "vista_add_external_msvc_project_of_package( ${_PACKAGE_NAME_UPPER} ) - adding external project as external_${_NAME}" )
							endif()
							if( MSVC11 OR MSVC12 OR MSVC14 )
								#file( STRINGS "${_ENTRY}" _MSVCPROJ_GUID REGEX "<ProjectGUID>{[^}]+}</ProjectGUID>" )
								#string( REGEX REPLACE "<ProjectGUID>{([^}]+)}</ProjectGUID>" "\\1" _MSVCPROJ_GUID "${_MSVCPROJ_GUID}" )
								#message( "_MSVCPROJ_GUID ${_MSVCPROJ_GUID}" )
								#include_external_msproject( "external_${_NAME}" "${_ENTRY}" GUID ${_MSVCPROJ_GUID} )
								include_external_msproject( "external_${_NAME}" "${_ENTRY}" )
							else()
								include_external_msproject( "external_${_NAME}" "${_ENTRY}" )
							endif()
							set_property( GLOBAL APPEND PROPERTY VISTA_EXTERNALLY_ADDED_PROJECTS ${_NAME} )
						endif()
						list( APPEND _POSSIBLE_DEPENDENCIES ${_NAME} )
						if( _FOLDER AND CMAKE_VERSION VERSION_GREATER 2.8.4 )
							set_target_properties( "external_${_NAME}" PROPERTIES FOLDER ${_FOLDER} )
						endif()
						foreach( _DEP ${_DEPENDS_TARGETS} )
							add_dependencies( external_${_NAME} ${_DEP} )
						endforeach()
						foreach( _DEP ${_DEPENDENT_TARGETS} )
							add_dependencies( ${_DEP} external_${_NAME} )
						endforeach()
					endif()
				elseif( _NEXT_IS_DEP )
					list( FIND _POSSIBLE_DEPENDENCIES ${_ENTRY} _FOUND )
					if( _FOUND GREATER -1 )
						add_dependencies( external_${_NAME} external_${_ENTRY} )
					elseif( NOT "${_WARNING_LEVEL}" STREQUAL "SILENT" )
						message( "vista_add_external_msvc_project_of_package( ${_PACKAGE_NAME} ) - project ${_NAME} requests unknown dependency ${_ENTRY}" )
					endif()
				else()
					message( WARNING "vista_add_external_msvc_project_of_package( ${_PACKAGE_NAME_UPPER} ) - unknown parameter \"${_ENTRY}\"" )
				endif()
			endforeach()
		endif()
	endif()
endmacro( vista_add_external_msvc_project_of_package )

# vista_find_package( <package> [version] [EXACT] [QUIET] [[REQUIRED|COMPONENTS] [components...]] [NO_POLICY_SCOPE] [NO_MODULE] )
# wrapper for the cmake-native find_package with the same (basic) syntax and the following extensions:
# - allows extended versions (e.g. NAME, 1.2.4-8, etc.)
# - checks if a vista-specific FindV<package>.cmake file exists, and prefers this
# - if no module is found, the config files are searched in additional subdirectories
macro( vista_find_package _PACKAGE_NAME )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )

	# parse arguments
	set( _FIND_PACKAGE_ARGS )
	set( _FIND_DEPENDENCIES FALSE )
	set( _PACKAGE_VERSION )
	set( _PACKAGE_COMPONENTS )
	set( _QUIET FALSE )
	set( _REQUIRED FALSE )
	set( _USING_COMPONENTS FALSE )
	set( _NO_MODULE FALSE )
	set( _EXACT FALSE )
	foreach( _ARG ${ARGV} )
		if( ${_ARG} STREQUAL "FIND_DEPENDENCIES" )
			set( _PARSE_COMPONENTS FALSE )
			set( _FIND_DEPENDENCIES TRUE )
		elseif( ${_ARG} STREQUAL "QUIET" )
			set( _PARSE_COMPONENTS FALSE )
			list( APPEND _FIND_PACKAGE_ARGS "QUIET" )
			set( _QUIET TRUE )
		elseif( ${_ARG} STREQUAL "REQUIRED" )
			set( _PARSE_COMPONENTS TRUE )
			list( APPEND _FIND_PACKAGE_ARGS "REQUIRED" )
			if( _VISTAUSEPACKAGE_COMPONENTS ) # possible overwrite of components from vista_use_package
				list( APPEND _FIND_PACKAGE_ARGS ${_VISTAUSEPACKAGE_COMPONENTS} )
				set( _PACKAGE_COMPONENTS ${_VISTAUSEPACKAGE_COMPONENTS} )
				set( _USING_COMPONENTS TRUE )
			endif()
			set( _REQUIRED TRUE )
		elseif( ${_ARG} STREQUAL "COMPONENTS" )
			set( _PARSE_COMPONENTS TRUE )
			list( APPEND _FIND_PACKAGE_ARGS "COMPONENTS" )
			if( _VISTAUSEPACKAGE_COMPONENTS ) # possible overwrite of components from vista_use_package
				list( APPEND _FIND_PACKAGE_ARGS ${_VISTAUSEPACKAGE_COMPONENTS} )
				set( _PACKAGE_COMPONENTS ${_VISTAUSEPACKAGE_COMPONENTS} )
				set( _USING_COMPONENTS TRUE )
			endif()
		elseif( ${_ARG} STREQUAL "EXACT" )
			set( _EXACT TRUE )
			set( _PARSE_COMPONENTS FALSE )
			list( APPEND _FIND_PACKAGE_ARGS "EXACT" )
		elseif( ${_ARG} STREQUAL "NO_POLICY_SCOPE" )
			set( _PARSE_COMPONENTS FALSE )
			list( APPEND _FIND_PACKAGE_ARGS "NO_POLICY_SCOPE" )
		elseif( ${_ARG} STREQUAL "NO_MODULE" )
			set( _NO_MODULE TRUE )
			list( APPEND _FIND_PACKAGE_ARGS "NO_MODULE" )
		elseif( ${_ARG} STREQUAL "${ARGV0}" )
			# it's okay, just the name
		elseif( ${_ARG} STREQUAL "${ARGV1}" )
			# the requested version
			set( _PACKAGE_VERSION ${_ARG} )
		elseif( _PARSE_COMPONENTS )
			if( NOT _VISTAUSEPACKAGE_COMPONENTS ) # only re-parse components if they aren't specified by vista_use_package alreaedy
				list( APPEND _FIND_PACKAGE_ARGS ${_ARG} )
				list( APPEND _PACKAGE_COMPONENTS ${_ARG} )
				set( _USING_COMPONENTS TRUE )
			endif()
		else()
			message( WARNING "vista_find_package( ${_PACKAGE_NAME} ) - Unknown argument [${_ARG}]" )
		endif( ${_ARG} STREQUAL "FIND_DEPENDENCIES" )
	endforeach( _ARG ${ARGV} )

	set( _DO_FIND TRUE )

	if( ${_PACKAGE_NAME_UPPER}_FOUND )
		set( _DO_FIND FALSE )

		set( _PREVIOUSLY_FOUND_VERSION )
		if( ${_PACKAGE_NAME_UPPER}_VERSION_EXT )
			set( _PREVIOUSLY_FOUND_VERSION ${${_PACKAGE_NAME_UPPER}_VERSION_EXT} )
		elseif( ${_PACKAGE_NAME}_VERSION_EXT )
			set( _PREVIOUSLY_FOUND_VERSION ${${_PACKAGE_NAME}_VERSION_EXT} )
		elseif( ${_PACKAGE_NAME_UPPER}_VERSION )
			set( _PREVIOUSLY_FOUND_VERSION ${${_PACKAGE_NAME_UPPER}_VERSION_EXT} )
		elseif( ${_PACKAGE_NAME}_VERSION )
			set( _PREVIOUSLY_FOUND_VERSION ${${_PACKAGE_NAME}_VERSION} )
		elseif( ${_PACKAGE_NAME_UPPER}_VERSION_STRING )
			set( _PREVIOUSLY_FOUND_VERSION ${${_PACKAGE_NAME_UPPER}_VERSION_STRING} )
		elseif( ${_PACKAGE_NAME}_VERSION_STRING )
			set( _PREVIOUSLY_FOUND_VERSION ${${_PACKAGE_NAME}_VERSION_STRING} )
		endif( ${_PACKAGE_NAME_UPPER}_VERSION_EXT )

		if( _PREVIOUSLY_FOUND_VERSION )
			if( _PACKAGE_VERSION )
				# we have to check that we don't include different versions!
				vista_string_to_version( ${_PREVIOUSLY_FOUND_VERSION} "PREVIOUS" )
				vista_string_to_version( ${_PACKAGE_VERSION} "REQUESTED" )
				vista_compare_versions( "REQUESTED" "PREVIOUS" _DIFFERENCE )
				if( _DIFFERENCE EQUAL -1 )
					message( WARNING "vista_find_package( ${_PACKAGE_NAME} ) - Package was previously found with"
									  " version (${_PREVIOUSLY_FOUND_VERSION}), but is now requested with"
									  " incompatible version (${_PACKAGE_VERSION}) - first found version is used,"
									  " but this may lead to conflicts" )
				elseif( _DIFFERENCE VERSION_GREATER 0.0.0.0 AND _EXACT )
					message( "vista_find_package( ${_PACKAGE_NAME} ) - Package was previously found with"
									  " version (${_PREVIOUSLY_FOUND_VERSION}), but is now requested with"
									  " different, but compatible version (${_PACKAGE_VERSION}) - first found version is used" )
				#else: prefect match
				endif( _DIFFERENCE EQUAL -1 )
			endif()
			# we always want to find the sam eversiona gain, so set it to the former one
			set( _PACKAGE_VERSION ${_PREVIOUSLY_FOUND_VERSION} )
		endif()

		if( _USING_COMPONENTS )
			# we need to check if the components are already included or not			
			if( ${_PACKAGE_NAME_UPPER}_FOUND_COMPONENTS )
				foreach( _COMPONENT ${_PACKAGE_COMPONENTS} )
					list( FIND ${_PACKAGE_NAME_UPPER}_FOUND_COMPONENTS ${_COMPONENT} _COMPONENT_FOUND )
					if( _COMPONENT_FOUND LESS 0 )
						set( _DO_FIND TRUE )
						break()
					endif( _COMPONENT_FOUND LESS 0 )
				endforeach( _COMPONENT ${_PACKAGE_COMPONENTS} )
			else()
				set( _DO_FIND TRUE )
			endif()
		endif()

	endif()

	if( _DO_FIND )
		# this is somewhat of an intransparent hack: if _MESSAGE_IF_DO_FIND is set, we print a message
		# with it and reset the value. This is to allow vista_use_package to print additional info
		# - no one else should need to use thsi
		if( _MESSAGE_IF_DO_FIND )
			message( STATUS "${_MESSAGE_IF_DO_FIND}" )
			set( _MESSAGE_IF_DO_FIND )
		endif( _MESSAGE_IF_DO_FIND )

		if( _PACKAGE_VERSION )
			# check if it is a "normal" or an extended version
			string( REGEX MATCH "^[0-9\\.]*$" _MATCH ${_PACKAGE_VERSION} )
			if( NOT _MATCH )
				# its an extended version
				set( PACKAGE_FIND_VERSION_EXT ${_PACKAGE_VERSION} )
				set( ${_PACKAGE_NAME}_FIND_VERSION_EXT ${_PACKAGE_VERSION} )
				set( V${_PACKAGE_NAME}_FIND_VERSION_EXT ${_PACKAGE_VERSION} )
				set( _PACKAGE_VERSION 0.0.0.0 )
			endif( NOT _MATCH )
		endif( _PACKAGE_VERSION )

		# there can be three differnet options
		# - there is a Vista-custom FindV<PackageName>.cmake
		# - there is a generic Find<PackageName>.cmake
		# - if none of the above, we use config mode
		# however, we skip the first two steps if no module should be found
		set( _FIND_VMODULE_EXISTS FALSE )
		set( _FIND_MODULE_EXISTS FALSE )
		if( NOT _NO_MODULE )
			foreach( _PATH ${CMAKE_MODULE_PATH} ${CMAKE_ROOT} ${CMAKE_ROOT}/Modules )
				if( EXISTS "${_PATH}/FindV${_PACKAGE_NAME}.cmake" )
					set( _FIND_VMODULE_EXISTS TRUE )					
				endif( EXISTS "${_PATH}/FindV${_PACKAGE_NAME}.cmake" )
				if( EXISTS "${_PATH}/Find${_PACKAGE_NAME}.cmake" )
					set( _FIND_MODULE_EXISTS TRUE )
				endif( EXISTS "${_PATH}/Find${_PACKAGE_NAME}.cmake" )
			endforeach( _PATH ${CMAKE_MODULE_PATH} )
		endif( NOT _NO_MODULE )

		set( ${_PACKAGE_NAME_UPPER}_FOUND_COMPONENTS )

		if( _FIND_VMODULE_EXISTS )
			find_package( V${_PACKAGE_NAME} ${_PACKAGE_VERSION} ${_FIND_PACKAGE_ARGS} )
			set( ${_PACKAGE_NAME_UPPER}_FOUND ${V${_PACKAGE_NAME_UPPER}_FOUND} )
		elseif( _FIND_MODULE_EXISTS )
			find_package( ${_PACKAGE_NAME} ${_PACKAGE_VERSION} ${_FIND_PACKAGE_ARGS} )
			if( NOT ${_PACKAGE_NAME_UPPER}_FOUND AND ${_PACKAGE_NAME}_FOUND )
				set( ${_PACKAGE_NAME_UPPER}_FOUND ${${_PACKAGE_NAME}_FOUND} )
			endif()
		else()
			if( NOT ${PACKAGE_NAME_UPPER}_ADDITIONAL_CONFIG_DIRS )
				# we look for additional directories to search for the config files
				# we also search for CoreLibs directories manually
				foreach( _PATH $ENV{${_PACKAGE_NAME_UPPER}_ROOT} ${VISTA_PACKAGE_SEARCH_PATHS} )
					if( EXISTS "${_PATH}" )
						file( TO_CMAKE_PATH ${_PATH} _PATH )
						list( APPEND _SEARCH_PREFIXES
								"${_PATH}/${_PACKAGE_NAME}*/${VISTA_HWARCH}"
								"${_PATH}/${_PACKAGE_NAME}*"
								"${_PATH}/${_PACKAGE_NAME}/*/${VISTA_HWARCH}"
								"${_PATH}/${_PACKAGE_NAME}/*/"
						)
					endif( EXISTS "${_PATH}" )
				endforeach( _PATH ${_SEARCH_DIRS} )

				foreach( _PATH ${_SEARCH_PREFIXES} )
					file( GLOB _TMP_PATHES "${_PATH}" )
					list( APPEND ${PACKAGE_NAME_UPPER}_ADDITIONAL_CONFIG_DIRS ${_TMP_PATHES} )
				endforeach( _PATH ${_PREFIX_PATHES} )
				if( ${PACKAGE_NAME_UPPER}_ADDITIONAL_CONFIG_DIRS )
					list( REMOVE_DUPLICATES ${PACKAGE_NAME_UPPER}_ADDITIONAL_CONFIG_DIRS )
				endif( ${PACKAGE_NAME_UPPER}_ADDITIONAL_CONFIG_DIRS )
			endif( NOT ${PACKAGE_NAME_UPPER}_ADDITIONAL_CONFIG_DIRS )

			set( ${_PACKAGE_NAME}_ACTUAL_DIR )
			
			#if( PACKAGE_FIND_VERSION_EXT AND NOT _QUIET )
			if( NOT _QUIET )
				string( REPLACE "REQUIRED" "COMPONENTS" _CLEANED_FIND_PACKAGE_ARGS "${_FIND_PACKAGE_ARGS}" )
				find_package( ${_PACKAGE_NAME} ${_PACKAGE_VERSION} ${_CLEANED_FIND_PACKAGE_ARGS}
							PATHS ${${PACKAGE_NAME_UPPER}_ADDITIONAL_CONFIG_DIRS} ${VISTA_PACKAGE_SEARCH_PATHS} QUIET )
				if( NOT ${_PACKAGE_NAME}_FOUND )
					set( _VERSION_NAME ${_PACKAGE_VERSION} )
					if( PACKAGE_FIND_VERSION_EXT )
						set( _VERSION_NAME ${PACKAGE_FIND_VERSION_EXT} )
					endif()
					if(${_PACKAGE_NAME}_CONSIDERED_CONFIGS)
						set( _MESSAGE              "  Could not find a configuration file for package \"${_PACKAGE_NAME}\" that is" )
						set( _MESSAGE "${_MESSAGE}\n  compatible with requested version \"${_VERSION_NAME}\" (Architecture: ${VISTA_HWARCH})" )
						set( _MESSAGE "${_MESSAGE}\n   The following configuration files were considered but not accepted:" )
						list( LENGTH ${_PACKAGE_NAME}_CONSIDERED_CONFIGS _CONFIG_COUNT )
						math( EXPR _CONFIG_COUNT "${_CONFIG_COUNT} - 1")
						foreach( _CONFIG_INDEX RANGE ${_CONFIG_COUNT} )
							list( GET ${_PACKAGE_NAME}_CONSIDERED_CONFIGS ${_CONFIG_INDEX} _FILENAME )
							list( GET ${_PACKAGE_NAME}_CONSIDERED_VERSIONS ${_CONFIG_INDEX} _VERSION )
							set( _MESSAGE  "${_MESSAGE}\n     ${_FILENAME}" )
							if( NOT "${_VERSION}" STREQUAL "" AND NOT "${_VERSION}" STREQUAL "unknown" )
								set( _MESSAGE  "${_MESSAGE}\n     ${_VERSION}" )
							endif()
						endforeach()
						if( NOT _VERSION_NAME )
							set( _MESSAGE "${_MESSAGE}\n  Probably, no build with matching architecture \"${VISTA_HWARCH}\" was found" )
						endif()
					else()
						string( TOLOWER ${_PACKAGE_NAME} _PACKAGE_NAME_LOWER )
						set( _MESSAGE              "  Could not find a configuration file for package \"${_PACKAGE_NAME}\"." )
						set( _MESSAGE "${_MESSAGE}\n  Set ${_PACKAGE_NAME}_DIR to the directory containing a CMake configuration" )
						set( _MESSAGE "${_MESSAGE}\n  file for ${_PACKAGE_NAME}. The file will have one of the following names" )
						set( _MESSAGE "${_MESSAGE}\n        ${_PACKAGE_NAME}Config.cmake" )
						set( _MESSAGE "${_MESSAGE}\n        ${_PACKAGE_NAME_LOWER}-config.cmake" )

					endif()		

					if( _REQUIRED )
						message( SEND_ERROR "${_MESSAGE}\n" )
					else()
						message( WARNING "${_MESSAGE}\n" )
					endif()					
												
					
				endif()
			else()
				find_package( ${_PACKAGE_NAME} ${_PACKAGE_VERSION} ${_FIND_PACKAGE_ARGS}
							PATHS ${${PACKAGE_NAME_UPPER}_ADDITIONAL_CONFIG_DIRS} ${VISTA_PACKAGE_SEARCH_PATHS} )
			endif()
							
			# in case we dound a reference file (e.g. in VistaCMakeCommon/share), the Package_DIR would point
			# to the reference. Instead, we want it to point to the actual file directory
			if( ${_PACKAGE_NAME}_ACTUAL_DIR )
				set( ${_PACKAGE_NAME}_DIR "${${_PACKAGE_NAME}_ACTUAL_DIR}" CACHE PATH "The directory containing a CMake configuration file for $_PACKAGE_NAME}" FORCE )
			endif()
		endif()
		
		if( NOT ${_PACKAGE_NAME_UPPER}_FOUND_COMPONENTS )
			set( ${_PACKAGE_NAME_UPPER}_FOUND_COMPONENTS ${_PACKAGE_COMPONENTS} )
		endif()
		
	endif( _DO_FIND )
	
	set( PACKAGE_FIND_VERSION_EXT )
	set( _PACKAGE_VERSION 0.0.0.0 )

	set( _MESSAGE_IF_DO_FIND )
endmacro( vista_find_package )


# vista_use_package( PACKAGE [VERSION] [EXACT] [[COMPONENTS | REQUIRED] comp1 comp2 ... ] [QUIET] [FIND_DEPENDENCIES] [ONLY_RUNTIME_PATH] [NO_RECURSIVE_DEPENCENCY] [SYSTEM_HEADERS] )
# finds the desired Package and automatically sets the include dirs, library dirs, definitions for the project.
# Libraries have to be included using the VARIABLE PACKAGENAME_LIBRARIES. Alternatively, VISTA_USE_PACKAGE_LIBRARIES contains
# all libraries that have been linked by vista_use_package calls. Additionally, buildsystem-specific variables are set that
# keep track of dependencies
# Parameters
# VERSION - string describing the version - either the normal cmake-format XX.YY.ZZ.WW or the vista-specific extended version string
# EXACT specifies that the version has to be matched exactly
# REQUIRED specifies that the package must be found to continue. can optionally be followed by a list of required components
# COMPONENTS can be followed by a list of optional, desired components
# QUIET suppresses any warnings and other output except for errors
# FIND_DEPENDENCIES If set, all packages that are required by the included packages are tried to be found and used automatically
# ONLY_RUNTIME_PATH If set, no library or include paths, libs, or compile flags will be set. Instead, only the runtime path
#                     will be updated. This is useful for dependencies of libraries which only have to resolves at runtime durign dll/so-linkage.
#                     It is used by dependencies from FIND_DEPENDENCIES
#                     TODO: finish and test
# NO_RECURSIVE_DEPENCENCY specifies that this package should not count as a recursive dependency, i.e. if a library uses this package,
#                     and is then used (via vista_use_package) by another project, this package will not count as dependency for the
#                     new project. This is useful e.g. for header-only-libraries or libs that are statically linked-in
# SYSTEM_HEADERS marks all include paths of this package as system include pathes on supported compilers (e.g. gcc), which
#                     may have multiple effects, e.g. ignoring of warnings in these files
macro( vista_use_package _PACKAGE_NAME )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
		
	# check if we need to rerun. this is the case it has not been used yet,
	# or if it has been used, but now additional dependencies are requested
	set( _REQUIRES_RERUN TRUE )
	set( _VISTAUSEPACKAGE_COMPONENTS )
	if( VISTA_USE_${_PACKAGE_NAME_UPPER} )
		# extract components, to see if they are met already or not
		set( _REQUESTED_COMPONENTS )
		set( _PARSE_COMPONENTS FALSE )
		set( _COMPONENTS_FOUND FALSE )
		foreach( _ARG ${ARGV} )
			if( ${_ARG} STREQUAL "COMPONENTS" OR ${_ARG} STREQUAL "REQUIRED" )
				set( _PARSE_COMPONENTS TRUE )
			elseif( ${_ARG} STREQUAL "QUIET"
					OR ${_ARG} STREQUAL "EXACT"
					OR ${_ARG} STREQUAL "NO_POLICY_SCOPE"
					OR ${_ARG} STREQUAL "FIND_DEPENDENCIES"	
					OR ${_ARG} STREQUAL "NO_RECURSIVE_DEPENCENCY"
					OR ${_ARG} STREQUAL "NO_RMODULE"
					OR ${_ARG} STREQUAL "SYSTEM_HEADERS"	)
				set( _PARSE_COMPONENTS FALSE )
			elseif( _PARSE_COMPONENTS )
				list( APPEND _REQUESTED_COMPONENTS ${_ARG} )
				set( _COMPONENTS_FOUND TRUE )
			endif( ${_ARG} STREQUAL "COMPONENTS" OR ${_ARG} STREQUAL "REQUIRED" )
		endforeach( _ARG ${ARGV} )
		
		set( _REQUIRES_RERUN FALSE )
		if( _COMPONENTS_FOUND )
			# we need to check if any additional components are required
			foreach( _COMPONENT ${_REQUESTED_COMPONENTS} )
				list( FIND ${_PACKAGE_NAME_UPPER}_FOUND_COMPONENTS ${_COMPONENT} _FOUND )
				if( _FOUND EQUAL -1 )
					# we need to find additional components - merge the requested and the already found ones
					# so that the new search finds both the newly requested components and those from the last run
					set( _VISTAUSEPACKAGE_COMPONENTS ${_REQUESTED_COMPONENTS} ${${_PACKAGE_NAME_UPPER}_FOUND_COMPONENTS} )
					list( REMOVE_DUPLICATES _VISTAUSEPACKAGE_COMPONENTS )
					set( _REQUIRES_RERUN TRUE )
					break()
				endif()
			endforeach()
		endif()
		# todo: check version

	endif()

	if( _REQUIRES_RERUN )
		# we first extract some parameters, then try to find the package

		set( _ARGUMENTS ${ARGV} )

		# parse arguments
		list( FIND _ARGUMENTS "FIND_DEPENDENCIES" _FIND_DEPENDENCIES_FOUND )
		if( _FIND_DEPENDENCIES_FOUND GREATER -1 )
			set( _FIND_DEPENDENCIES TRUE )
		else()
			set( _FIND_DEPENDENCIES FALSE )
		endif()
		
		
		list( FIND _ARGUMENTS "NO_RECURSIVE_DEPENCENCY" _NO_RECURSIVE_DEPENCENCY_FOUND )
		if( _NO_RECURSIVE_DEPENCENCY_FOUND GREATER -1 )
			set( _NO_RECURSIVE_DEPENCENCY TRUE )
			list( REMOVE_ITEM _ARGUMENTS "NO_RECURSIVE_DEPENCENCY" )
		else()
			set( _NO_RECURSIVE_DEPENCENCY FALSE )
		endif()

		list( FIND _ARGUMENTS "QUIET" _QUIET_FOUND )
		if( _QUIET_FOUND GREATER -1 )
			set( _QUIET TRUE )
		else()
			set( _QUIET FALSE )
		endif()
		
		list( FIND _ARGUMENTS "SYSTEM_HEADERS" _SYSTEM_HEADERS_FOUND )
		if( _SYSTEM_HEADERS_FOUND GREATER -1 )
			set( _PACKAGE_INCLUDES_ARE_SYSTEM_HEADERS TRUE )
			list( REMOVE_ITEM _ARGUMENTS SYSTEM_HEADERS )
		else()
			set( _PACKAGE_INCLUDES_ARE_SYSTEM_HEADERS FALSE )
		endif()

		# finding will handle differences to already run find's
		vista_find_package( ${_ARGUMENTS} )
		set( _VISTAUSEPACKAGE_COMPONENTS )

		# set required variables if package was found AND it wasn't sufficiently included before (in which case _DO_FIND is FALSE )
		if( ${_PACKAGE_NAME_UPPER}_FOUND AND ( _DO_FIND OR NOT VISTA_USE_${_PACKAGE_NAME_UPPER} ) )
			# check if HWARCH matches
			if( ${_PACKAGE_NAME_UPPER}_HWARCH AND NOT ${${_PACKAGE_NAME_UPPER}_HWARCH} STREQUAL ${VISTA_HWARCH} )
				message( WARNING "vista_use_package( ${_PACKAGE_NAME} ) - Package was built as ${${_PACKAGE_NAME_UPPER}_HWARCH}, but is used with ${VISTA_HWARCH}" )
			endif( ${_PACKAGE_NAME_UPPER}_HWARCH AND NOT ${${_PACKAGE_NAME_UPPER}_HWARCH} STREQUAL ${VISTA_HWARCH} )
			
			# if a USE_FILE is specified, we assume that it handles all the settings
			# if not, we set the necessary values ourselves
			if( ${_PACKAGE_NAME_UPPER}_USE_FILE )
				include( "${${_PACKAGE_NAME_UPPER}_USE_FILE}" )
			else()
				if( _PACKAGE_INCLUDES_ARE_SYSTEM_HEADERS )
					include_directories( SYSTEM ${${_PACKAGE_NAME_UPPER}_INCLUDE_DIRS} )
				else()
					include_directories( ${${_PACKAGE_NAME_UPPER}_INCLUDE_DIRS} )
				endif()
				link_directories( ${${_PACKAGE_NAME_UPPER}_LIBRARY_DIRS} )
				add_definitions( ${${_PACKAGE_NAME_UPPER}_DEFINITIONS} )
			endif( ${_PACKAGE_NAME_UPPER}_USE_FILE )
			
			
			list( APPEND VISTA_TARGET_LINK_DIRS ${${_PACKAGE_NAME_UPPER}_LIBRARY_DIRS} )
			if( VISTA_TARGET_LINK_DIRS )
				list( REMOVE_DUPLICATES VISTA_TARGET_LINK_DIRS )
			endif( VISTA_TARGET_LINK_DIRS )
			set( VISTA_USING_${_PACKAGE_NAME_UPPER} TRUE )
			
			if( NOT _NO_RECURSIVE_DEPENCENCY )
				list( APPEND VISTA_TARGET_FULL_DEPENDENCIES ${_PACKAGE_NAME} )
				list( REMOVE_DUPLICATES VISTA_TARGET_FULL_DEPENDENCIES )
				# store dependencies only if we were not called recursively, and only if at least one library is actually added
				if( NOT VISTA_USE_PACKAGE_RECURSION_COUNT OR VISTA_USE_PACKAGE_RECURSION_COUNT EQUAL 0 )
					list( APPEND VISTA_TARGET_DEPENDENCIES "package" ${ARGV} )
				endif()
			endif()

			# add libraries to VISTA_USE_PACKAGE_LIBRARIES. We do this after adding dependencies, and
			# by pre-prending, in order to provide correct static linking under linux, where the dependent library
			# has to be listed before the one it depends on
			# To achieve this, we must first build a list of the current package's libraries, then process all
			# its dependencies (which may append libraries to the internal storage), and finally prepend
			# the internal storage to VISTA_USE_PACKAGE_LIBRARIES
			set( INTERNAL_VISTA_FIND_PACKAGE_LIBS ${INTERNAL_VISTA_FIND_PACKAGE_LIBS} ${${_PACKAGE_NAME_UPPER}_LIBRARIES} )
						
			if( ${_PACKAGE_NAME_UPPER}_SHADER_DIRS )
				list( APPEND VISTA_SHADER_DIRECTORIS ${${_PACKAGE_NAME_UPPER}_SHADER_DIRS} )
				list( REMOVE_DUPLICATES VISTA_SHADER_DIRECTORIS )
			endif( ${_PACKAGE_NAME_UPPER}_SHADER_DIRS )	
			
			#mark call now (lateron, _PACKAGE_NAME_UPPER might be overwritten by sub-calls for dependencies)
			set( VISTA_USE_${_PACKAGE_NAME_UPPER} TRUE )

			# parse dependencies automatically call vista_use_package on not previously found ones
			# indicate for them that they are recursively called, to prevent adding them to the dependency list,
			# and to ensure correct ordering of libraries
			if( NOT VISTA_USE_PACKAGE_RECURSION_COUNT )
				set( VISTA_USE_PACKAGE_RECURSION_COUNT 1 )
			else()
				math( EXPR VISTA_USE_PACKAGE_RECURSION_COUNT "${VISTA_USE_PACKAGE_RECURSION_COUNT} + 1" )
			endif()
			
			set( _DEPENDENCY_ARGS )
			# the last package ensures that the last listed package is included, too
			foreach( _DEPENDENCY ${${_PACKAGE_NAME_UPPER}_DEPENDENCIES} "package" ) 
				string( REGEX MATCH "^([^\\-]+)\\-(.+)$" _MATCHED ${_DEPENDENCY} )
				if( _DEPENDENCY STREQUAL "package" )
					if( _DEPENDENCY_ARGS AND NOT "${_DEPENDENCY_ARGS}" STREQUAL "" )
						list( GET _DEPENDENCY_ARGS 0 _DEPENDENCY_NAME )
						string( TOUPPER "${_DEPENDENCY_NAME}" _DEPENDENCY_NAME_UPPER )
						if( _FIND_DEPENDENCIES )
							#if( NOT ${_DEPENDENCY_NAME_UPPER}_FOUND )
								# find and use the dependency. If it fails, utter a warning
								if( NOT _QUIET )
									string( REPLACE ";" " " _DEPENDENCY_ARGS_STRING "${_DEPENDENCY_ARGS}" )
									set( _MESSAGE_IF_DO_FIND "Automatically adding ${_PACKAGE_NAME}-dependency \"${_DEPENDENCY_ARGS_STRING}\"" )
								endif( NOT _QUIET )
								vista_use_package( ${_DEPENDENCY_ARGS} FIND_DEPENDENCIES )
								if( NOT ${_DEPENDENCY_NAME_UPPER}_FOUND AND NOT _QUIET )
									message( WARNING "vista_use_package( ${_PACKAGE_NAME} ) - Package depends on \"${_DEPENDENCY_ARGS}\", but including it failed" )
								endif( NOT ${_DEPENDENCY_NAME_UPPER}_FOUND AND NOT _QUIET )
							#endif( NOT ${_DEPENDENCY_NAME_UPPER}_FOUND )
						else()
							# check if dependencies are already included. If not, utter a warning
							if( NOT ${_DEPENDENCY_NAME_UPPER}_FOUND AND NOT _QUIET )
								string( REPLACE ";" " " _DEPENDENCY_ARGS_STRING "${_DEPENDENCY_ARGS}" )
								message( "vista_use_package( ${_PACKAGE_NAME} ) - Package depends on \"${_DEPENDENCY_ARGS_STRING}\", which was not found yet - "
										"make sure to find this package before searchig for ${_PACKAGE_NAME}, or use parameter FIND_DEPENDENCIES" )
							endif()
						endif( _FIND_DEPENDENCIES )
						set( _DEPENDENCY_ARGS )
					endif()
				else()
					list( APPEND _DEPENDENCY_ARGS ${_DEPENDENCY} )
				endif()
			endforeach()

			math( EXPR VISTA_USE_PACKAGE_RECURSION_COUNT "${VISTA_USE_PACKAGE_RECURSION_COUNT} - 1" )

			#finally: prepend correctly ordered list of libraries to VISTA_USE_PACKAGE_LIBRARIES
			if( VISTA_USE_PACKAGE_RECURSION_COUNT EQUAL 0 )
				set( VISTA_USE_PACKAGE_LIBRARIES ${INTERNAL_VISTA_FIND_PACKAGE_LIBS} ${VISTA_USE_PACKAGE_LIBRARIES} )
				set( INTERNAL_VISTA_FIND_PACKAGE_LIBS )
			endif()			

		endif()		
	endif()
endmacro()

# vista_find_shader_dirs( PACKAGE_NAME )
# searches recursively for all subdirectories of CURRENT_SOURCE_DIR that contain shader files and 
# adds them to the list of shader directories
macro( vista_find_shader_dirs _PACKAGE_NAME )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
	
	set ( ${_PACKAGE_NAME_UPPER}_INSTALL_SHADERS TRUE )
	
	set ( _EXTENSIONS "vp" "fp" "glsl" )
	
	foreach( _EXT ${_EXTENSIONS} )
		file( GLOB_RECURSE _FOUND_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.${_EXT}" )			
		foreach( _FILE ${_FOUND_FILES} )
			string( REGEX REPLACE "(.*)/(.*)\\.${_EXT}" "\\1" _DIR ${_FILE} )
			list( APPEND ${_PACKAGE_NAME_UPPER}_SHADER_DIRS ${_DIR} )
			list( REMOVE_DUPLICATES ${_PACKAGE_NAME_UPPER}_SHADER_DIRS )
		endforeach( _FILE ${_FOUND_FILES} )
	endforeach( _EXT ${_EXTENSIONS} )
	
endmacro( vista_find_shader_dirs _PACKAGE_NAME )

# vista_configure_app( package_name [output_name] [DONT_COPY_EXECUTABLE | COPY_EXECUTABLE_TO dir] [WORKING_DIR dir] )
# sets some general properties for the target to configure it as application
#	sets default out_dir (i.e. where the executable will be built) to the /bin subdir in the binary tree
#	sets the Application Name to _PACKAGE_NAME with "D"-PostFix under Debug
#	if not overwritten, sets the outdir to the target's source directory
#	creates a shell script that sets the path to find required libraries
#	for MSVC, a *.vcproj.user file is created, setting Working Directory and Path Environment
#   By default, the executable (together with the path script and an optional build info file)
#   is created in the outdir, but to the source dir copied after a successful build.
#   This behavior can be controlled by adding the parameter DONT_COPY_EXECUTABLE if the files should
#   not be copied, or by COPY_EXECUTABLE_TO TARGET_DIR to specify an alternative copy destination
macro( vista_configure_app _PACKAGE_NAME )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )

	set( ${_PACKAGE_NAME_UPPER}_TARGET_TYPE "APP" )

	set( ${_PACKAGE_NAME_UPPER}_OUTPUT_NAME ${_PACKAGE_NAME} CACHE INTERNAL "" FORCE )
	
	# parse arguments
	set( ${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
	set( _OVERRIDE_WORKING_DIR )
	set( _PARSE_MODE 0 ) # 0: first entry 1: awaiting command 2: next is TargetDir 3: next is WorkingDir
	
	foreach( _ARG ${ARGN} )
		if( _ARG STREQUAL "DONT_COPY_EXECUTABLE" )
			set( ${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR )
			set( _PARSE_MODE 1 )
		elseif( _ARG STREQUAL "COPY_EXECUTABLE_TO" )
			set( _PARSE_MODE 2 )
		elseif( _ARG STREQUAL "WORKING_DIR" )
			set( _PARSE_MODE 3 )
		else()
			if( _PARSE_MODE EQUAL 0 )
				set( ${_PACKAGE_NAME_UPPER}_OUTPUT_NAME ${_ARG} CACHE INTERNAL "" FORCE )
			elseif( _PARSE_MODE EQUAL 2 )
				set( ${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR "${_ARG}" )
			elseif( _PARSE_MODE EQUAL 3 )
				set( _OVERRIDE_WORKING_DIR "${_ARG}" )
			else()
				message( WARNING "vista_configure_app( ${_PACKAGE_NAME} ) - could not parse argument \"${_ARG}\"" )
			endif()
		endif()
	endforeach()

	set_target_properties( ${_PACKAGE_NAME} PROPERTIES OUTPUT_NAME_DEBUG			"${${_PACKAGE_NAME_UPPER}_OUTPUT_NAME}${CMAKE_DEBUG_POSTFIX}" )
	set_target_properties( ${_PACKAGE_NAME} PROPERTIES OUTPUT_NAME_RELEASE			"${${_PACKAGE_NAME_UPPER}_OUTPUT_NAME}" )
	set_target_properties( ${_PACKAGE_NAME} PROPERTIES OUTPUT_NAME_MINSIZEREL 		"${${_PACKAGE_NAME_UPPER}_OUTPUT_NAME}" )
	set_target_properties( ${_PACKAGE_NAME} PROPERTIES OUTPUT_NAME_RELWITHDEBINFO	"${${_PACKAGE_NAME_UPPER}_OUTPUT_NAME}" )
	set_target_properties( ${_PACKAGE_NAME} PROPERTIES OUTPUT_NAME					"${${_PACKAGE_NAME_UPPER}_OUTPUT_NAME}" )

	if( NOT DEFINED ${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR )
		vista_set_outdir( ${_PACKAGE_NAME} "${CMAKE_CURRENT_BINARY_DIR}/bin" )
	else()
		if( ${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR_WITH_CONFIG_SUBDIRS )
			vista_set_outdir( ${_PACKAGE_NAME} ${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR} USE_CONFIG_SUBDIRS )
		else()
			vista_set_outdir( ${_PACKAGE_NAME} ${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR} )
		endif()
	endif()
	
	# we store the dependencies as required
	set( ${_PACKAGE_NAME_UPPER}_DEPENDENCIES ${VISTA_TARGET_DEPENDENCIES} CACHE INTERNAL "" FORCE )
	set( ${_PACKAGE_NAME_UPPER}_FULL_DEPENDENCIES ${VISTA_TARGET_FULL_DEPENDENCIES} CACHE INTERNAL "" FORCE )
	
	set( _DYNAMIC_LIB_DIRS ${VISTA_TARGET_LINK_DIRS} ${VISTA_${_PACKAGE_NAME}_ADDITIONAL_PATHENTRIES} ${VISTA_ADDITIONAL_PATHENTRIES} )
	# create a script that sets the path
	if( _DYNAMIC_LIB_DIRS OR VISTA_ENVVARS OR VISTA_${_PACKAGE_NAME}_ENVVARS )
		if( WIN32 )
			find_file( VISTA_ENVIRONMENT_SCRIPT_FILE "set_path.bat_proto" ${CMAKE_MODULE_PATH} )
			mark_as_advanced( VISTA_ENVIRONMENT_SCRIPT_FILE )
			
			set( _ENTRIES "" )
			if( _DYNAMIC_LIB_DIRS )
				set( _ENTRIES "SET PATH=${_DYNAMIC_LIB_DIRS};%PATH%\n" )
			endif()
			if( VISTACORELIBS_DRIVER_PLUGIN_DIRS )
				set( _ENTRIES "${_ENTRIES}SET VISTACORELIBS_DRIVER_PLUGIN_DIRS=${VISTACORELIBS_DRIVER_PLUGIN_DIRS}\n" )
			endif()
			if( VISTA_SHADER_DIRECTORIS )
				set( _ENTRIES "${_ENTRIES}SET VISTA_SHADER_DIRS=${VISTA_SHADER_DIRECTORIS}\n" )
			endif()
			set( _ENVVARNAME "" )
			foreach( _ENTRY ${VISTA_ENVVARS} ${VISTA_${_PACKAGE_NAME}_ENVVARS} )
				if( _ENVVARNAME STREQUAL "" )
					set( _ENVVARNAME "${_ENTRY}" )
				else()
					set( _ENTRIES "${_ENTRIES}SET ${_ENVVARNAME}=${_ENTRY}\n" )
					set( _ENVVARNAME "" )
				endif()
			endforeach()
						
			if( VISTA_ENVIRONMENT_SCRIPT_FILE )
				set( ${_PACKAGE_NAME_UPPER}_SET_PATH_SCRIPT "${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR}/set_path_for_${_PACKAGE_NAME}.bat" )
				configure_file(
						${VISTA_ENVIRONMENT_SCRIPT_FILE}
						"${${_PACKAGE_NAME_UPPER}_SET_PATH_SCRIPT}"
						@ONLY
				)
			endif()
		elseif( UNIX )
			find_file( VISTA_ENVIRONMENT_SCRIPT_FILE "set_path.sh_proto" ${CMAKE_MODULE_PATH} )
			mark_as_advanced( VISTA_ENVIRONMENT_SCRIPT_FILE )
			
			set( _ENTRIES "" )
			if( _DYNAMIC_LIB_DIRS )
				set( _ENTRIES "export LD_LIBRARY_PATH=" )
				foreach (_ENTRY ${_DYNAMIC_LIB_DIRS} )
					if( NOT "${_ENTRY}" STREQUAL "/usr/lib" AND NOT "${_ENTRY}" STREQUAL "/usr/lib64" AND NOT "${_ENTRY}" STREQUAL "/lib" )
						set( _ENTRIES "${_ENTRIES}${_ENTRY}:" )
					endif()
				endforeach()
				set ( _ENTRIES "${_ENTRIES}$LD_LIBRARY_PATH\n" )
			endif()
			if( VISTACORELIBS_DRIVER_PLUGIN_DIRS)
				string( REPLACE ";" ":" _VISTACORELIBS_DRIVER_PLUGIN_DIRS_ENV "${VISTACORELIBS_DRIVER_PLUGIN_DIRS}" )
				set( _ENTRIES "${_ENTRIES}export VISTACORELIBS_DRIVER_PLUGIN_DIRS=${_VISTACORELIBS_DRIVER_PLUGIN_DIRS_ENV}\n" )
			endif()
			if( VISTA_SHADER_DIRECTORIS)
				string( REPLACE ";" ":" _VISTA_SHADER_DIRECTORIES_ENV "${VISTA_SHADER_DIRECTORIS}" )
				set( _ENTRIES "${_ENTRIES}export VISTA_SHADER_DIRS=${_VISTA_SHADER_DIRECTORIES_ENV}\n" )
			endif()
			set( _ENVVARNAME "" )
			foreach( _ENTRY ${VISTA_ENVVARS} ${VISTA_${_PACKAGE_NAME}_ENVVARS} )
				if( "${${_ENVVARNAME}}" STREQUAL "" )
					set( _ENVVARNAME ${_ENTRY} )
				else()
					set( _ENTRIES "${_ENTRIES}export ${_ENVVARNAME}=${_ENTRY}\n" )
					set( _ENVVARNAME "" )
				endif()
			endforeach()
			
			if( VISTA_ENVIRONMENT_SCRIPT_FILE )
				set( ${_PACKAGE_NAME_UPPER}_SET_PATH_SCRIPT "${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR}/set_path_for_${_PACKAGE_NAME}.sh" )
				configure_file(
						"${VISTA_ENVIRONMENT_SCRIPT_FILE}"
						"${${_PACKAGE_NAME_UPPER}_SET_PATH_SCRIPT}"
						@ONLY
				)
			endif()
		endif()
	endif()

	# set up copying of executable after build
	set( ${_PACKAGE_NAME_UPPER}_TARGET_MSVC_PROJECT "" CACHE INTERNAL "" FORCE )
	
		if( "${${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR}" STREQUAL "${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR}" )
		# prevent copying to same location
		set( ${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR )
	endif()
	
	if( ${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR )
		if( CMAKE_VERSION VERSION_LESS 2.8.5 )
			message( WARNING "vista_configure_app( ${_PACKAGE_NAME} ) - executable copying is only supported in cmake 2.8.5+ - use DONT_COPY_EXECUTABLE option or update cmake" )
		else()
			add_custom_command(	TARGET ${_PACKAGE_NAME}
								POST_BUILD
								COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR}"
								COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<TARGET_FILE:${_PACKAGE_NAME}>" "${${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR}"
								COMMENT "Post-build copying of files" )

			if( ${_PACKAGE_NAME_UPPER}_SET_PATH_SCRIPT )
				add_custom_command( TARGET ${_PACKAGE_NAME}
									POST_BUILD
									COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different "${${_PACKAGE_NAME_UPPER}_SET_PATH_SCRIPT}" "${${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR}"
									COMMENT "Post-build copying of files" )
			endif()			
		endif()
	endif()
	
	#if we're usign MSVC, we set up a *.vcproj.user file
	if( MSVC )
		if( MSVC10 OR MSVC11 OR MSVC12 OR MSVC14 )
			set( VISTA_VCPROJUSER_PROTO_FILE_NAME "VisualStudio2010.vcxproj.user_proto" )
		else()
			set( VISTA_VCPROJUSER_PROTO_FILE_NAME "VisualStudio.vcproj.user_proto")
		endif()
		find_file( VISTA_VCPROJUSER_PROTO_FILE "${VISTA_VCPROJUSER_PROTO_FILE_NAME}" ${CMAKE_MODULE_PATH} )
		set( VISTA_VCPROJUSER_PROTO_FILE ${VISTA_VCPROJUSER_PROTO_FILE} CACHE INTERNAL "" )
		if( VISTA_VCPROJUSER_PROTO_FILE )
			if( VISTA_64BIT )
				set( _CONFIG_NAME "x64" )
			else()
				set( _CONFIG_NAME "Win32" )
			endif()

			if( MSVC80 )
				set( _VERSION_STRING "8,00" )
			elseif( MSVC90 )
				set( _VERSION_STRING "9,00" )
			elseif( MSVC10 )
				set( _VERSION_STRING "10,00" )
			elseif( MSVC11 )
				set( _VERSION_STRING "11,00" )
			elseif( MSVC12 )
				set( _VERSION_STRING "12,00" )
			elseif( MSVC14 )
				set( _VERSION_STRING "14,00" )
			endif()

			if( _OVERRIDE_WORKING_DIR )
				set( _WORK_DIR "${_OVERRIDE_WORKING_DIR}" )
			elseif( ${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR )
				set( _WORK_DIR "${${_PACKAGE_NAME_UPPER}_COPY_EXEC_DIR}" )
			else()
				set( _WORK_DIR "${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR}" )
			endif()

			set( _ENVIRONMENT "" )
			if( _DYNAMIC_LIB_DIRS )
				set( _ENVIRONMENT "PATH=${_DYNAMIC_LIB_DIRS};%PATH%&#x0A;" )
			endif()
			if( VISTACORELIBS_DRIVER_PLUGIN_DIRS )
				set( _ENVIRONMENT "${_ENVIRONMENT}VISTACORELIBS_DRIVER_PLUGIN_DIRS=${VISTACORELIBS_DRIVER_PLUGIN_DIRS}&#x0A;" )
			endif()
			if( VISTA_SHADER_DIRECTORIS )
				set( _ENVIRONMENT "${_ENVIRONMENT}VISTA_SHADER_DIRS=${VISTA_SHADER_DIRECTORIS}&#x0A;" )
			endif()
			
			set( _ENVVARNAME "" )
			foreach( _ENTRY ${VISTA_${_PACKAGE_NAME}_ENVVARS} ${VISTA_ENVVARS} )
				if( _ENVVARNAME STREQUAL "" )
					set( _ENVVARNAME "${_ENTRY}" )
				else()
					set( _ENVIRONMENT "${_ENVIRONMENT}${_ENVVARNAME}=${_ENTRY}&#x0A;" )
					set( _ENVVARNAME "" )
				endif()
			endforeach()
			
			set( _COMMANDARGS ${VISTA_${_PACKAGE_NAME}_MSVC_ARGUMENTS} )
			
			if( MSVC10 OR MSVC11 OR MSVC12 OR MSVC14 )
				configure_file(
					${VISTA_VCPROJUSER_PROTO_FILE}
					${CMAKE_CURRENT_BINARY_DIR}/${_PACKAGE_NAME}.vcxproj.user
					@ONLY
				)
				set( ${_PACKAGE_NAME_UPPER}_TARGET_MSVC_PROJECT "${CMAKE_CURRENT_BINARY_DIR}/${_PACKAGE_NAME}.vcxproj" CACHE INTERNAL "" FORCE )
			else()
				configure_file(
					${VISTA_VCPROJUSER_PROTO_FILE}
					${CMAKE_CURRENT_BINARY_DIR}/${_PACKAGE_NAME}.vcproj.user
					@ONLY
				)
				set( ${_PACKAGE_NAME_UPPER}_TARGET_MSVC_PROJECT "${CMAKE_CURRENT_BINARY_DIR}/${_PACKAGE_NAME}.vcproj" CACHE INTERNAL "" FORCE )
			endif()						
		else( VISTA_VCPROJUSER_PROTO_FILE )
			message( WARNING "vista_configure_app( ${_PACKAGE_NAME} ) - could not find file VisualStudio project user setting prototype \"${VISTA_VCPROJUSER_PROTO_FILE}\"" )
		endif( VISTA_VCPROJUSER_PROTO_FILE )
	endif( MSVC )	

endmacro( vista_configure_app )

# vista_configure_lib( _PACKAGE_NAME [OUT_NAME] )
# sets some general properties for the target to configure it as application
#	sets default value for CMAKE_INSTALL_PREFIX (if not set otherwise) to /dist/VISTA_HWARCH
#	if not overwritten, sets the outdir to the target's source directory
#	adds *_EXPORT or *_STATIC definition
macro( vista_configure_lib _PACKAGE_NAME )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )

	set( ${_PACKAGE_NAME_UPPER}_TARGET_TYPE "LIB" )

	set( ${_PACKAGE_NAME_UPPER}_OUTPUT_NAME ${_PACKAGE_NAME} CACHE INTERNAL "" FORCE )
	if( ${ARGC} GREATER 1 )
		set( ${_PACKAGE_NAME_UPPER}_OUTPUT_NAME ${ARGV1} CACHE INTERNAL "" FORCE )
	endif()

	set_target_properties( ${_PACKAGE_NAME} PROPERTIES OUTPUT_NAME	"${${_PACKAGE_NAME_UPPER}_OUTPUT_NAME}" )

	if( NOT DEFINED ${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR )
		vista_set_outdir( ${_PACKAGE_NAME} "${CMAKE_BINARY_DIR}/lib" )
	else()
		if( ${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR_WITH_CONFIG_SUBDIRS )
			vista_set_outdir( ${_PACKAGE_NAME} ${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR} USE_CONFIG_SUBDIRS )
		else()
			vista_set_outdir( ${_PACKAGE_NAME} ${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR} )
		endif( ${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR_WITH_CONFIG_SUBDIRS )
	endif()

	# we store the dependencies as required
	set( ${_PACKAGE_NAME_UPPER}_DEPENDENCIES ${VISTA_TARGET_DEPENDENCIES} CACHE INTERNAL "" FORCE )
	set( ${_PACKAGE_NAME_UPPER}_FULL_DEPENDENCIES ${VISTA_TARGET_FULL_DEPENDENCIES} CACHE INTERNAL "" FORCE )

	string( TOUPPER ${_PACKAGE_NAME} _NAME_UPPER )	
	
	set( ${_PACKAGE_NAME_UPPER}_TARGET_MSVC_PROJECT "" CACHE INTERNAL "" FORCE )
	if( WIN32 )
		get_target_property( _BUILD_TYPE ${_PACKAGE_NAME} TYPE )
		if( "${_BUILD_TYPE}" STREQUAL "SHARED_LIBRARY" OR "${_BUILD_TYPE}" STREQUAL "MODULE_LIBRARY" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES COMPILE_FLAGS -D${_NAME_UPPER}_EXPORTS )
		else()
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES COMPILE_FLAGS -D${_NAME_UPPER}_STATIC )
		endif()
		
		# store location to msvcproj file
		if( MSVC10 OR MSVC11 OR MSVC12 OR MSVC14 )
			set( ${_PACKAGE_NAME_UPPER}_TARGET_MSVC_PROJECT "${CMAKE_CURRENT_BINARY_DIR}/${_PACKAGE_NAME}.vcxproj" CACHE INTERNAL "" FORCE )
		elseif( MSVC )
			set( ${_PACKAGE_NAME_UPPER}_TARGET_MSVC_PROJECT "${CMAKE_CURRENT_BINARY_DIR}/${_PACKAGE_NAME}.vcproj" CACHE INTERNAL "" FORCE )
		endif()
	elseif( UNIX )
		if( NOT ${_PACKAGE_NAME_UPPER}_VERSION_MAJOR )
			set( ${_PACKAGE_NAME_UPPER}_VERSION_MAJOR 1 )
		endif()
		if( NOT ${_PACKAGE_NAME_UPPER}_VERSION_POSIX )
			set( ${_PACKAGE_NAME_UPPER}_VERSION_POSIX "1.0.0" )
		endif()
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES VERSION ${${_PACKAGE_NAME_UPPER}_VERSION_POSIX} SOVERSION ${${_PACKAGE_NAME_UPPER}_VERSION_MAJOR} )
	endif()
endmacro()


# vista_add_target_pathscript_dynamic_lib_path( _PACKAGE_NAME _PATH [PATH_LIST] )
# adds an environment variable that will be added to the set_path_for_* scripts
# and the msvc projects for the specified application target
# if the option PATH_LIST is provided, the input will be transformed to a list
# of pathes using the OS-specific separator
macro( vista_add_target_pathscript_dynamic_lib_path _PACKAGE_NAME _PATH )
	if( UNIX AND ${ARGC} GREATER 2 AND "${ARGV2}" STREQUAL "PATH_LIST" )
		string( REPLACE ";" ":" _OUTPUT_LIST "${_PATH}" )
		list( APPEND VISTA_${_PACKAGE_NAME}_ADDITIONAL_PATHENTRIES "${_OUTPUT_LIST}" )
	else()
		list( APPEND VISTA_${_PACKAGE_NAME}_ADDITIONAL_PATHENTRIES "${_PATH}" )
	endif()
endmacro()

# vista_add_pathscript_dynamic_lib_path( _PATH )
# adds an environment variable that will be added to the set_path_for_* scripts
# and the msvc projects for all apps that are configured afterwards
# if the option PATH_LIST is provided, the input will be transformed to a list
# of pathes using the OS-specific separator
macro( vista_add_pathscript_dynamic_lib_path _PATH )
	if( UNIX AND ${ARGC} GREATER 1 AND "${ARGV1}" STREQUAL "PATH_LIST" )
		string( REPLACE ";" ":" _OUTPUT_LIST "${_PATH}" )
		list( APPEND VISTA_${_PACKAGE_NAME}_ADDITIONAL_PATHENTRIES "${_OUTPUT_LIST}" )
	else()
		list( APPEND VISTA_ADDITIONAL_PATHENTRIES "${_PATH}" )
	endif( UNIX AND ${ARGC} GREATER 1 AND "${ARGV1}" STREQUAL "PATH_LIST" )
endmacro( vista_add_pathscript_dynamic_lib_path _PATH )


# vista_add_target_pathscript_envvar( _PACKAGE_NAME _ENVVAR _VALUE )
# adds an environment variable that will be added to the set_path_for_* scripts
# and the msvc projects for the specified application target
# if the option PATH_LIST is provided, the input will be transformed to a list
# of pathes using the OS-specific separator
macro( vista_add_target_pathscript_envvar _PACKAGE_NAME _ENVVAR _VALUE )
	if( UNIX AND ${ARGC} GREATER 4 AND "${ARGV4}" STREQUAL "PATH_LIST" )
		string( REPLACE ";" ":" _OUTPUT_LIST "${_VALUE}" )
		list( APPEND VISTA_${_PACKAGE_NAME}_ENVVARS "${_ENVVAR}" "${_OUTPUT_LIST}" )
	else()
		list( APPEND VISTA_${_PACKAGE_NAME}_ENVVARS "${_ENVVAR}" "${_VALUE}" )
	endif( UNIX AND ${ARGC} GREATER 4 AND "${ARGV4}" STREQUAL "PATH_LIST" )
endmacro( vista_add_target_pathscript_envvar )

# vista_add_target_pathscript_envvar( _ENVVAR _VALUE )
# adds an environment variable that will be added to the set_path_for_* scripts
# and the msvc projects for all apps that are configured afterwards
# if the option PATH_LIST is provided, the input will be transformed to a list
# of pathes using the OS-specific separator
macro( vista_add_pathscript_envvar _ENVVAR _VALUE )
	if( UNIX AND ${ARGC} GREATER 3 AND "${ARGV3}" STREQUAL "PATH_LIST" )
		string( REPLACE ";" ":" _OUTPUT_LIST "${_VALUE}" )
		list( APPEND VISTA_ENVVARS "${_ENVVAR}" "${_OUTPUT_LIST}" )
	else()
		list( APPEND VISTA_ENVVARS "${_ENVVAR}" "${_VALUE}" )
	endif( UNIX AND ${ARGC} GREATER 3 AND "${ARGV3}" STREQUAL "PATH_LIST" )
endmacro( vista_add_pathscript_envvar )


# vista_set_target_msvc_arguments( _PACKAGE_NAME _COMMANDLINE_VARS )
# sets the default commandline args in the msvc project for the specified application target
macro( vista_set_target_msvc_arguments _PACKAGE_NAME _COMMANDLINE_VARS )
	if( UNIX AND ${ARGC} GREATER 1 AND "${ARGV1}" STREQUAL "PATH_LIST" )
		string( REPLACE ";" ":" _OUTPUT_LIST "${_COMMANDLINE_VARS}" )
		set( VISTA_${_PACKAGE_NAME}_MSVC_ARGUMENTS ${_OUTPUT_LIST} )
	else()
		set( VISTA_${_PACKAGE_NAME}_MSVC_ARGUMENTS ${_COMMANDLINE_VARS} )
	endif( UNIX AND ${ARGC} GREATER 1 AND "${ARGV1}" STREQUAL "PATH_LIST" )
endmacro( vista_set_target_msvc_arguments )

# vista_install( TARGET [INCLUDE/BIN_SUBDIRECTORY [LIBRARY_SUBDIRECTORY] ] [NO_POSTFIX] )
# can only be called after vista_configure_[app|lib]
# installs generic files (headers, librarys, executables, .pdb's)
# headers will be installed to include, or to include/INCLUDE_SUBDIRECTORY, maintaining their
# local subfolder in the project (excluding folders names src, source, or include)
# libraries/dlls will be installed to lib, or to lib/LIBRARY_SUBDIRECTORY
# executables will be installed in a /bin/BIN_SUBDIR subdir
# all the postfixes (bin/lib/include) can be prevented by adding the NO_POSTFIX option
macro( vista_install _PACKAGE_NAME )

	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )

	set( _USE_POSTFIX TRUE )
	set( _ARGS ${ARGV} )
	list( FIND _ARGS "NO_POSTFIX" _NO_POSTFIX_FOUND_FOUND )
	if( _NO_POSTFIX_FOUND_FOUND GREATER -1 )
		set( _USE_POSTFIX FALSE )
	endif( _NO_POSTFIX_FOUND_FOUND GREATER -1 )

	if( _USE_POSTFIX )
		set( ${_PACKAGE_NAME_UPPER}_INC_INSTALLDIR "${CMAKE_INSTALL_PREFIX}/include" )

		if( UNIX AND VISTA_64BIT )
			set( ${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR "${CMAKE_INSTALL_PREFIX}/lib64" )
		else( UNIX AND VISTA_64BIT )
			set( ${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR "${CMAKE_INSTALL_PREFIX}/lib" )
		endif( UNIX AND VISTA_64BIT )

		set( ${_PACKAGE_NAME_UPPER}_BIN_INSTALLDIR "${CMAKE_INSTALL_PREFIX}/bin" )
		
		if( ${_PACKAGE_NAME_UPPER}_INSTALL_SHADERS )
			set( ${_PACKAGE_NAME_UPPER}_SHADER_INSTALLDIR "${CMAKE_INSTALL_PREFIX}/shaders" )
		endif( ${_PACKAGE_NAME_UPPER}_INSTALL_SHADERS )
	else( _USE_POSTFIX )
		set( ${_PACKAGE_NAME_UPPER}_INC_INSTALLDIR "${CMAKE_INSTALL_PREFIX}" )
		set( ${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR "${CMAKE_INSTALL_PREFIX}" )
		set( ${_PACKAGE_NAME_UPPER}_BIN_INSTALLDIR "${CMAKE_INSTALL_PREFIX}" )
		
		if( ${_PACKAGE_NAME_UPPER}_INSTALL_SHADERS )
			set( ${_PACKAGE_NAME_UPPER}_SHADER_INSTALLDIR "${CMAKE_INSTALL_PREFIX}" )
		endif( ${_PACKAGE_NAME_UPPER}_INSTALL_SHADERS )
	endif( _USE_POSTFIX )

	if( ${ARGC} GREATER 1 AND NOT ${ARGV1} STREQUAL "NO_POSTFIX"  )
		set( ${_PACKAGE_NAME_UPPER}_INC_INSTALLDIR "${${_PACKAGE_NAME_UPPER}_INC_INSTALLDIR}/${ARGV1}" )
		set( ${_PACKAGE_NAME_UPPER}_BIN_INSTALLDIR "${${_PACKAGE_NAME_UPPER}_BIN_INSTALLDIR}/${ARGV1}" )
	endif( ${ARGC} GREATER 1 AND NOT ${ARGV1} STREQUAL "NO_POSTFIX"  )

	if( ${ARGC} GREATER 2 AND NOT ${ARGV2} STREQUAL "NO_POSTFIX" )
		set( ${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR "${${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR}/${ARGV2}" )
	endif( ${ARGC} GREATER 2 AND NOT ${ARGV2} STREQUAL "NO_POSTFIX"  )

	if( ${_PACKAGE_NAME_UPPER}_TARGET_TYPE STREQUAL "APP" )
		install( TARGETS ${_PACKAGE_NAME}
			RUNTIME DESTINATION ${${_PACKAGE_NAME_UPPER}_BIN_INSTALLDIR}
			PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_EXEC}
		)
		#if( WIN32 )
		#	install( FILES "${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR}/set_path_for_${_PACKAGE_NAME}.bat"
		#				DESTINATION ${${_PACKAGE_NAME_UPPER}_BIN_INSTALLDIR}
		#				PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_EXEC} )
		#else( WIN32 )
		#	install( FILES "${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR}/set_path_for_${_PACKAGE_NAME}.sh"
		#				DESTINATION ${${_PACKAGE_NAME_UPPER}_BIN_INSTALLDIR}
		#				PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_EXEC} )
		#endif( WIN32 )
	else()
		install( TARGETS ${_PACKAGE_NAME}
			LIBRARY DESTINATION ${${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR}
			ARCHIVE DESTINATION ${${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR}
			RUNTIME DESTINATION ${${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR}
			PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC}
		)
		get_target_property( _SOURCE_FILES ${_PACKAGE_NAME} SOURCES )
		foreach( _FILE ${_SOURCE_FILES} )
			get_filename_component( _EXTENSION_TMP ${_FILE} EXT )
			string( TOLOWER "${_EXTENSION_TMP}" _EXTENSION )
			if( "${_EXTENSION}" STREQUAL ".h" OR "${_EXTENSION}" STREQUAL ".inl" )
				get_filename_component( _PATH ${_FILE} PATH )
				if( IS_ABSOLUTE "${_PATH}" )
					file( RELATIVE_PATH _PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${_PATH}" )
				endif()
				string( REPLACE "src" "" _PATH "${_PATH}" )
				string( REPLACE "Src" "" _PATH "${_PATH}" )
				string( REPLACE "source" "" _PATH "${_PATH}" )
				string( REPLACE "Source" "" _PATH "${_PATH}" )
				string( REPLACE "include" "" _PATH "${_PATH}" )
				string( REPLACE "Include" "" _PATH "${_PATH}" )
				install( FILES 	${_FILE} DESTINATION "${${_PACKAGE_NAME_UPPER}_INC_INSTALLDIR}/${_PATH}"
							PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC} )
			endif( )
		endforeach()
		
		if( ${_PACKAGE_NAME_UPPER}_INSTALL_SHADERS )
			set ( _EXTENSIONS "vp" "fp" "glsl" )
			foreach( _EXT ${_EXTENSIONS} )
				file( GLOB_RECURSE _FOUND_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.${_EXT}" )			
		
				install( FILES ${_FOUND_FILES} 
							DESTINATION "${${_PACKAGE_NAME_UPPER}_SHADER_INSTALLDIR}" 
							PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC} )
			endforeach()
		endif()

		if( MSVC )
			install( DIRECTORY "${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR}/"
				DESTINATION ${${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR}
				FILES_MATCHING PATTERN "*.pdb"
				PATTERN "build" EXCLUDE
				PATTERN ".svn" EXCLUDE
				PATTERN "CMakeFiles" EXCLUDE
				PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC}
			)
		endif()
	endif()
endmacro()

# vista_install_files_by_extension( SEARCH_ROOT INSTALL_SUBDIR [NON_RECURSIVE] [EXECUTABLE] EXTENSION1 [EXTENSION2 ...] )
# searches in SEARCH_ROOT dor all files matching any of the provided extensions, and
# installs them to the specified Subdir
# if NON_RECURSIVE is specified as first parameter after INSTALL_SUBDIR,only the top-level
# SEARCH_ROOT is searched, otherwise, all subdirs are parsed recursively, too
# NOTE: files are searched at configure time, not at install time! Thus, if you add a file
# matching the pattern, you have to configure cmake again to add it to the list of files to
# install
macro( vista_install_files_by_extension _SEARCH_ROOT _INSTALL_SUBDIR )
	set( _EXTENSIONS ${ARGN} )
	set( _RECURSIVE TRUE )
	set( _PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC} )
	
	if( "${ARGV2}" STREQUAL "NON_RECURSIVE" )
		set( _RECURSIVE FALSE )
		list( REMOVE_AT _EXTENSIONS 0 )
		if( "${ARGV3}" STREQUAL "EXECUTABLE" )
			set( _PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_EXEC} )
			list( REMOVE_AT _EXTENSIONS 0 )
		endif()
	elseif( "${ARGV2}" STREQUAL "EXECUTABLE" )
		set( _PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_EXEC} )
		list( REMOVE_AT _EXTENSIONS 0 )
		if( "${ARGV3}" STREQUAL "NON_RECURSIVE" )
			set( _RECURSIVE FALSE )
			list( REMOVE_AT _EXTENSIONS 0 )
		endif()
	endif()		
		
	if( _RECURSIVE )
		foreach( _EXT ${_EXTENSIONS} )
			file( GLOB_RECURSE _FOUND_FILES "${_SEARCH_ROOT}/*.${_EXT}" "${_SEARCH_ROOT}/**/*.${_EXT}" )			
			install( FILES ${_FOUND_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}/${_INSTALL_SUBDIR}" 
						PERMISSIONS ${_PERMISSIONS} )
		endforeach( _EXT ${_EXTENSIONS} )		
	else()
		foreach( _EXT ${_EXTENSIONS} )
			file( GLOB _FOUND_FILES "${_SEARCH_ROOT}/*.${_EXT}" )
			install( FILES ${_FOUND_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}/${_INSTALL_SUBDIR}"
						 PERMISSIONS ${_PERMISSIONS} )
		endforeach( _EXT ${_EXTENSIONS} )
	endif()
endmacro( vista_install_files_by_extension )


# vista_install_libs_by_buildtype( SEARCH_ROOT INSTALL_SUBDIR EXTENSION1 [EXTENSION2 ...] )
# searches in SEARCH_ROOT for all files matching any of the provided extensions, and
# installs them to the specified Subdir
# Excludes Debug libraries (*D.$EXTENSION) if buildtype(s) matches Release.
# NOTE: files are searched at configure time, not at install time! Thus, if you add a file
# matching the pattern, you have to configure cmake again to add it to the list of files to
# install
macro( vista_install_libs_by_buildtype _SEARCH_ROOT _INSTALL_SUBDIR )
	set( _EXTENSIONS ${ARGN} )
	set( _PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_EXEC} )
	
	foreach( _EXT ${_EXTENSIONS} )
		file( GLOB _FOUND_FILES "${_SEARCH_ROOT}/*${_EXT}*" )
		list( FIND CMAKE_CONFIGURATION_TYPES "Release" _FOUND_RELEASE )
		if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR NOT ${_FOUND_RELEASE} EQUAL -1 )
			foreach( _FILE ${_FOUND_FILES} )
				if( ${_FILE} MATCHES "(D.${_EXT}[.0-9]*)$" )
					list( REMOVE_ITEM _FOUND_FILES ${_FILE} )
				endif( ${_FILE} MATCHES "(D.${_EXT}[.0-9]*)$" )
			endforeach( _FILE ${_FOUND_FILES} )
		endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR NOT ${_FOUND_RELEASE} EQUAL -1 )
		install( FILES ${_FOUND_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}/${_INSTALL_SUBDIR}"
				 PERMISSIONS ${_PERMISSIONS} )
	endforeach( _EXT ${_EXTENSIONS} )
endmacro( vista_install_libs_by_buildtype )


# vista_install_all_dlls( INSTALL_SUBDIR )
# searches for ALL .dll's or .so's in all link directories, and installs them
# to the specified subdir. Only dlls that already exist at configure time will be installed!
# WARNING use with great care! this can potentially copy a whole lot of dlls if
# one of the lib's link dirs contains other dll's, too
# However, this script skips all .so's in /usr/lib* (and subfolders) and /lib* (and subfolders)
macro( vista_install_all_dlls _INSTALL_SUBDIR )
	foreach( _DIR ${VISTA_TARGET_LINK_DIRS} ${VISTACORELIBS_DRIVER_PLUGIN_DIRS} )
		if( WIN32 )
#			vista_install_files_by_extension( "${_DIR}" ${_INSTALL_SUBDIR} NON_RECURSIVE "dll" )
			vista_install_libs_by_buildtype( "${_DIR}" ${_INSTALL_SUBDIR} "dll" )
		elseif( UNIX )
			string( REGEX MATCH "^/usr/lib.*" _USR_LIB_MATCHED "${_DIR}" )
			string( REGEX MATCH "^/lib.*" _LIB_MATCHED "${_DIR}" )
			if( NOT _USR_LIB_MATCHED AND NOT _LIB_MATCHED )
#				vista_install_files_by_extension( ${_DIR} ${_INSTALL_SUBDIR} NON_RECURSIVE "so" "so.*" )
				vista_install_libs_by_buildtype( ${_DIR} ${_INSTALL_SUBDIR} "so" "so.*" )
			endif()
		endif()
	endforeach()
endmacro( vista_install_all_dlls )



# vista_create_cmake_config_build( PACKAGE_NAME CONFIG_PROTO_FILE TARGET_DIR )
# configures the specified <package>Config.cmake prototype file, and copies it to the
# target directory.
# Has to be called after vista_configure_lib to work properly
# If the cache variable VISTA_COPY_BUILD_CONFIGS_REFS_TO_CMAKECOMMON is ON -- and
# VISTA_CMAKE_COMMON env var is set -- a reference to this config is installed
# to VISTA_CMAKE_COMMON/share/. Additionally, previously installed older versions are removed.
# Furthermore, if a ConfigCMake of the same package and version already exists, it is parsed and
# the defined library dirs are adopted - this helps if multiple cmake builds are created for the same
# package
# The following variables are set internally to help configuring the configfile
#     _PACKAGE_NAME          - name of the package
#     _PACKAGE_NAME_UPPER    - uppercase name
#     _PACKAGE_LIBRARY_NAME  - output name of the package (not including Debug Postifix)
#     _PACKAGE_ROOT_DIR      - toplevel file of the package (i.e. directory from which vista_create_cmake_config_build is called)
#     _PACKAGE_LIBRARY_DIRS  - folder where the libraries are output to (not including optional postfixes)
#                              can be overwritten by defining ${_PACKAGE_NAME_UPPER}_LIBRARY_OUTDIR before calling the macro
#     _PACKAGE_INCLUDE_DIRS  - folder where the header files of the package are
#                              defaults to the current folder and the projects root folder
#                              can be overwritten by defining ${_PACKAGE_NAME_UPPER}_INCLUDE_OUTDIR before calling the macro
#     _PACKAGE_RELATIVE_LIBRARY_DIRS - _PACKAGE_LIBRARY_DIRS relative to current dir
#     _PACKAGE_RELATIVE_INCLUDE_DIRS - _PACKAGE_INCLUDE_DIRS relative to current dir
#     _PACKAGE_DEFINITIONS   - definitions for the package, defaults to nothing
#                              can be overwritten by defining ${_PACKAGE_NAME_UPPER}_CONFIG_DEFINITIONS before calling the macro
#     _PACKAGE_MSVC_PROJECT  - msvc project(s) that can be added to other solutions
#                              format: ( PROJ name location [ DEP ( dependency)* ] )*
#                              usually set to the internally stored ${_PACKAGE_NAME_UPPER}_TARGET_MSVC_PROJECT
#                              can be overwritten by setting ${_PACKAGE_NAME_UPPER}_MSVC_PROJECT_OVERWRITE
macro( vista_create_cmake_config_build _PACKAGE_NAME _CONFIG_PROTO_FILE _TARGET_DIR )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )

	# store the directory - it may be used by the versioning lateron
	set( ${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_DIR ${_TARGET_DIR} )

	#if VISTA_CMAKE_COMMON exisits, we give the user the cache options to toggle copying of references
	# to CISTA_CMAKE_COMMON/share on and off
	if( EXISTS "$ENV{VISTA_CMAKE_COMMON}" )
		set( VISTA_COPY_BUILD_CONFIGS_REFS_TO_CMAKECOMMON TRUE CACHE BOOL
			"if enabled, References to <Package>Config.cmake files will be copied to VistaCMakeCommon/share for easier finding" )
	endif( EXISTS "$ENV{VISTA_CMAKE_COMMON}" )


	# configure the temporary variables for configuring

	set( _PACKAGE_LIBRARY_NAME ${${_PACKAGE_NAME_UPPER}_OUTPUT_NAME} )

	# check if the library outdir should be overwritten
	set( _PACKAGE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
	if( ${_PACKAGE_NAME_UPPER}_LIBRARY_OUTDIR )
		set( _PACKAGE_LIBRARY_DIRS ${${_PACKAGE_NAME_UPPER}_LIBRARY_OUTDIR} )
	else()
		set( _PACKAGE_LIBRARY_DIRS ${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR} )
	endif( ${_PACKAGE_NAME_UPPER}_LIBRARY_OUTDIR )
	if( ${_PACKAGE_NAME_UPPER}_INCLUDE_OUTDIR )
		set( _PACKAGE_INCLUDE_DIRS ${${_PACKAGE_NAME_UPPER}_INCLUDE_OUTDIR} )
	else()
		set( _PACKAGE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"  )
		list( REMOVE_DUPLICATES _PACKAGE_INCLUDE_DIRS )
	endif( ${_PACKAGE_NAME_UPPER}_INCLUDE_OUTDIR )
	
	set(_PACKAGE_SHADER_DIRS )
	if( ${_PACKAGE_NAME_UPPER}_SHADER_DIRS )
		set( _PACKAGE_SHADER_DIRS ${${_PACKAGE_NAME_UPPER}_SHADER_DIRS} )
	endif( ${_PACKAGE_NAME_UPPER}_SHADER_DIRS )
	set(_PACKAGE_DEFINITIONS )
	if( ${_PACKAGE_NAME_UPPER}_CONFIG_DEFINITIONS )
		set( _PACKAGE_DEFINITIONS ${${_PACKAGE_NAME_UPPER}_CONFIG_DEFINITIONS} )
	endif( ${_PACKAGE_NAME_UPPER}_CONFIG_DEFINITIONS )
	
	# set the msvc project (check for overwrite)
	if( ${_PACKAGE_NAME_UPPER}_MSVC_PROJECT_OVERWRITE )
		set( _PACKAGE_MSVC_PROJECT ${${_PACKAGE_NAME_UPPER}_MSVC_PROJECT_OVERWRITE} )
	else()
		set( _PACKAGE_MSVC_PROJECT "PROJ ${_PACKAGE_NAME} \"${${_PACKAGE_NAME_UPPER}_TARGET_MSVC_PROJECT}\"" )
	endif()

	# if we should create a referenced config file, we create it's target dir
	if( VISTA_COPY_BUILD_CONFIGS_REFS_TO_CMAKECOMMON )
		set( ${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR "${VISTA_CMAKE_COMMON}/share/${_PACKAGE_NAME}" )
		if( DEFINED ${_PACKAGE_NAME_UPPER}_VERSION_EXT )
			set( ${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR
					"${${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR}-${${_PACKAGE_NAME_UPPER}_VERSION_EXT}-build" )
		else()
			set( ${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR
					"${${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR}-${VISTA_HWARCH}-build" )
		endif()

		# if any reference already exists, we parse it and append its library dirs to the current one
		# this helps if several different build types are used in different cmake-build-dirs, but
		local_use_existing_config_libs( ${_PACKAGE_NAME} "${_PACKAGE_ROOT_DIR}"
									"${${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR}/${_PACKAGE_NAME}Config.cmake"
									_PACKAGE_LIBRARY_DIRS )
	
	endif()

	# retrieve relative pathes for library/include dirs
	set( _PACKAGE_RELATIVE_INCLUDE_DIRS )
	foreach( _DIR ${_PACKAGE_INCLUDE_DIRS} )
		file( RELATIVE_PATH _REL_DIR "${_PACKAGE_ROOT_DIR}" "${_DIR}" )
		if( _REL_DIR )
			list( APPEND _PACKAGE_RELATIVE_INCLUDE_DIRS "${_REL_DIR}" )
		else()
			list( APPEND _PACKAGE_RELATIVE_INCLUDE_DIRS "." )
		endif()
	endforeach()

	set( _PACKAGE_RELATIVE_LIBRARY_DIRS )
	foreach( _DIR ${_PACKAGE_LIBRARY_DIRS} )		
		file( RELATIVE_PATH _REL_DIR "${_PACKAGE_ROOT_DIR}" "${_DIR}" )
		if( _REL_DIR )
			list( APPEND _PACKAGE_RELATIVE_LIBRARY_DIRS "${_REL_DIR}" )
		else()
			list( APPEND _PACKAGE_RELATIVE_LIBRARY_DIRS "." )
		endif()
	endforeach()

	#get_filename_component( _PATH_UP "${CMAKE_CURRENT_SOURCE_DIR}/.." REALPATH  )
	#list( APPEND _PACKAGE_INCLUDE_DIR "${_PATH_UP}" )
	#list( REMOVE_DUPLICATES _PACKAGE_INCLUDE_DIR )

	set( _TARGET_FILENAME "${${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_DIR}/${_PACKAGE_NAME}Config.cmake" )

	#  configure the actual file
	configure_file(	${_CONFIG_PROTO_FILE} ${_TARGET_FILENAME} @ONLY )

	#if we should create the reference - do so now
	set( _REFERENCED_FILE ${_TARGET_FILENAME} )
	if( VISTA_COPY_BUILD_CONFIGS_REFS_TO_CMAKECOMMON )
		# since prior configure runs may have already added it (before the cache was turned off), we
		# delete any prior copied versions to this location
		local_clean_old_config_references( ${_PACKAGE_NAME} "${_REFERENCED_FILE}" "${${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR}" )
		# find proto file
		find_file( VISTA_REFERENCE_CONFIG_PROTO_FILE "PackageConfigReference.cmake_proto" PATH ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH} )
		set( VISTA_REFERENCE_CONFIG_PROTO_FILE ${VISTA_REFERENCE_CONFIG_PROTO_FILE} CACHE INTERNAL "" FORCE )
		if( VISTA_REFERENCE_CONFIG_PROTO_FILE )
			# we only have to copy the reference if the referenced file is different to out config
			set( _ORIG_REFERENCED_FILE )
			local_get_existing_config_filename( ${_PACKAGE_NAME} "${_PACKAGE_ROOT_DIR}" "${${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR}/${_PACKAGE_NAME}Config.cmake" _ORIG_REFERENCED_FILE )
			if( NOT _ORIG_REFERENCED_FILE OR NOT ( "${_ORIG_REFERENCED_FILE}" STREQUAL "${_REFERENCED_FILE}" ) )
				
				#compare contents of files
				execute_process( COMMAND ${CMAKE_COMMAND} -E compare_files ${_ORIG_REFERENCED_FILE} ${_REFERENCED_FILE} RESULT_VARIABLE _ARE_SAME OUTPUT_QUIET ERROR_QUIET )

				if( NOT _ARE_SAME EQUAL 0 )
					# configure the actual reference file
					set( _REFERENCE_TARGET_FILENAME "${${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR}/${_PACKAGE_NAME}Config.cmake" )
					message( STATUS "configuring reference file to \"${VISTA_REFERENCE_CONFIG_PROTO_FILE}\"" )
					configure_file( ${VISTA_REFERENCE_CONFIG_PROTO_FILE} ${_REFERENCE_TARGET_FILENAME} @ONLY )
				endif()
			endif()
									
			
		endif()
	else()
		# since prior configure runs may have already added it (before the cache was turned off), we
		# delete any prior copied versions to this location
		local_clean_old_config_references( ${_PACKAGE_NAME} ${_REFERENCED_FILE} "" )
	endif()
endmacro()

# vista_create_cmake_install( PACKAGE_NAME CONFIG_PROTO_FILE TARGET_DIR )
# configures the specified <package>Config.cmake prototype file, stores it in a temporary
# directory, and adds it to the files to install
# Has to be called after vista_configure_lib and vista_install to work properly
# If the cache variable VISTA_COPY_BUILD_CONFIGS_REFS_TO_CMAKECOMMON is ON -- and
# VISTA_CMAKE_COMMON env var is set -- a reference to this config is installed
# to VISTA_CMAKE_COMMON/share/.
# The following variables are set internally to help configuring the configfile
#     _PACKAGE_NAME          - name of the package
#     _PACKAGE_NAME_UPPER    - uppercase name
#     _PACKAGE_LIBRARY_NAME  - output name of the package (not including Debug Postifix)
#     _PACKAGE_ROOT_DIR      - toplevel file of the package (i.e. directory from which vista_create_cmake_config_build is called)
#     _PACKAGE_LIBRARY_DIRS  - folder where the libraries are installed to
#                              can be overwritten by defining ${_PACKAGE_NAME_UPPER}_LIBRARY_INSTALLDIR before calling the macro
#     _PACKAGE_INCLUDE_DIRS  - folder where the header files are installed to
#                              can be overwritten by defining ${_PACKAGE_NAME_UPPER}_INCLUDE_INSTALLDIR before calling the macro
#     _PACKAGE_RELATIVE_LIBRARY_DIRS - _PACKAGE_LIBRARY_DIRS relative to current dir
#     _PACKAGE_RELATIVE_INCLUDE_DIRS - _PACKAGE_INCLUDE_DIRS relative to current dir
#     _PACKAGE_DEFINITIONS   - definitions for the package, defaults to nothing
#                              can be overwritten by defining ${_PACKAGE_NAME_UPPER}_CONFIG_DEFINITIONS before calling the macro
macro( vista_create_cmake_config_install _PACKAGE_NAME _CONFIG_PROTO_FILE _TARGET_DIR )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )

	# store the directory - it may be used by the versioning lateron
	set( ${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_DIR "${_TARGET_DIR}" )

	#if VISTA_CMAKE_COMMON exisits, we give the user the cache options to toggle copying of references
	# to VISTA_CMAKE_COMMON/share on and off
	if( EXISTS ${VISTA_CMAKE_COMMON} )
		set( VISTA_COPY_INSTALL_CONFIGS_REFS_TO_CMAKECOMMON TRUE CACHE BOOL
			"if enabled, References to <Package>Config.cmake files will be copied to VistaCMakeCommon/share for easier finding" )
	endif( EXISTS ${VISTA_CMAKE_COMMON} )


	# configure the temporary variables for configuring

	set( _PACKAGE_LIBRARY_NAME ${${_PACKAGE_NAME_UPPER}_OUTPUT_NAME} )

	set( _TARGET_FILENAME "${CMAKE_BINARY_DIR}/toinstall/${_PACKAGE_NAME}Config.cmake" )
	set( _TARGET_REF_FILENAME "${CMAKE_BINARY_DIR}/toinstall/references/${_PACKAGE_NAME}Config.cmake" )

	set( _PACKAGE_ROOT_DIR "${CMAKE_INSTALL_PREFIX}" )
	if( ${_PACKAGE_NAME_UPPER}_INCLUDE_INSTALLDIR )
		set( _PACKAGE_INCLUDE_DIRS ${${_PACKAGE_NAME_UPPER}_INCLUDE_INSTALLDIR}  )
	elseif( ${${_PACKAGE_NAME_UPPER}_INC_INSTALLDIR} )
		set( _PACKAGE_INCLUDE_DIRS "${_PACKAGE_ROOT_DIR}/${${_PACKAGE_NAME_UPPER}_INC_INSTALLDIR}" )
	else()
		set( _PACKAGE_INCLUDE_DIRS "${_PACKAGE_ROOT_DIR}/include" )
	endif()
	
	if( ${_PACKAGE_NAME_UPPER}_LIBRARY_INSTALLDIR )
		set( _PACKAGE_LIBRARY_DIRS ${${_PACKAGE_NAME_UPPER}_LIBRARY_INSTALLDIR}  )
	elseif( ${${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR} )
		set( _PACKAGE_LIBRARY_DIRS "${_PACKAGE_ROOT_DIR}/${${_PACKAGE_NAME_UPPER}_LIB_INSTALLDIR}" )
	else()
		set( _PACKAGE_LIBRARY_DIRS "${_PACKAGE_ROOT_DIR}/lib" )
	endif()
	
	set(_PACKAGE_SHADER_DIRS )
	if( ${_PACKAGE_NAME_UPPER}_SHADER_INSTALLDIR )
		set( _PACKAGE_SHADER_DIRS ${${_PACKAGE_NAME_UPPER}_SHADER_INSTALLDIR} )
	endif( ${_PACKAGE_NAME_UPPER}_SHADER_INSTALLDIR )
	
	set(_PACKAGE_DEFINITIONS )
	if( ${_PACKAGE_NAME_UPPER}_CONFIG_DEFINITIONS )
		set( _PACKAGE_DEFINITIONS ${${_PACKAGE_NAME_UPPER}_CONFIG_DEFINITIONS} )
	endif( ${_PACKAGE_NAME_UPPER}_CONFIG_DEFINITIONS )
	
	set( _PACKAGE_MSVC_PROJECT "" )

	#retrieve relative pathes for library/include dirs
	set( _PACKAGE_RELATIVE_INCLUDE_DIRS )
	foreach( _DIR ${_PACKAGE_INCLUDE_DIRS} )
		file( RELATIVE_PATH _REL_DIR "${_PACKAGE_ROOT_DIR}" "${_DIR}" )
		if( _REL_DIR )
			list( APPEND _PACKAGE_RELATIVE_INCLUDE_DIRS "${_REL_DIR}" )
		else( _REL_DIR )
			list( APPEND _PACKAGE_RELATIVE_INCLUDE_DIRS "." )
		endif( _REL_DIR )
	endforeach( _DIR ${_PACKAGE_INCLUDE_DIRS} )

	set( _PACKAGE_RELATIVE_LIBRARY_DIRS )
	foreach( _DIR ${_PACKAGE_LIBRARY_DIRS} )
		file( RELATIVE_PATH _REL_DIR "${_PACKAGE_ROOT_DIR}" "${_DIR}" )
		if( _REL_DIR )
			list( APPEND _PACKAGE_RELATIVE_LIBRARY_DIRS "${_REL_DIR}" )
		else( _REL_DIR )
			list( APPEND _PACKAGE_RELATIVE_LIBRARY_DIRS "." )
		endif( _REL_DIR )
	endforeach( _DIR ${_PACKAGE_LIBRARY_DIRS} )
	
	set( _PACKAGE_RELATIVE_SHADER_DIRS )
	foreach( _DIR ${_PACKAGE_SHADER_DIRS} )
		file( RELATIVE_PATH _REL_DIR "${_PACKAGE_ROOT_DIR}" "${_DIR}" )
		if( _REL_DIR )
			list( APPEND _PACKAGE_RELATIVE_SHADER_DIRS "${_REL_DIR}" )
		else( _REL_DIR )
			list( APPEND _PACKAGE_RELATIVE_SHADER_DIRS "." )
		endif( _REL_DIR )
	endforeach( _DIR ${_PACKAGE_SHADER_DIRS} )

	set( _TEMPORARY_FILENAME "${CMAKE_BINARY_DIR}/toinstall/${_PACKAGE_NAME}Config.cmake" )
	# configure the actual file to a local folder, and add it for install
	configure_file(	"${_CONFIG_PROTO_FILE}" "${_TEMPORARY_FILENAME}" @ONLY )
	install( FILES "${_TEMPORARY_FILENAME}" DESTINATION "${${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_DIR}"
				PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC} )



	set( _REFERENCED_FILE "${${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_DIR}/${_PACKAGE_NAME}Config.cmake" )


	if( VISTA_COPY_INSTALL_CONFIGS_REFS_TO_CMAKECOMMON )
		# find proto file
		find_file( VISTA_REFERENCE_CONFIG_PROTO_FILE "PackageConfigReference.cmake_proto" PATH ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH} )
		set( VISTA_REFERENCE_CONFIG_PROTO_FILE ${VISTA_REFERENCE_CONFIG_PROTO_FILE} CACHE INTERNAL "" FORCE )

		#determine dir (and store it for later use of versions)
		set( ${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_REFERENCE_DIR "${VISTA_CMAKE_COMMON}/share/${_PACKAGE_NAME}" )
		if( DEFINED ${_PACKAGE_NAME_UPPER}_VERSION_EXT )
			set( ${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_REFERENCE_DIR
					"${${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_REFERENCE_DIR}-${${_PACKAGE_NAME_UPPER}_VERSION_EXT}-install" )
		else( DEFINED ${_PACKAGE_NAME_UPPER}_VERSION_EXT )
			set( ${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_REFERENCE_DIR
					"${${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_REFERENCE_DIR}-${VISTA_HWARCH}-install" )
		endif( DEFINED ${_PACKAGE_NAME_UPPER}_VERSION_EXT )

		if( VISTA_REFERENCE_CONFIG_PROTO_FILE )
			#eliminate older installed configs
			local_clean_old_config_references( "${_PACKAGE_NAME}" "${_REFERENCED_FILE}" "${${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_REFERENCE_DIR}" )
			# configure the reference file
			set( _TEMPORARY_REF_FILENAME "${CMAKE_BINARY_DIR}/toinstall/references/${_PACKAGE_NAME}Config.cmake" )
			configure_file(	"${VISTA_REFERENCE_CONFIG_PROTO_FILE}" "${_TARGET_REF_FILENAME}" @ONLY )
			install( FILES "${_TARGET_REF_FILENAME}" 
					DESTINATION "${${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_REFERENCE_DIR}"
					PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC} )
		endif( VISTA_REFERENCE_CONFIG_PROTO_FILE )
	else( VISTA_COPY_INSTALL_CONFIGS_REFS_TO_CMAKECOMMON )
		# since prior configure runs may have already added it (before the cache was turned off), we
		# delete any prior copied versions to this location
		local_clean_old_config_references( ${_PACKAGE_NAME} "${_REFERENCED_FILE}" "" )
	endif( VISTA_COPY_INSTALL_CONFIGS_REFS_TO_CMAKECOMMON )
endmacro( vista_create_cmake_config_install )

# vista_create_version_config( PACKAGE_NAME VERSION_PROTO_FILE )
# configures the specified <package>ConfigVersion.cmake prototype file.
# for this to work, the version variables have to be set (e.g. using vistaa_set_version),
# at least one of vista_create_cmake_config_build or vista_create_cmake_config_install
# has to be performed
# the version files are placed at the same location as the created config files
# If the cache variable VISTA_COPY_BUILD_CONFIGS_REFS_TO_CMAKECOMMON is ON -- and
# VISTA_CMAKE_COMMON env var is set -- references are created too
macro( vista_create_version_config _PACKAGE_NAME _VERSION_PROTO_FILE )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )

	find_file( VISTA_REFERENCE_CONFIG_PROTO_FILE "PackageConfigReference.cmake_proto" PATH ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH} )
	set( VISTA_REFERENCE_CONFIG_PROTO_FILE ${VISTA_REFERENCE_CONFIG_PROTO_FILE} CACHE INTERNAL "" FORCE )

	set( _PACKAGE_LIBRARY_NAME ${${_PACKAGE_NAME_UPPER}_OUTPUT_NAME} )

	if( EXISTS ${_VERSION_PROTO_FILE} )
		set( _VERSION_TYPE 	${${_PACKAGE_NAME_UPPER}_VERSION_TYPE} )
		set( _VERSION_NAME 	${${_PACKAGE_NAME_UPPER}_VERSION_NAME} )
		set( _VERSION_MAJOR ${${_PACKAGE_NAME_UPPER}_VERSION_MAJOR} )
		set( _VERSION_MINOR ${${_PACKAGE_NAME_UPPER}_VERSION_MINOR} )
		set( _VERSION_PATCH ${${_PACKAGE_NAME_UPPER}_VERSION_PATCH} )
		set( _VERSION_TWEAK ${${_PACKAGE_NAME_UPPER}_VERSION_TWEAK} )
		set( _VERSION 		${${_PACKAGE_NAME_UPPER}_VERSION} )
		set( _VERSION_EXT 	${${_PACKAGE_NAME_UPPER}_VERSION_EXT} )

		if( ${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_DIR )
			set( _BUILD_VERSION_TARGET "${${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_DIR}/${_PACKAGE_NAME}ConfigVersion.cmake" )
			configure_file( "${_VERSION_PROTO_FILE}" "${_BUILD_VERSION_TARGET}" @ONLY )

			if( VISTA_COPY_BUILD_CONFIGS_REFS_TO_CMAKECOMMON )
				set( _REFERENCED_FILE "${_BUILD_VERSION_TARGET}" )
				set( _REFERENCE_TARGET_FILENAME "${${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_REFERENCE_DIR}/${_PACKAGE_NAME}ConfigVersion.cmake" )
				configure_file( "${VISTA_REFERENCE_CONFIG_PROTO_FILE}" "${_REFERENCE_TARGET_FILENAME}" @ONLY )
			endif( VISTA_COPY_BUILD_CONFIGS_REFS_TO_CMAKECOMMON )
		endif( ${_PACKAGE_NAME_UPPER}_BUILD_CONFIG_DIR )

		if( ${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_DIR )
			set( _TEMPORARY_FILENAME "${CMAKE_BINARY_DIR}/toinstall/${_PACKAGE_NAME}ConfigVersion.cmake" )
			set( _INSTALL_DIR  "${${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_DIR}" )
			configure_file( "${_VERSION_PROTO_FILE}" "${_TEMPORARY_FILENAME}" @ONLY )
			install( FILES "${_TEMPORARY_FILENAME}" DESTINATION "${_INSTALL_DIR}" PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC} )

			if( VISTA_COPY_INSTALL_CONFIGS_REFS_TO_CMAKECOMMON )
				set( _REFERENCED_FILE "${_INSTALL_DIR}/${_PACKAGE_NAME}ConfigVersion.cmake" )
				set( _REFERENCE_TEMPORARY_FILENAME "${CMAKE_BINARY_DIR}/toinstall/references/${_PACKAGE_NAME}ConfigVersion.cmake" )
				configure_file( "${VISTA_REFERENCE_CONFIG_PROTO_FILE}" "${_REFERENCE_TEMPORARY_FILENAME}" @ONLY )
				install( FILES "${_REFERENCE_TEMPORARY_FILENAME}"
						DESTINATION "${${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_REFERENCE_DIR}"
						PERMISSIONS ${VISTA_INSTALL_PERMISSIONS_NONEXEC} )
			endif( VISTA_COPY_INSTALL_CONFIGS_REFS_TO_CMAKECOMMON )
		endif( ${_PACKAGE_NAME_UPPER}_INSTALL_CONFIG_DIR )

	endif( EXISTS ${_VERSION_PROTO_FILE} )
endmacro( vista_create_version_config )

# vista_create_cmake_configs( TARGET [CUSTOM_CONFIG_FILE_BUILD [CUSTOM_CONFIG_FILE_INSTALL] ] )
# can only be called after vista_configure_[app|lib]
# generates XYZConfig.cmake-files for the target, either from a generic prototype or
# from the optional specified one. Each configfile is created twice: one for the build version, and one
# for the install version, which point to different locations
# If the VISTA_CMAKE_ROOT environment variable is set, the XYZConfig.cmake files will also be copied to
# VISTA_CMAKE_ROOT/share into a subfolder composed from the name, the (optional) version, and either -build or -install
# NOTE: these will be overwritten at the next configure/install, so make sure different versions of the same project
# have different version names
# In Addition to the XYZConfig.cmake files, a generic XYZConfigVersion.cmake file is created if the version has been specified
# using vista_set_version() or vista_adopt_version(), in the same way as the Config files
macro( vista_create_cmake_configs _TARGET )
	set( _PACKAGE_NAME ${_TARGET} )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
	set( _PRECONDITION_FAIL false )

	if( ${ARGC} GREATER 1 )
		if( EXISTS ${ARGV1} )
			#use for custom config file
			set( _CONFIG_PROTO_FILE_BUILD ${ARGV1} )
			if( ${ARGC} GREATER 2 )
				if( EXISTS ${ARGV2} )
					set( _CONFIG_PROTO_FILE_INSTALL ${ARGV2} )
				else( EXISTS ${ARGV2} )
					message( WARNING "vista_create_cmake_configs( ${_TARGET} ) - Could not find config file \"${ARGV2}\"" )
					set( _PRECONDITION_FAIL TRUE )
				endif( EXISTS ${ARGV2} )
			else()
				set( _CONFIG_PROTO_FILE_INSTALL ${ARGV1} )
			endif( ${ARGC} GREATER 2 )
		else( EXISTS ${ARGV1} )
			message( WARNING "vista_create_cmake_configs( ${_TARGET} ) - Could not find config file \"${ARGV1}\"" )
			set( _PRECONDITION_FAIL TRUE )
		endif( EXISTS ${ARGV1} )
	else( ${ARGC} GREATER 1 )
		#use default config file
		find_file( VISTA_DEFAULT_CONFIG_PROTO_FILE_BUILD "PackageConfig-build.cmake_proto" PATHS ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH} )
		set( VISTA_DEFAULT_CONFIG_PROTO_FILE_BUILD "${VISTA_DEFAULT_CONFIG_PROTO_FILE_BUILD}" CACHE INTERNAL "Default Prototype file for <Package>Config.cmake in build config" FORCE )
		find_file( VISTA_DEFAULT_CONFIG_PROTO_FILE_INSTALL "PackageConfig-install.cmake_proto" PATHS ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH} )
		set( VISTA_DEFAULT_CONFIG_PROTO_FILE_INSTALL "${VISTA_DEFAULT_CONFIG_PROTO_FILE_INSTALL}" CACHE INTERNAL "Default Prototype file for <Package>Config.cmake in install config" FORCE )

		if( NOT VISTA_DEFAULT_CONFIG_PROTO_FILE_BUILD OR NOT VISTA_DEFAULT_CONFIG_PROTO_FILE_INSTALL )
			message( WARNING "vista_create_cmake_configs( ${_TARGET} ) - Could not find default config file PackageConfig.cmake_proto" )
			set( _PRECONDITION_FAIL TRUE )
		endif( NOT VISTA_DEFAULT_CONFIG_PROTO_FILE_BUILD OR NOT VISTA_DEFAULT_CONFIG_PROTO_FILE_INSTALL )
		set( _CONFIG_PROTO_FILE_BUILD "${VISTA_DEFAULT_CONFIG_PROTO_FILE_BUILD}" )
		set( _CONFIG_PROTO_FILE_INSTALL "${VISTA_DEFAULT_CONFIG_PROTO_FILE_INSTALL}" )
	endif( ${ARGC} GREATER 1 )


	if( NOT _PRECONDITION_FAIL )
		vista_create_cmake_config_build( ${_PACKAGE_NAME}
											"${_CONFIG_PROTO_FILE_BUILD}"
											"${CMAKE_BINARY_DIR}/cmake" )
		vista_create_cmake_config_install( ${_PACKAGE_NAME}
											"${_CONFIG_PROTO_FILE_INSTALL}"
											"${CMAKE_INSTALL_PREFIX}/share/${_PACKAGE_NAME}/cmake" )

		# we create a PackageConfigVersion.cmake file to check the version. we even do so if no version is given
		# for the package - in this case, it is still helpful in order to check a compatible
		# build type
		find_file( VISTA_VERSION_PROTO_FILE "PackageConfigVersion.cmake_proto" PATHS ${CMAKE_MODULE_PATH} )
		set( VISTA_VERSION_PROTO_FILE "${VISTA_VERSION_PROTO_FILE}" CACHE INTERNAL "" )
		if( VISTA_VERSION_PROTO_FILE )
			vista_create_version_config( ${_PACKAGE_NAME} "${VISTA_VERSION_PROTO_FILE}"
										"${CMAKE_BINARY_DIR}/cmake/${_PACKAGE_NAME}ConfigVersion.cmake" )
		endif( VISTA_VERSION_PROTO_FILE )

	endif( NOT _PRECONDITION_FAIL )
endmacro( vista_create_cmake_configs )

# vista_set_outdir( TARGET DIRECTORY [USE_CONFIG_SUBDIRS])
# sets the outdir of the target to the directory
# should be used after calling vista_configuer_[app|lib]
#  if USE_CONFIG_SUBDIRS is added, a postfix will be set for each BuildType (Debug, Release, RelWithDebInfo, ...)
macro( vista_set_outdir _PACKAGE_NAME _TARGET_DIR )
	string( TOUPPER  ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
	if( ${ARGC} GREATER 2 AND "${ARGV2}" STREQUAL "USE_CONFIG_SUBDIRS" )
		# use subdirs
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${_TARGET_DIR}/Debug" )
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_TARGET_DIR}/Release" )
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${_TARGET_DIR}/MinSizeRel" )
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_TARGET_DIR}/RelWithDebInfo" )
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${_TARGET_DIR}" )
		if( NOT VISTA_${_PACKAGE_NAME_UPPER}_TARGET_TYPE OR VISTA_${_PACKAGE_NAME_UPPER}_TARGET_TYPE STREQUAL "LIB" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${_TARGET_DIR}/Debug" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${_TARGET_DIR}/Release" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${_TARGET_DIR}/MinSizeRel" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_TARGET_DIR}/RelWithDebInfo" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${_TARGET_DIR} ")
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${_TARGET_DIR}/Debug" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${_TARGET_DIR}/Release" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${_TARGET_DIR}/MinSizeRel" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_TARGET_DIR}/RelWithDebInfo" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${_TARGET_DIR}" )
		endif()
		set( ${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR_WITH_CONFIG_SUBDIRS TRUE CACHE INTERNAL "" FORCE )
	else()
		# dont use subdirs
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${_TARGET_DIR} )
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${_TARGET_DIR} )
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${_TARGET_DIR} )
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${_TARGET_DIR} )
		set_target_properties( ${_PACKAGE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${_TARGET_DIR} )
		if( NOT VISTA_${_PACKAGE_NAME_UPPER}_TARGET_TYPE OR VISTA_${_PACKAGE_NAME_UPPER}_TARGET_TYPE STREQUAL "LIB" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${_TARGET_DIR}" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${_TARGET_DIR}" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${_TARGET_DIR}" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_TARGET_DIR}" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${_TARGET_DIR}" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${_TARGET_DIR}" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${_TARGET_DIR}" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${_TARGET_DIR}" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_TARGET_DIR}" )
			set_target_properties( ${_PACKAGE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${_TARGET_DIR}" )
		endif()
		set( ${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR_WITH_CONFIG_SUBDIRS FALSE CACHE INTERNAL "" FORCE )
	endif()
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )

	set( ${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR "${_TARGET_DIR}" CACHE INTERNAL "" FORCE )
endmacro()

# vista_set_version( PACKAGE TYPE NAME [ MAJOR [ MINOR [ PATCH [ TWEAK ]]]] )
# sets the extended version info for the package
# TYPE has to be RELEASE, HEAD, BRANCH, or TAG
# NAME can be an arbitrary name (excluding character -)
# MAJOR, MINOR, PATCH, TWEAK are optional version numbers. If svn_rev is specified, an svn revision is extracted if possible
# the macro defines the following
# <PACKAGE>_VERSION_EXT
# <PACKAGE>_VERSION_TYPE
# <PACKAGE>_VERSION_NAME
# <PACKAGE>_VERSION_MAJOR
# <PACKAGE>_VERSION_MINOR
# <PACKAGE>_VERSION_PATCH
# <PACKAGE>_VERSION_TWEAK
# <PACKAGE>_VERSION_POSIX
macro( vista_set_version _PACKAGE _TYPE _NAME )
	
	string( TOUPPER  ${_PACKAGE} _PACKAGE_UPPER )
	set( ${_PACKAGE_UPPER}_VERSION_TYPE		${_TYPE} )
	set( ${_PACKAGE_UPPER}_VERSION_NAME		${_NAME} )

	if( ${ARGC} GREATER 3 )
		if( ${ARGV3} STREQUAL "svn_rev" )
			vista_find_package( SVN QUIET )
			vista_get_svn_revision( ${_PACKAGE_UPPER}_VERSION_MAJOR )
		else( ${ARGV3} STREQUAL "svn_rev" )
			set( ${_PACKAGE_UPPER}_VERSION_MAJOR ${ARGV3} )
		endif( ${ARGV3} STREQUAL "svn_rev" )
		set( ${_PACKAGE_UPPER}_VERSION			"${${_PACKAGE_UPPER}_VERSION_MAJOR}" )
	endif( ${ARGC} GREATER 3 )
	if( ${ARGC} GREATER 4 )
		if( ${ARGV4} STREQUAL "svn_rev" )
			vista_find_package( SVN QUIET )
			vista_get_svn_revision( ${_PACKAGE_UPPER}_VERSION_MINOR )
		else( ${ARGV4} STREQUAL "svn_rev" )
			set( ${_PACKAGE_UPPER}_VERSION_MINOR ${ARGV4} )
		endif( ${ARGV4} STREQUAL "svn_rev" )
		set( ${_PACKAGE_UPPER}_VERSION			"${${_PACKAGE_UPPER}_VERSION}.${${_PACKAGE_UPPER}_VERSION_MINOR}" )
	endif( ${ARGC} GREATER 4 )
	if( ${ARGC} GREATER 5 )
		if( ${ARGV5} STREQUAL "svn_rev" )
			vista_find_package( SVN QUIET )
			vista_get_svn_revision( ${_PACKAGE_UPPER}_VERSION_PATCH )
		else( ${ARGV5} STREQUAL "svn_rev" )
			set( ${_PACKAGE_UPPER}_VERSION_PATCH ${ARGV5} )
		endif( ${ARGV5} STREQUAL "svn_rev" )
		set( ${_PACKAGE_UPPER}_VERSION			"${${_PACKAGE_UPPER}_VERSION}.${${_PACKAGE_UPPER}_VERSION_PATCH}" )
	endif( ${ARGC} GREATER 5 )
	if( ${ARGC} GREATER 6 )
		if( ${ARGV6} STREQUAL "svn_rev" )
			vista_find_package( SVN QUIET )
			vista_get_svn_revision( ${_PACKAGE_UPPER}_VERSION_TWEAK )
		else( ${ARGV6} STREQUAL "svn_rev" )
			set( ${_PACKAGE_UPPER}_VERSION_TWEAK 	${ARGV6} )
		endif( ${ARGV6} STREQUAL "svn_rev" )
		set( ${_PACKAGE_UPPER}_VERSION			"${${_PACKAGE_UPPER}_VERSION}.${${_PACKAGE_UPPER}_VERSION_TWEAK}" )
	endif( ${ARGC} GREATER 6 )

	set( ${_PACKAGE_UPPER}_VERSION_EXT			"${${_PACKAGE_UPPER}_VERSION_TYPE}_${${_PACKAGE_UPPER}_VERSION_NAME}" )
	if( DEFINED ${_PACKAGE_UPPER}_VERSION )
		set( ${_PACKAGE_UPPER}_VERSION_EXT		"${${_PACKAGE_UPPER}_VERSION_EXT}-${${_PACKAGE_UPPER}_VERSION}" )
	endif( DEFINED ${_PACKAGE_UPPER}_VERSION )
	set( ${_PACKAGE_UPPER}_VERSION_EXT			"${${_PACKAGE_UPPER}_VERSION_EXT}-${VISTA_HWARCH}" )

	if( NOT ${_PACKAGE_UPPER}_VERSION_MAJOR )
		set( ${_PACKAGE_UPPER}_VERSION_MAJOR 1 )
	endif()
	if( NOT ${_PACKAGE_UPPER}_VERSION_MINOR )
		set( ${_PACKAGE_UPPER}_VERSION_MINOR 0 )
	endif()
	if( NOT ${_PACKAGE_UPPER}_VERSION_PATCH )
		set( ${_PACKAGE_UPPER}_VERSION_PATCH 0 )
	endif()
	set( ${_PACKAGE_UPPER}_VERSION_POSIX "${${_PACKAGE_UPPER}_VERSION_MAJOR}.${${_PACKAGE_UPPER}_VERSION_MINOR}.${${_PACKAGE_UPPER}_VERSION_PATCH}" )

endmacro( vista_set_version _PACKAGE _TYPE _NAME )

# vista_adopt_version( PACKAGE ADOPT_PARENT )
# sets the version of the package to the one of the adopt parent
macro( vista_adopt_version _NAME _ADOPT_PARENT )
	string( TOUPPER ${_NAME} _NAME_UPPER )
	string( TOUPPER ${_ADOPT_PARENT} _ADOPT_UPPER )

	if( ${_ADOPT_UPPER}_VERSION_EXT )
		set( ${_NAME_UPPER}_VERSION_TYPE		${${_ADOPT_UPPER}_VERSION_TYPE} )
		set( ${_NAME_UPPER}_VERSION_NAME		${${_ADOPT_UPPER}_VERSION_NAME} )
		set( ${_NAME_UPPER}_VERSION				${${_ADOPT_UPPER}_VERSION} )
		set( ${_NAME_UPPER}_VERSION_EXT			${${_ADOPT_UPPER}_VERSION_EXT} )
		set( ${_NAME_UPPER}_VERSION_MAJOR		${${_ADOPT_UPPER}_VERSION_MAJOR} )
		set( ${_NAME_UPPER}_VERSION_MINOR		${${_ADOPT_UPPER}_VERSION_MINOR} )
		set( ${_NAME_UPPER}_VERSION_PATCH		${${_ADOPT_UPPER}_VERSION_PATCH} )
		set( ${_NAME_UPPER}_VERSION_TWEAK		${${_ADOPT_UPPER}_VERSION_TWEAK} )
		set( ${_NAME_UPPER}_VERSION_POSIX		${${_ADOPT_UPPER}_VERSION_POSIX} )
	else( ${_ADOPT_UPPER}_VERSION_EXT )
		message( WARNING "vista_adopt_version( ${_NAME} ${_ADOPT_PARENT} ) - cannot find version info for parent!" )
	endif( ${_ADOPT_UPPER}_VERSION_EXT )
endmacro( vista_adopt_version _NAME _ADOPT_PARENT )

# vista_create_default_info_file( PACKAGE_NAME )
# uses the cache variable VISTA_CREATE_BUILD_INFO_FILES to determine
# if a build info file should be created, and if so, creates it next to the lib/app,
# and installs it to .../share/VistaBuildInfo
macro( vista_create_default_info_file _PACKAGE_NAME )
	set( VISTA_CREATE_BUILD_INFO_FILES TRUE CACHE BOOL "If enabled, an auto-generated build info file will be generated and installed for each target" )
	if( VISTA_CREATE_BUILD_INFO_FILES )
		string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
		vista_create_info_file( ${_PACKAGE_NAME} "${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR}" "${CMAKE_INSTALL_PREFIX}/share/VistaBuildInfo" )
	else()
		vista_delete_info_file( ${_PACKAGE_NAME} "${${_PACKAGE_NAME_UPPER}_TARGET_OUTDIR}" )
	endif( VISTA_CREATE_BUILD_INFO_FILES )
endmacro( vista_create_default_info_file )

# vista_create_doxygen_target( DOXYFILE [WORKING_DIR] )
# adds a target for creating doxygen info
# only works if Doxygen can be found on the system. If successfull, doxygen can be
# creating by running the "Doxygen" project in MSVC or by calling make Doxygen
# The path to the Doxy-File has to be absolute!
# Doxygen is run in either the localion specified by the optional parameter WORKING_DIR, or 
# by default in the same location as the doxyfile. This means, the OUTPUT_DIRECTORY and INPUT targets
# specified inside the Doxyfile are relative to this working dir
macro( vista_create_doxygen_target _DOXYFILE )
	find_package( Doxygen )
	if( NOT DOXYGEN_FOUND )
		message( STATUS "vista_create_doxygen - Doxygen executable not found - cant create doxygen target" )
	else()
		if( ${ARGC} GREATER 1 )
			set( _WORKDIR ${ARGV1} )
		else()
			get_filename_component( _WORKDIR "${_DOXYFILE}" PATH )
		endif( ${ARGC} GREATER 1 )
		if( NOT EXISTS _WORKDIR )
			file( MAKE_DIRECTORY "${_WORKDIR}" )
		endif( NOT EXISTS _WORKDIR )
		add_custom_target( Doxygen
			"${DOXYGEN_EXECUTABLE}" "${_DOXYFILE}"
			WORKING_DIRECTORY "${_WORKDIR}"
			COMMENT "Generating API documentation with Doxygen"
		)
		set_target_properties( Doxygen PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE )
	endif( NOT DOXYGEN_FOUND )
endmacro( vista_create_doxygen_target )

# vista_create_uninstall_target( [ON|OFF] )
# sets a cache variable VISTA_ALLOW_UNINSTALL, with default value of argument (or OFF if no argument is given)
# if VISTA_ALLOW_UNINSTALL is ON, an uninstall target will be created, which removes all previously installed files.
# WARNING: this may accidently remove files that might still be needed - use with care
# Also, the uninstall may leave behind empty directories
macro( vista_create_uninstall_target )
	if( ${ARGC} GREATER 1 )
		set( _DEFAULT ${ARGV0} )
	else()
		set( _DEFAULT "OFF" )
	endif( ${ARGC} GREATER 1 )
	set( VISTA_ALLOW_UNINSTALL ${_DEFAULT} CACHE BOOL "In enabled, an uninstall project will be created. Use at your own risk - may remove wrong files!" )
	if( VISTA_ALLOW_UNINSTALL )
		find_file( VISTA_CMAKE_UNINSTALL_PROTO_FILE "cmake_uninstall.cmake_proto" PATHS ${CMAKE_MODULE_PATH} )
		if( NOT VISTA_CMAKE_UNINSTALL_PROTO_FILE )
			message( AUTHOR_WARNING "cant find cmake_uninstall proto file - uninstall target will not be created." )
		else()
			configure_file( "${VISTA_CMAKE_UNINSTALL_PROTO_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY )
			if( WIN32 )
				add_custom_target( UNINSTALL "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" )
				set_target_properties( UNINSTALL PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE )
				set_target_properties( UNINSTALL PROPERTIES FOLDER "CMakePredefinedTargets" )
			else()
				add_custom_target( uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" )
				set_target_properties( uninstall PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE )
			endif( WIN32 )

		endif( NOT VISTA_CMAKE_UNINSTALL_PROTO_FILE )
	endif( VISTA_ALLOW_UNINSTALL )
endmacro( vista_create_uninstall_target )

# vista_set_install_permissions( [OWNER_READ] [OWNER_WRITE] [OWNER_EXECUTE] [GROUP_READ] [GROUP_WRITE] [GROUP_EXECUTE] [WORLD_READ] [WORLD_WRITE] [WORLD_EXECUTE] [SETUID] [SETGID] )
# sets the default permissions for installed files (using VistaCMakeCommon-commands, still needs to be set
# manually for manually called install() )
# defines two variables VISTA_INSTALL_PERMISSIONS_EXEC and VISTA_INSTALL_PERMISSIONS_NONEXEC, where
# the latter conforms to the passed permission parameters minus the executable options
macro( vista_set_install_permissions )
	set( VISTA_INSTALL_PERMISSIONS_NONEXEC ${ARGN} )
	set( VISTA_INSTALL_PERMISSIONS_EXEC ${ARGN} )
	list( REMOVE_ITEM VISTA_INSTALL_PERMISSIONS_NONEXEC OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE ) 
endmacro()

# vista_configure_cpack ( _PACKAGE_NAME )
# Configures common cpack parameters.
# Sets default package type to tgz / zip.
# Calls default VistaCMakeCommon/VistaCPackConfig.cmake, if there is no local CPackConfig.cmake defined.
# IMPORTANT :
# vista_configure_app/lib should be done before (defines app / lib type, lib is assumed else)!
# Any install commands have to be done afterwards !
# Conflicts with vista_create_cmake_configs if archive packages with relative path are used !
macro( vista_configure_cpack _PACKAGE_NAME )
	set( VISTA_BUILD_PACKS OFF CACHE BOOL "Build various packages from libs" )
	if( VISTA_BUILD_PACKS )
		# Hand over anything that may be useful to CPack

		if( UNIX )
			set( VISTA_CPACK_GENERATOR "TGZ" CACHE STRING "CPack Generator type, see CPack documentation for acceptable values." )
		elseif( WIN32 )
			set( VISTA_CPACK_GENERATOR "ZIP" CACHE STRING "CPack Generator type, see CPack documentation for acceptable values." )
		endif()

		# Setting this on command line expands to absoule path.
		# Archive packagers need relative path !
		if( ${VISTA_CPACK_GENERATOR} STREQUAL "TGZ" )
			if( VISTA_64BIT )
				set( CMAKE_INSTALL_PREFIX "LINUX.X86_64" CACHE PATH "distribution directory" FORCE )
			else()
				set( CMAKE_INSTALL_PREFIX "LINUX.X86" CACHE PATH "distribution directory" FORCE )
			endif()
		elseif( ${VISTA_CPACK_GENERATOR} STREQUAL "ZIP" )
			if( MSVC11 )
				set( VISTA_MSVC_VERSION 11 )
			elseif( MSVC10 )
				set( VISTA_MSVC_VERSION 10 )
			elseif( MSVC12 )
				set( VISTA_MSVC_VERSION 12 )
			elseif( MSVC14 )
				set( VISTA_MSVC_VERSION 14 )
			endif()
			if( VISTA_64BIT )
				set( CMAKE_INSTALL_PREFIX "win32-x64.vc${VISTA_MSVC_VERSION}" CACHE PATH "distribution directory" FORCE )
			else()
				set( CMAKE_INSTALL_PREFIX "win32.vc${VISTA_MSVC_VERSION}" CACHE PATH "distribution directory" FORCE )
			endif()
		elseif( ${VISTA_CPACK_GENERATOR} STREQUAL "RPM" )
			set( CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "distribution directory" )
		endif()

		# Make sure RPath is turned off for installation
		set( VISTA_USE_RPATH OFF CACHE BOOL "Automatically set the rpath for external libs" FORCE )

		# Don't copy release build info
		set( VISTA_COPY_INSTALL_CONFIGS_REFS_TO_CMAKECOMMON FALSE CACHE BOOL
			"if enabled, References to <Package>Config.cmake files will be copied to VistaCMakeCommon/share for easier finding" FORCE )
		
		string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
		
		if( DEFINED ${_PACKAGE_NAME_UPPER}_TARGET_TYPE )
			set( CPACK_TARGET_TYPE ${_PACKAGE_NAME_UPPER}_TARGET_TYPE )
		else()
			set( CPACK_TARGET_TYPE "LIB" )
		endif()

		if( DEFINED ${_PACKAGE_NAME_UPPER}_VERSION_MAJOR )
			set( CPACK_PACKAGE_VERSION_MAJOR ${${_PACKAGE_NAME_UPPER}_VERSION_MAJOR} )
		else()
			set( CPACK_PACKAGE_VERSION_MAJOR 1 )
		endif()
		if( DEFINED ${_PACKAGE_NAME_UPPER}_VERSION_MINOR )
			set( CPACK_PACKAGE_VERSION_MINOR ${${_PACKAGE_NAME_UPPER}_VERSION_MINOR} )
		else()
			set( CPACK_PACKAGE_VERSION_MINOR 0 )
		endif()
		
		if( DEFINED ${_PACKAGE_NAME_UPPER}_VERSION_PATCH )
			set( CPACK_PACKAGE_VERSION_PATCH ${${_PACKAGE_NAME_UPPER}_VERSION_PATCH} )
		else()
			set( CPACK_PACKAGE_VERSION_PATCH 0 )
		endif()

		set( CPACK_CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
		set( CPACK_CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} )
		set( CPACK_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} )

		# Define what kind of packages should be build
		set( CPACK_GENERATOR ${VISTA_CPACK_GENERATOR} )
		
		# Generator dependant setup, prefer local config
		if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake" )
			set( CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake" )
		else()
			set( CPACK_PROJECT_CONFIG_FILE "${VISTA_CMAKE_COMMON}/VistaCPackConfig.cmake" )
		endif()

		include(CPack)
	endif()
endmacro()



###########################
###   General Settings  ###
###########################

set( VISTACMAKECOMMON_REVISION "$Rev$" )
string( REGEX MATCH " ([0-9]+) " _MATCH "${VISTACMAKECOMMON_REVISION}" )
set( VISTACMAKECOMMON_REVISION ${CMAKE_MATCH_1} )

vista_set_defaultvalue( BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries if ON, static libraries if OFF" FORCE )

# if VISTA_CMAKE_COMMON envvar is set, we buffer it and add it to CMAKE_MODULE_PATH and CMAKE_PREFIX_PATH
if( EXISTS "$ENV{VISTA_CMAKE_COMMON}" )
	file( TO_CMAKE_PATH $ENV{VISTA_CMAKE_COMMON} VISTA_CMAKE_COMMON )
	
	list( APPEND CMAKE_MODULE_PATH "${VISTA_CMAKE_COMMON}" "${VISTA_CMAKE_COMMON}/share" )
	list( APPEND CMAKE_PREFIX_PATH "${VISTA_CMAKE_COMMON}" "${VISTA_CMAKE_COMMON}/share" )
	list( REMOVE_DUPLICATES CMAKE_MODULE_PATH )
	list( REMOVE_DUPLICATES CMAKE_PREFIX_PATH )
endif( EXISTS "$ENV{VISTA_CMAKE_COMMON}" )

if( NOT ALREADY_CONFIGURED_ONCE OR FIRST_CONFIGURE_RUN )
	set( ALREADY_CONFIGURED_ONCE TRUE CACHE INTERNAL "defines if this is the first config run or not" )
	set( FIRST_CONFIGURE_RUN TRUE )
else( NOT ALREADY_CONFIGURED_ONCE OR FIRST_CONFIGURE_RUN )
	set( FIRST_CONFIGURE_RUN FALSE )
endif( NOT ALREADY_CONFIGURED_ONCE OR FIRST_CONFIGURE_RUN )

# general settings/flags
set( CMAKE_DEBUG_POSTFIX "D" )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG" )

if( UNIX )
	# Should we use rpath? This enables us to use OpenSG etc. within the Vista* libraries without having
	# to set a LIBRARY_PATH while linking against these libraries
	set( VISTA_USE_RPATH ON CACHE BOOL "Automatically set the rpath for external libs" )
	if( VISTA_USE_RPATH )
		set( CMAKE_SKIP_BUILD_RPATH FALSE )
		set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
		set( CMAKE_SKIP_RPATH FALSE )
	else( VISTA_USE_RPATH )
		set( CMAKE_SKIP_RPATH TRUE )
	endif( VISTA_USE_RPATH )
endif( UNIX )

# Platform dependent definitions
add_definitions( ${VISTA_PLATFORM_DEFINE} ) # adds -DWIN32 / -DLINUX or similar


if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
	vista_set_defaultvalue( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/dist/${VISTA_HWARCH}" CACHE PATH "distribution directory" FORCE )
	set( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT FALSE )
endif( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )

if( WIN32 )
	if( MSVC )
		vista_set_defaultvalue( CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "CMake configuration types" FORCE )
		# msvc disable some warnings
		set( VISTA_DISABLE_GENERIC_MSVC_WARNINGS ON CACHE BOOL "If true, generic warnings (4251, 4275, 4503, CRT_SECURE_NO_WARNINGS) will be disabled for Visual Studio" )
		if( VISTA_DISABLE_GENERIC_MSVC_WARNINGS )
			add_definitions( /D_CRT_SECURE_NO_WARNINGS /wd4251 /wd4275 /wd4503 )
		endif()
		#Enable string pooling
		add_definitions( -GF )
		# Parallel build for Visual Studio?
		set( VISTA_USE_PARALLEL_MSVC_BUILD ON CACHE BOOL "Add /MP flag for parallel build on Visual Studio" )
		if( VISTA_USE_PARALLEL_MSVC_BUILD )
			add_definitions( /MP )
		else()
			remove_definitions(/MP)
		endif()
	endif()
endif()

# we once parse the referenced configs in VISTA_CMAKE_COMMON to remove outdated ones
if( FIRST_CONFIGURATION_RUN AND EXISTS "${VISTA_CMAKE_COMMON}" AND NOT VISTA_CHECKED_COPIED_CONFIG_FILES )
	set( VISTA_CHECKED_COPIED_CONFIG_FILES TRUE )
	set( PACKAGE_REFERENCE_EXISTS_TEST TRUE )
	file( GLOB_RECURSE _ALL_VERSION_FILES "${VISTA_CMAKE_COMMON}/share/*Config.cmake" )
	foreach( _FILE ${_ALL_VERSION_FILES} )
		set( PACKAGE_REFERENCE_OUTDATED FALSE )
		include( ${_FILE} )
		if( PACKAGE_REFERENCE_OUTDATED )
			get_filename_component( _DIR ${_FILE} PATH )
			message( STATUS "Removing outdated config reference copied to \"${_DIR}\"" )
			file( REMOVE_RECURSE ${_DIR} )
		endif( PACKAGE_REFERENCE_OUTDATED )
	endforeach( _FILE ${_ALL_VERSION_FILES} )
	set( PACKAGE_REFERENCE_EXISTS_TEST FALSE )
endif()

set( VISTACOMMON_FILE_LOCATION "VISTACOMMON_FILE_LOCATION-NOTFOUND" CACHE INTERNAL "" FORCE )
find_file( VISTACOMMON_FILE_LOCATION "VistaCommon.cmake" PATHS ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH} NO_DEFAULT_PATH )
set( VISTACOMMON_FILE_LOCATION ${VISTACOMMON_FILE_LOCATION} CACHE INTERNAL "" FORCE )

vista_set_install_permissions( OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

set( CMAKE_SUPPRESS_REGENERATION "${CMAKE_SUPPRESS_REGENERATION}" CACHE BOOL "If activated, the cmake files will not be checked and regenerated automatically before each build" )

endif( NOT VISTA_COMMON_INCLUDED ) # this shows we did not include it yet
