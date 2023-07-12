
# defines list VISTA_PACKAGE_SEARCH_PATHS that contains pathes where packages can be found
# defines utilitiy macros for finding packages/directories and checking versions
# vista_check_version_entry( INPUT_VERSION OWN_VERSION DIFFERENCE_VAR )
# vista_string_to_version( VERSION_STRING VERSION_VARIABLES_PREFIX )
# vista_compare_versions( INPUT_VERSION_PREFIX OWN_VERSION_PREFIX DIFFERENCE_VAR )
# vista_find_package_dirs( PACKAGE_NAME EXAMPLE_FILE [NAMES name1 name2 ...] [ PATHS path1 path2 ... ] )
# vista_get_version_from_path( PATH NAME_LIST VERSION_VAR )
# SEE FULL DOCUMENTATION! vista_find_package_root( PACKAGE EXAMPLE_FILE [DONT_ALLOW_UNVERSIONED] [QUIET] [NAMES name1 name2 ...] [ PATHS path1 path2 ... ] [ADVANCED] [NO_CACHE] )
# SEE FULL DOCUMENTATION! vista_find_library_uncached( ...find_library_parameters... )

if( NOT VISTA_FIND_UTILS_INCLUDED )
set( VISTA_FIND_UTILS_INCLUDED TRUE )

set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

include( VistaHWArchSettings )

# vista_find_original_package( OWN_PREFIX [ORIGINAL_NAME  [ADDITIONAL_PARAMS]] )
# Finds and loads an 'original' FindORIGINAL_NAME module from within a
# VModule file
# OWN_PREFIX is the name of the current FindModule file (including the V!)
# ORIGINAL_NAME is the optional name of the original module file (defaults 
#                to OWN_PREFIX without preceeding V)
# ADDITIONAL_PARAMS are added to the find_package calls - note that if these are given, original name must be set
macro( vista_find_original_package _OWNPREFIX )
	if( "${ARGC}" EQUAL 1 )
		string( REGEX MATCH "^V(.*)$" _MATCH ${_OWNPREFIX} )
		if( _MATCH )
			set( _FINDARGS ${CMAKE_MATCH_1} )
		else()
			set( _FINDARGS ${_OWNPREFIX} )
		endif()
	else()
		set( _FINDARGS ${ARGV1} )
	endif()
	
	set( _FINDARGS ${_FINDARGS} ${${_OWNPREFIX}_FIND_VERSION} )
	
	if( ${_OWNPREFIX}_FIND_COMPONENTS )
		set( _FINDARGS ${_FINDARGS} COMPONENTS ${${_OWNPREFIX}_FIND_COMPONENTS} )
	endif()
	
	if( ${_OWNPREFIX}_FIND_REQUIRED )
		set( _FINDARGS ${_FINDARGS} REQUIRED )
	endif()
	
	if( ${_OWNPREFIX}_FIND_QUIETLY )
		set( _FINDARGS ${_FINDARGS} QUIET )
	endif()
	
	if( ${PREFIX}_FIND_QUIETLY )
	set( _FINDARGS ${_FINDARGS} QUIET )
	endif()
	if( ${PREFIX}_FIND_REQUIRED )
		set( _FINDARGS ${_FINDARGS} REQUIRED )
	endif()
	
	if( ${ARGC} GREATER 2 )
		set( _ADDITIONAL_ARGS ${ARGN} )
		list( REMOVE_AT _ADDITIONAL_ARGS 0 )
		set( _FINDARGS ${_FINDARGS} ${_ADDITIONAL_ARGS} )
	endif()
	
	find_package( ${_FINDARGS} )
	
endmacro( vista_find_original_package )


set( VISTA_PACKAGE_SEARCH_PATHS )
foreach( _PATH "$ENV{VRDEV}" "$ENV{VISTA_EXTERNAL_LIBS}" "$ENV{VRSOFTWARE}"
						${CMAKE_PREFIX_PATH} $ENV{CMAKE_PREFIX_PATH}
						${CMAKE_SYSTEM_PREFIX_PATH}	$ENV{CMAKE_SYSTEM_PREFIX_PATH} )
	if( NOT _PATH STREQUAL "/" AND NOT _PATH STREQUAL "" )
		file( TO_CMAKE_PATH "${_PATH}" _PATH )
		list( APPEND VISTA_PACKAGE_SEARCH_PATHS ${_PATH} )
	endif()
endforeach()
list( REMOVE_DUPLICATES VISTA_PACKAGE_SEARCH_PATHS )

# vista_check_version_entry( INPUT_VERSION OWN_VERSION DIFFERENCE_VAR )
# compares the input version against the own version entry, and computes the difference
# INPUT_VERSION has to be a number, OWN_VERSION is an extended version number, i.e.
# x or x+ or x-y (where x and y are numbers). The Output difference is stored in the
# difference variable, and is
#   v tested against own x     :    -1 if v < x, (v-x) otherwise
#   v tested against own x+    :    -1 if v < x, 0 if v >= x
#   v tested against own x-y    :   0 if x <= v <= y, -1 otherwise
macro( vista_check_version_entry INPUT_VERSION OWN_VERSION DIFFERENCE_OUTPUT_VAR )
	set( ${DIFFERENCE_OUTPUT_VAR} -1 )

	if( "${INPUT_VERSION}" STREQUAL "" OR "${OWN_VERSION}" STREQUAL "" )
		# fine for us, just accept
		set( ${DIFFERENCE_OUTPUT_VAR} 0 )
	else()
		string( REGEX MATCH "([0-9]+)\\+$" _STRING_IS_MIN ${INPUT_VERSION} )
		if( _STRING_IS_MIN )
			if( ( ${OWN_VERSION} EQUAL ${CMAKE_MATCH_1} ) OR ( ${OWN_VERSION} GREATER ${CMAKE_MATCH_1} ) )
				set( ${DIFFERENCE_OUTPUT_VAR} 0 )
			endif()
		else()
			string( REGEX MATCH "([0-9]+)\\-([0-9]+)$" _STRING_IS_RANGE ${INPUT_VERSION} )
			if( _STRING_IS_RANGE )
				if( ( ${OWN_VERSION} EQUAL ${CMAKE_MATCH_1} ) OR ( ${OWN_VERSION} GREATER ${CMAKE_MATCH_1} ) )
					if( ( ${OWN_VERSION} EQUAL ${CMAKE_MATCH_2} ) OR ( ${OWN_VERSION} LESS ${CMAKE_MATCH_2} ) )
						set( ${DIFFERENCE_OUTPUT_VAR} 0 )
					endif( ( ${OWN_VERSION} EQUAL ${CMAKE_MATCH_2} ) OR ( ${OWN_VERSION} LESS ${CMAKE_MATCH_2} ) )
				endif()
			elseif( "${INPUT_VERSION}" VERSION_EQUAL "${OWN_VERSION}" )
				# exact match
				set( ${DIFFERENCE_OUTPUT_VAR} 0 )
			elseif( "${INPUT_VERSION}" VERSION_LESS "${OWN_VERSION}" )
				# compatible match
				math( EXPR ${DIFFERENCE_OUTPUT_VAR}  "${OWN_VERSION} - ${INPUT_VERSION}" )
			endif()
		endif()
	endif()
endmacro( vista_check_version_entry )

# vista_extract_version_part( TARGET ENTRY SEPARATOR )
# specialized, mainly for internal use
macro( vista_extract_version_part _TARGET _ENTRY _SEPARATOR  )
	set( ${_TARGET} )

	if( NOT "${_REMAINING_VERSION}" STREQUAL "" )
		string( REGEX MATCH "^(${_ENTRY})${_SEPARATOR}(.*)$" _MATCH_SUCCESS "${_REMAINING_VERSION}" )
		if( _MATCH_SUCCESS )
			# we found a textual start -> type
			set( ${_TARGET} ${CMAKE_MATCH_1} )
			set( _REMAINING_VERSION ${CMAKE_MATCH_2} )
		else()
			string( REGEX MATCH "^(${_ENTRY})$" _MATCH2_SUCCESS "${_REMAINING_VERSION}" )
			if( NOT "${_MATCH2_SUCCESS}" STREQUAL "" )
				# we found a textual start -> type
				set( ${_TARGET} ${CMAKE_MATCH_1} )
				set( _REMAINING_VERSION ${CMAKE_MATCH_2} )
			endif()
		endif()
	endif()
endmacro()

# vista_find_library_dir( TARGET_DIR_VARIABLE LIBRARY_NAME PATH [ PATH... ] )
# searches the prepended pathes for the specified library, and, if found, adds
# the path to TARGET_DIR_VARIABLE (by appending it)
macro( vista_find_library_dir _TARGET_VAR _LIB_NAME )
	set( _TMP_LIB _TMP_LIB-NOTFOUND CACHE INTERNAL "" FORCE )
	find_library( _TMP_LIB ${_LIB_NAME} PATHS ${ARGN} NO_DEFAULT_PATH )
	if( _TMP_LIB )
		get_filename_component( _TMP_PATH "${_TMP_LIB}" PATH )
		list( APPEND ${_TARGET_VAR} "${_TMP_PATH}" )
	endif()
	set( _TMP_LIB _TMP_LIB-NOTFOUND CACHE INTERNAL "" FORCE )
endmacro( vista_find_library_dir )

# vista_find_library_dir( OUT_VARIABLE LIBRARY_NAME PATH [ PATH... ] )
# searches the passed pathes for the specified library, and sers
# OUT_VARIABLE to either TRUE or FALSE accordingly
macro( vista_check_library_exists _TARGET_VAR _LIB_NAME )
	set( ${_TARGET_VAR} FALSE )
	set( _TMP_LIB _TMP_LIB-NOTFOUND CACHE INTERNAL "" FORCE )
	find_library( _TMP_LIB ${_LIB_NAME} PATHS ${ARGN} NO_DEFAULT_PATH )
	if( _TMP_LIB )
		set( ${_TARGET_VAR} TRUE )
	endif()
	set( _TMP_LIB _TMP_LIB-NOTFOUND CACHE INTERNAL "" FORCE )
endmacro( vista_check_library_exists )

# vista_string_to_version( VERSION_STRING VERSION_VARIABLES_PREFIX )
# parses the VERSION_STRING and sets the version values for the given prefix
# The input format can be: V1[.V2[.V3[.V4]]], [TYPE_]NAME, [TYPE_]NAME-V1[.V2[.V3[.V4]]]
# the following variables will fold the parsed version results:
# <PREFIX>_VERSION_TYPE
# <PREFIX>_VERSION_NAME
# <PREFIX>_VERSION_MAJOR
# <PREFIX>_VERSION_MINOR
# <PREFIX>_VERSION_PATCH
# <PREFIX>_VERSION_TWEAK
macro( vista_string_to_version VERSION_STRING VERSION_VARIABLES_PREFIX )
	set( _REMAINING_VERSION ${VERSION_STRING} )

	vista_extract_version_part( ${VERSION_VARIABLES_PREFIX}_VERSION_TYPE  "[a-zA-Z]+"         "_" )
	if( NOT ${VERSION_VARIABLES_PREFIX}_VERSION_TYPE STREQUAL "HEAD"
		AND NOT ${VERSION_VARIABLES_PREFIX}_VERSION_TYPE STREQUAL "RELEASE"
		AND NOT ${VERSION_VARIABLES_PREFIX}_VERSION_TYPE STREQUAL "BRANCH"
		AND NOT ${VERSION_VARIABLES_PREFIX}_VERSION_TYPE STREQUAL "TRUNK" )
		set( ${VERSION_VARIABLES_PREFIX}_VERSION_TYPE )
		set( _REMAINING_VERSION ${VERSION_STRING} )
	endif()
	vista_extract_version_part( ${VERSION_VARIABLES_PREFIX}_VERSION_NAME  "[a-zA-Z][^\\\\-]+" "\\\\-" )
	vista_extract_version_part( ${VERSION_VARIABLES_PREFIX}_VERSION_MAJOR "[0-9\\\\+\\\\-]+"  "\\\\." )
	vista_extract_version_part( ${VERSION_VARIABLES_PREFIX}_VERSION_MINOR "[0-9\\\\+\\\\-]+"  "\\\\." )
	vista_extract_version_part( ${VERSION_VARIABLES_PREFIX}_VERSION_PATCH "[0-9\\\\+\\\\-]+"  "\\\\." )
	vista_extract_version_part( ${VERSION_VARIABLES_PREFIX}_VERSION_TWEAK "[0-9\\\\+\\\\-]+"  "\\\\-" )

	#if there is just one (textual) entry, it's the name
	if( ${VERSION_VARIABLES_PREFIX}_VERSION_TYPE AND NOT ${VERSION_VARIABLES_PREFIX}_VERSION_NAME AND NOT ${VERSION_VARIABLES_PREFIX}_VERSION_MAJOR )
		set( ${VERSION_VARIABLES_PREFIX}_VERSION_NAME ${${VERSION_VARIABLES_PREFIX}_VERSION_TYPE} )
		set( ${VERSION_VARIABLES_PREFIX}_VERSION_TYPE "" )
	endif()

endmacro( vista_string_to_version )

# vista_compare_versions( INPUT_VERSION_PREFIX OWN_VERSION_PREFIX DIFFERENCE_VAR )
# takes an input version and an own version, where each one is defined by version variables
# with the given prefix, as they are set by vista_string_to_version
# the result written to DIFFERENCE_VAR is a version difference of form x.y.z.w, that specifies
# how well the version matches. If any of the four values is -1, the version does not match at all,
# if all values are 0, the version matches exactly
macro( vista_compare_versions INPUT_VERSION_PREFIX OWN_VERSION_PREFIX DIFFERENCE_OUTPUT_VAR )
	set( _MATCHED FALSE )
	set( _VERSION_REQUIRED TRUE )
	set( ${DIFFERENCE_OUTPUT_VAR} -1 )
	
	# check if type matches
	if( NOT ${INPUT_VERSION_PREFIX}_VERSION_TYPE
			OR ${INPUT_VERSION_PREFIX}_VERSION_TYPE STREQUAL "RELEASE" )
		# if no version type is given - we assume release is requested
		if( NOT ${INPUT_VERSION_PREFIX}_VERSION_NAME )
			set( _MATCHED TRUE )
			set( _VERSION_REQUIRED TRUE )
		elseif( ${INPUT_VERSION_PREFIX}_VERSION_NAME STREQUAL ${OWN_VERSION_PREFIX}_VERSION_NAME )
			set( _MATCHED TRUE )
			set( _VERSION_REQUIRED FALSE )
		elseif( ${OWN_VERSION_PREFIX}_VERSION_TYPE STREQUAL "RELEASE" AND ( ${INPUT_VERSION_PREFIX}_VERSION_NAME STREQUAL "" ) )
				# for release, we accept either the matching name, or none at all
				set( _MATCHED TRUE )
				set( _VERSION_REQUIRED TRUE )
		endif()
	elseif( ${INPUT_VERSION_PREFIX}_VERSION_TYPE STREQUAL "HEAD"
			OR ${INPUT_VERSION_PREFIX}_VERSION_TYPE STREQUAL "BRANCH"
			OR ${INPUT_VERSION_PREFIX}_VERSION_TYPE STREQUAL "TAG" )
		# 'normal' test - name has to match
		if( ${INPUT_VERSION_PREFIX}_VERSION_NAME STREQUAL ${OWN_VERSION_PREFIX}_VERSION_NAME )
			set( _MATCHED TRUE )
			set( _VERSION_REQUIRED FALSE )
		endif()
	else()
		message( WARNING "vista_compare_versions() - version type ${INPUT_VERSION_PREFIX}_VERSION_TYPE = ${${INPUT_VERSION_PREFIX}_VERSION_TYPE} is unknown" )
	endif()
			
	if( _MATCHED )
		# version type and name are okay - check number
		# if no numer exists, distinguish: if name exists, no num is required for matching, else it does
		if( "${${INPUT_VERSION_PREFIX}_VERSION_MAJOR}" STREQUAL "" OR "${${OWN_VERSION_PREFIX}_VERSION_MAJOR}" STREQUAL "" )
			if( _VERSION_REQUIRED )
				set( ${DIFFERENCE_OUTPUT_VAR} "-1.-1.-1.-1" )
			else()
				set( ${DIFFERENCE_OUTPUT_VAR} "0.0.0.0" )
			endif()
		else()			
			vista_check_version_entry( "${${INPUT_VERSION_PREFIX}_VERSION_MAJOR}" "${${OWN_VERSION_PREFIX}_VERSION_MAJOR}" _DIFFERENCE_MAJOR )
			vista_check_version_entry( "${${INPUT_VERSION_PREFIX}_VERSION_MINOR}" "${${OWN_VERSION_PREFIX}_VERSION_MINOR}" _DIFFERENCE_MINOR )
			vista_check_version_entry( "${${INPUT_VERSION_PREFIX}_VERSION_PATCH}" "${${OWN_VERSION_PREFIX}_VERSION_PATCH}" _DIFFERENCE_PATCH )
			vista_check_version_entry( "${${INPUT_VERSION_PREFIX}_VERSION_TWEAK}" "${${OWN_VERSION_PREFIX}_VERSION_TWEAK}" _DIFFERENCE_TWEAK )
			if( _DIFFERENCE_MAJOR GREATER -1 AND _DIFFERENCE_MINOR GREATER -1 AND _DIFFERENCE_PATCH GREATER -1 AND _DIFFERENCE_TWEAK GREATER -1 )
				set( ${DIFFERENCE_OUTPUT_VAR} "${_DIFFERENCE_MAJOR}.${_DIFFERENCE_MINOR}.${_DIFFERENCE_PATCH}.${_DIFFERENCE_TWEAK}" )
			endif()
		endif()
	endif()	
endmacro( vista_compare_versions )

# vista_find_package_dirs( PACKAGE_NAME EXAMPLE_FILE [NAMES folder1 folder2 ...] [PATHS path1 path2] [DEBUG_OUTPUT] )
# parses the standard search directories
# CMAKE_PREFIX_PATH and CMAKE_SYSTEM_PREFIX_PATH -- to find any root dirs and their version
# Parameters:
#      PACKAGE_NAME     - Name of the Package
#      EXAMPLE_FILE     - File (optionally with prefixing dirs, e.g./include/GL/glut.h)
#                         that is located in the searched-for root dir
#      NAMES folder1 folder2 ... - list of alternate names for the folders that should be
#                         accepted, e.g. glut and freeglut as alternatives
#      SUBDIRS subdir1 subdir2 ... - list of additional subdirs between the normal base pathes and the root dir (allows globbing wildcards) 
#      PATHS path1 path2 ... - list of (absolute) base pathes that are to be used in
#                         addition to the default ones
# Output: The following variables will be set
#      <PACKAGE_NAME>_CANDIDATE_DIRS        - List of versioned dirs that were found
#      <PACKAGE_NAME>_CANDIDATE_VERSIONS    - List of versions for the found candidates
#      <PACKAGE_NAME>_CANDIDATE_UNVERSIONED - First directory found without a version
macro( vista_find_package_dirs _PACKAGE_NAME _EXAMPLE_FILE )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
	string( TOLOWER ${_PACKAGE_NAME} _PACKAGE_NAME_LOWER )

	set( _ARGS ${ARGV} )
	list( REMOVE_AT _ARGS 0 1 )

	if( WIN32 )
		set( _PACKAGE_FOLDER_NAMES ${_PACKAGE_NAME} )
	else()
		set( _PACKAGE_FOLDER_NAMES ${_PACKAGE_NAME} ${_PACKAGE_NAME_UPPER} ${_PACKAGE_NAME_LOWER} )
	endif( WIN32 )
	
	set( _ADDITIONAL_PATHES "" )
	set( _SUBDIRS "/" )

	set( _NEXT_IS_NAME FALSE )
	set( _NEXT_IS_PATH FALSE )
	set( _NEXT_IS_SUBDIR FALSE )
	set( _DEBUG_OUTPUT FALSE )
	foreach( _ARG ${_ARGS} )
		if( ${_ARG} STREQUAL "NAMES" )
			set( _NEXT_IS_NAME TRUE )
			set( _NEXT_IS_PATH FALSE )
			set( _NEXT_IS_SUBDIR FASLE )
		elseif( ${_ARG} STREQUAL "PATHS" )
			set( _NEXT_IS_NAME FALSE)
			set( _NEXT_IS_PATH TRUE )
			set( _NEXT_IS_SUBDIR FALSE )
		elseif( ${_ARG} STREQUAL "SUBDIRS" )
			set( _NEXT_IS_NAME FALSE)
			set( _NEXT_IS_PATH FALSE )
			set( _NEXT_IS_SUBDIR TRUE )
		elseif( ${_ARG} STREQUAL "DEBUG_OUTPUT" )
			set( _NEXT_IS_NAME FALSE)
			set( _NEXT_IS_PATH FALSE )
			set( _DEBUG_OUTPUT TRUE )		
		elseif( _NEXT_IS_NAME )
			list( APPEND _PACKAGE_FOLDER_NAMES ${_ARG} )
		elseif( _NEXT_IS_PATH )
			list( APPEND _ADDITIONAL_PATHES ${_ARG} )
		elseif( _NEXT_IS_SUBDIR )
			list( APPEND _SUBDIRS "${_ARG}/" )
		else()
			message( WARNING "vista_find_package_dirs() - unknown argument ${_ARG}" )
		endif( ${_ARG} STREQUAL "NAMES" )
	endforeach( _ARG ${_ARGS} )

	set( ${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS )
	set( ${_PACKAGE_NAME_UPPER}_CANDIDATE_VERSIONS )
	set( ${_PACKAGE_NAME_UPPER}_CANDIDATE_UNVERSIONED )

	set( _VERSIONED_PATHES )
	set( _UNVERSIONED )

	if( EXISTS "$ENV{${_PACKAGE_NAME_UPPER}_ROOT}" )
		# check if PACKAGENAME_ROOT envvar is set and valid
		# if so, use it as unversioned
		if( _DEBUG_OUTPUT )
			message( STATUS "vista_find_package_dirs( \"${_PACKAGE_NAME}\" ) Found env var ${_PACKAGE_NAME_UPPER}_ROOT=\"$ENV{${_PACKAGE_NAME_UPPER}_ROOT}\"" )
		endif()
		if( EXISTS "$ENV{${_PACKAGE_NAME_UPPER}_ROOT}/${_VISTA_HWARCH}/${_EXAMPLE_FILE}" )
			set( _UNVERSIONED "$ENV{${_PACKAGE_NAME_UPPER}_ROOT}/${_VISTA_HWARCH}" )
		elseif( EXISTS "$ENV{${_PACKAGE_NAME_UPPER}_ROOT}/${_EXAMPLE_FILE}" )
			set( _UNVERSIONED "$ENV{${_PACKAGE_NAME_UPPER}_ROOT}" )
		endif()
	endif()

	foreach( _PATH $ENV{${_PACKAGE_NAME_UPPER}_ROOT} ${_ADDITIONAL_PATHES} ${VISTA_PACKAGE_SEARCH_PATHS} )
		foreach( _FOLDER ${_PACKAGE_FOLDER_NAMES} )	
			foreach( _SUBDIR ${_SUBDIRS} )
				foreach( _HWARCH "" ${VISTA_COMPATIBLE_HWARCH} )
					foreach( _COMPOUND_PATH	"${_PATH}/${_HWARCH}/${_SUBDIR}"
											"${_PATH}/${_FOLDER}/${_HWARCH}/${_SUBDIR}"
											"${_PATH}/${_FOLDER}-*/${_HWARCH}/${_SUBDIR}"
											"${_PATH}/${_FOLDER}/${_FOLDER}-*/${_HWARCH}/${_SUBDIR}" )
					
						# look for pathes with a version
						file( GLOB _TMP_PATHES "${_COMPOUND_PATH}" )
						if( _DEBUG_OUTPUT )
							message( STATUS "vista_find_package_dirs( \"${_PACKAGE_NAME}\" ) Results for \"${_COMPOUND_PATH}\" are ${_TMP_PATHES}" )
						endif()
						list( APPEND _VERSIONED_PATHES ${_TMP_PATHES} )
					
						# look for unversioned pathes
						if( NOT _UNVERSIONED )
							if( EXISTS "${_COMPOUND_PATH}/${_EXAMPLE_FILE}" )						
								# ../NAME/NAME/HWARCH
								set( _UNVERSIONED "${_COMPOUND_PATH}" )
								if( _DEBUG_OUTPUT )
									message( STATUS "vista_find_package_dirs( \"${_PACKAGE_NAME}\" ) found unversioned \"${_COMPOUND_PATH}\"" )
								endif()
								break()							
							endif()
						endif()
						
					endforeach()
				endforeach()
			endforeach()
		endforeach()
	endforeach()

	if( _UNVERSIONED )
		file( TO_CMAKE_PATH ${_UNVERSIONED} "${_PACKAGE_NAME_UPPER}_CANDIDATE_UNVERSIONED" )
	endif()

	#check versioned pathes
	foreach( _PATH ${_VERSIONED_PATHES} )
		file( TO_CMAKE_PATH "${_PATH}" _PATH )
		# determine version
		string( REGEX REPLACE "([+-.])" "\\\\\\1" _REGEX_PATH "${_PATH}" )
		string( REGEX MATCH ".+\\-([0-9a-zA-Z_\\-\\.]+)" _MATCHED "${_REGEX_PATH}" )
		if( _MATCHED )
			string( REPLACE "\\" "" _CLEANED_VERSION "${CMAKE_MATCH_1}" )
			set( _FOUND )
			if( EXISTS "${_PATH}/${_EXAMPLE_FILE}" )
				list( APPEND ${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS "${_PATH}" )				
				list( APPEND ${_PACKAGE_NAME_UPPER}_CANDIDATE_VERSIONS ${_CLEANED_VERSION} )
				if( _DEBUG_OUTPUT )
					message( STATUS "found requested file \"${_EXAMPLE_FILE}\" in dir \"${_PATH}\" (Version: ${_CLEANED_VERSION})" )
					set( _FOUND TRUE )
				endif()
			else()
				foreach( _HWARCH ${VISTA_COMPATIBLE_HWARCH} )
					if( EXISTS "${_PATH}/${_HWARCH}/${_EXAMPLE_FILE}" )
						list( APPEND ${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS "${_PATH}/${_HWARCH}" )
						list( APPEND ${_PACKAGE_NAME_UPPER}_CANDIDATE_VERSIONS ${_CLEANED_VERSION} )
						if( _DEBUG_OUTPUT )
							message( STATUS "found requested file \"${_EXAMPLE_FILE}\" in dir \"${_PATH}/${_HWARCH}\" (Version: ${_CLEANED_VERSION})" )
							set( _FOUND TRUE )
						endif()
						break()
					endif()
				endforeach( _HWARCH ${VISTA_COMPATIBLE_HWARCH} )
			endif()
			if( _DEBUG_OUTPUT AND NOT _FOUND )
				message( STATUS "did NOT find requested file \"${_EXAMPLE_FILE}\" in dir \"${_PATH}/${_HWARCH}\"" )
			endif()
		else()
			message( WARNING "vista_find_package_dirs cant extract version from \"${_PATH}\" - skipping" )
		endif()
	endforeach( _PATH ${_VERSIONED_PATHES} )
	
	if( _DEBUG_OUTPUT )
		message( STATUS "vista_find_package_dirs( \"${_PACKAGE_NAME}\" ) found dirs:" )
		message( STATUS "\t\t${_PACKAGE_NAME_UPPER}_CANDIDATE_UNVERSIONED = ${${_PACKAGE_NAME_UPPER}_CANDIDATE_UNVERSIONED}" )
		message( STATUS "\t\t${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS = ${${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS}" )
		message( STATUS "\t\t${_PACKAGE_NAME_UPPER}_CANDIDATE_VERSIONS = ${${_PACKAGE_NAME_UPPER}_CANDIDATE_VERSIONS}" )
	endif()

endmacro()

# vista_get_version_from_path( PATH NAME_LIST VERSION_VAR )
# parses the path string and tries to find a version.
# For each name in NAME_LIST, a version is accepted as
# ...<NAME>-VERSION[/..]
# the found version is stored in VERSION_VAR
macro( vista_get_version_from_path _PATH _NAME_LIST _VERSION_VAR )
	file( TO_CMAKE_PATH "${_PATH}" _PATH )
	set( ${_VERSION_VAR} )
	# determine version
	foreach( _NAME ${${_NAME_LIST}} )
		string( REGEX MATCH ".+\\${_NAME}-([0-9a-zA-Z_\\-\\.]+)(/.*)*$" _MATCHED ${_PATH} )
		if( _MATCHED )
			set( ${_VERSION_VAR} ${CMAKE_MATCH_1} )
			break()
		endif( _MATCHED )
	endforeach( _NAME ${${_NAME_LIST}} )
endmacro( vista_get_version_from_path )

# vista_find_package_root( PACKAGE EXAMPLE_FILE [DONT_ALLOW_UNVERSIONED] [PREFER_UNVERSIONED] [QUIET] [DEBUG_OUTPUT] [NAMES name1 name2 ...] [PATHS path1 path2] [SUNDIRS dir1 dir2] [ADVANCED] [NO_CACHE] )
# finds the package root for PACKAGE, and stores it in the variable <PACKAGE>_ROOT_DIR
# Should only be called from the context of a Find<XYZ>.cmake file - it automatically checks if
# versions are requested, and if so, looks for an appropriately versioned dir, otherwise, it
# takes the first unversioned dir or the one with the highest version
# Since the ROOT_DIR is a cache variable, it can be manipulated by the user - if it is changed manually
# the new path is checked to try to extract a version
# By default, <PACKAGE>_ROOT_DIR is a cache var, but the optional arguments ADVANCED and NO_CACHE
# make it advanced or uncached
# if QUIET is specified, no info messages will be printed
# if DEBUG_OUTPUT is specified, additional debug info will be printed
# if DONT_ALLOW_UNVERSIONED is specified, no unversioned path is accepted when a versioned one is requested
# if PREFER_UNVERSIONED is set and no version is requested, an unversioned package is used - otherwise by default
# the highest version is chosen
# the NAMES list provides alternative names to be searched (equivalently to vista_find_package_dirs)
# the PATHS list provides additional (absolute) base pathes to search (equivalently to vista_find_package_dirs)
#      SUBDIRS subdir1 subdir2 ... - list of additional subdirs between the normal base pathes and the root dir (allows globbing wildcards), equivalent to vista_find_package_dirs
macro( vista_find_package_root _PACKAGE_NAME _EXAMPLE_FILE )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
	string( TOLOWER ${_PACKAGE_NAME} _PACKAGE_NAME_LOWER )
	
	if( ${_PACKAGE_NAME_UPPER}_ROOT_DIR )
		# we first check if the set dir actually exists
		
		if( NOT EXISTS "${${_PACKAGE_NAME_UPPER}_ROOT_DIR}" )
			# the formerly found or manually set dir doesn't exist - warn, and re-search
			message( WARNING "${_PACKAGE_NAME_UPPER}_ROOT_DIR was set to non-existant directory "
					"\"${${_PACKAGE_NAME_UPPER}_ROOT_DIR}\" - discarding dir and performing new search" )
			set( ${_PACKAGE_NAME_UPPER}_ROOT_DIR "${_PACKAGE_NAME_UPPER}_ROOT_DIR-NOTFOUND" CACHE PATH "${_PACKAGE_NAME} package root dir" FORCE )
			set( ${_PACKAGE_NAME_UPPER}_LAST_CACHED_PATCH ${${_PACKAGE_NAME_UPPER}_ROOT_DIR} CACHE INTERNAL "" FORCE )
		elseif( NOT ${_PACKAGE_NAME_UPPER}_ROOT_DIR STREQUAL ${_PACKAGE_NAME_UPPER}_LAST_CACHED_PATCH )
			# version was changed - we parse args to find the possible names, and then
			# try to extract a version from the path
			set( _ARGS ${ARGV} )
			if( WIN32 )
				set( _PACKAGE_FOLDER_NAMES ${_PACKAGE_NAME} )
			else()
				set( _PACKAGE_FOLDER_NAMES ${_PACKAGE_NAME} ${_PACKAGE_NAME_UPPER} ${_PACKAGE_NAME_LOWER} )
			endif( WIN32 )
			set( _NEXT_IS_FOLDER FALSE )
			foreach( _ARG ${_ARGS} )
				if( ${_ARG} STREQUAL "NAMES" )
                    Message("${NAMES}")
					set( _NEXT_IS_FOLDER TRUE )
				elseif( _NEXT_IS_FOLDER )
					if( ${_ARG} STREQUAL "DONT_ALLOW_UNVERSIONED"
						OR ${_ARG} STREQUAL "PREFER_UNVERSIONED"
						OR ${_ARG} STREQUAL "QUIET"
						OR ${_ARG} STREQUAL "ADVANCED"
						OR ${_ARG} STREQUAL "DEBUG_OUTPUT"
						OR ${_ARG} STREQUAL "NO_CACHE" )
						break()
					else()
						list( APPEND _PACKAGE_FOLDER_NAMES ${_ARG} )
					endif()
				endif()
			endforeach()

			set( ${_PACKAGE_NAME_UPPER}_LAST_CACHED_PATCH ${${_PACKAGE_NAME_UPPER}_ROOT_DIR} CACHE INTERNAL "" FORCE )
			vista_get_version_from_path( ${${_PACKAGE_NAME_UPPER}_ROOT_DIR} _PACKAGE_FOLDER_NAMES _VERSION )
			if( _VERSION )
				if( NOT QUIET )
					message( STATUS "${_PACKAGE_NAME_UPPER}_ROOT_DIR was overwritten to \"${${_PACKAGE_NAME_UPPER}_ROOT_DIR}\""
								" - extracted version (${_VERSION}) from directory name" )
				endif( NOT QUIET )
				set( ${_PACKAGE_NAME_UPPER}_VERSION_STRING ${_VERSION} CACHE INTERNAL "" )
				set( ${_PACKAGE_NAME}_VERSION ${${_PACKAGE_NAME_UPPER}_VERSION_STRING} )
				vista_string_to_version( "${${_PACKAGE_NAME_UPPER}_VERSION_STRING}" "${_PACKAGE_NAME_UPPER}" )
			endif( _VERSION )
		endif()
	endif()

	if( NOT ${_PACKAGE_NAME_UPPER}_ROOT_DIR )
		# parse arguments
		set( _DONT_ALLOW_UNVERSIONED FALSE )
		set( _PREFER_UNVERSIONED FALSE )
		set( _QUIET FALSE )
		set( _ADVANCED FALSE )
		set( _NO_CACHE FALSE )
		set( _DEBUG_OUTPUT FALSE )

		set( _ARGS ${ARGV} )
		list( FIND _ARGS "NO_CACHE" _FOUND )
		if( _FOUND GREATER -1 )
			set( _NO_CACHE TRUE )
		endif( _FOUND GREATER -1 )
		list( FIND _ARGS "QUIET" _FOUND )
		if( _FOUND GREATER -1 )
			set( _QUIET TRUE )
		endif( _FOUND GREATER -1 )
		list( FIND _ARGS "ADVANCED" _FOUND )
		if( _FOUND GREATER -1 )
			set( _ADVANCED TRUE )
		endif( _FOUND GREATER -1 )
		list( FIND _ARGS "DONT_ALLOW_UNVERSIONED" _FOUND )
		if( _FOUND GREATER -1 )
			set( _DONT_ALLOW_UNVERSIONED TRUE )
		endif( _FOUND GREATER -1 )
		list( FIND _ARGS "PREFER_UNVERSIONED" _FOUND )
		if( _FOUND GREATER -1 )
			set( _PREFER_UNVERSIONED TRUE )
		endif( _FOUND GREATER -1 )
		list( FIND _ARGS "DEBUG_OUTPUT" _FOUND )
		if( _FOUND GREATER -1 )
			set( _DEBUG_OUTPUT TRUE )
		endif( _FOUND GREATER -1 )

		list( REMOVE_ITEM _ARGS "NO_CACHE" "QUIET" "ADVANCED" "DONT_ALLOW_UNVERSIONED" )

		#find package dirs
		vista_find_package_dirs( ${_ARGS} )

		set( _FOUND_DIR "${_PACKAGE_NAME_UPPER}_ROOT_DIR-NOTFOUND" )
		set( _FOUND_VERSION "" )

		# chech if a version is requested
		if( V${_PACKAGE_NAME}_FIND_VERSION_EXT )
			set( _REQUESTED_VERSION ${V${_PACKAGE_NAME}_FIND_VERSION_EXT} )
			set( _VERSION_EXACT ${V${_PACKAGE_NAME}_FIND_VERSION_EXACT} )
		elseif( V${_PACKAGE_NAME}_FIND_VERSION )
			set( _REQUESTED_VERSION ${V${_PACKAGE_NAME}_FIND_VERSION} )
			set( _VERSION_EXACT ${V${_PACKAGE_NAME}_FIND_VERSION_EXACT} )
		elseif( ${_PACKAGE_NAME}_FIND_VERSION_EXT )
			set( _REQUESTED_VERSION ${${_PACKAGE_NAME}_FIND_VERSION_EXT} )
			set( _VERSION_EXACT ${${_PACKAGE_NAME}_FIND_VERSION_EXACT} )
		elseif( ${_PACKAGE_NAME}_FIND_VERSION )
			set( _REQUESTED_VERSION ${${_PACKAGE_NAME}_FIND_VERSION} )
			set( _VERSION_EXACT ${${_PACKAGE_NAME}_FIND_VERSION_EXACT} )
		else()
			set( _REQUESTED_VERSION )
		endif( V${_PACKAGE_NAME}_FIND_VERSION_EXT )
		
		if( _DEBUG_OUTPUT )
			message( STATUS "vista_find_package_root( \"${_PACKAGE_NAME}\" ) looking for version \"${_REQUESTED_VERSION}\"" )
		endif()

		if( _REQUESTED_VERSION )
			# parse requested version
			vista_string_to_version( ${_REQUESTED_VERSION} "_TEST_VERSION_IN" )

			set( _BEST_DIFF 999999999.999999999.999999999.999999999 )

			list( LENGTH ${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS _COUNT )
			foreach( _INDEX RANGE ${_COUNT} )
				if( _INDEX STREQUAL _COUNT )
					break() # RANGE includes last value, so we have to skip this one
				endif( _INDEX STREQUAL _COUNT )
				list( GET ${_PACKAGE_NAME_UPPER}_CANDIDATE_VERSIONS ${_INDEX} _DIR_VERSION )
				vista_string_to_version( ${_DIR_VERSION} "_TEST_VERSION_DIR" )
				vista_compare_versions( "_TEST_VERSION_IN" "_TEST_VERSION_DIR" _VERSION_DIFFERENCE )
				if( NOT _VERSION_DIFFERENCE EQUAL -1 )
					if( _VERSION_DIFFERENCE VERSION_LESS _BEST_DIFF )
						set( _BEST_DIFF ${_VERSION_DIFFERENCE} )
						list( GET ${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS ${_INDEX} _FOUND_DIR )
						set( _FOUND_VERSION ${_DIR_VERSION} )
					endif( _VERSION_DIFFERENCE VERSION_LESS _BEST_DIFF )
				endif( NOT _VERSION_DIFFERENCE EQUAL -1 )
			endforeach( _INDEX RANGE ${_COUNT} )
			
			if( NOT _FOUND_DIR )
				if( _DEBUG_OUTPUT )
					message( STATUS "vista_find_package_root( \"${_PACKAGE_NAME}\" ) found no matching version - trying unversioned" )
				endif()
				if( NOT _DONT_ALLOW_UNVERSIONED AND ${_PACKAGE_NAME_UPPER}_CANDIDATE_UNVERSIONED )
					set( _FOUND_DIR ${${_PACKAGE_NAME_UPPER}_CANDIDATE_UNVERSIONED} )
					set( _FOUND_VERSION )
					if( NOT _QUIET )
						message( STATUS "Package root for ${_PACKAGE_NAME} with version ${_REQUESTED_VERSION} "
									"could not be found - using unversioned root" )
					endif( NOT _QUIET )
				else()
					if( _DEBUG_OUTPUT )
						message( STATUS "vista_find_package_root( \"${_PACKAGE_NAME}\" ) found no unversioned candidate" )
					endif()
				endif( NOT _DONT_ALLOW_UNVERSIONED AND ${_PACKAGE_NAME_UPPER}_CANDIDATE_UNVERSIONED )
			elseif( _BEST_DIFF VERSION_GREATER 0.0.0.0 )
				# no exact match - not found if exact, wrning otherwise
				if( _VERSION_EXACT )
					if( _DEBUG_OUTPUT )
						message( STATUS "vista_find_package_root( \"${_PACKAGE_NAME}\" ) found no exactly matching version" )
					endif()
					set( _FOUND_DIR "${_PACKAGE_NAME_UPPER}_ROOT_DIR-NOTFOUND" )
				elseif( NOT QUIET )
					message( STATUS "Package ${_PACKAGE_NAME} not found with version ${_REQUESTED_VERSION} - "
									"using best matching version ${_FOUND_VERSION}" )
				endif( _VERSION_EXACT )
			else()
				if( _DEBUG_OUTPUT )
					message( STATUS "vista_find_package_root( \"${_PACKAGE_NAME}\" ) found matching version in \"${_FOUND_DIR}\"" )
				endif()
			endif( NOT _FOUND_DIR )

		else() # no version requested
			if( ${_PACKAGE_NAME_UPPER}_CANDIDATE_UNVERSIONED AND ( _PREFER_UNVERSIONED OR NOT ${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS ) )
				set( _FOUND_DIR ${${_PACKAGE_NAME_UPPER}_CANDIDATE_UNVERSIONED} )
				set( _FOUND_VERSION )
				if( _DEBUG_OUTPUT )
					message( STATUS "vista_find_package_root( \"${_PACKAGE_NAME}\" ) found unversioned match in \"${_FOUND_DIR}\"" )
				endif()
			elseif( ${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS )
				set( _BEST_DIFF  )

				# find highest version
				list( LENGTH ${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS _COUNT )
				foreach( _INDEX RANGE ${_COUNT} )
					if( _INDEX STREQUAL _COUNT )
						break() # RANGE includes last value, so we have to skip this one
					endif( _INDEX STREQUAL _COUNT )
					list( GET ${_PACKAGE_NAME_UPPER}_CANDIDATE_VERSIONS ${_INDEX} _DIR_VERSION )
					vista_string_to_version( ${_DIR_VERSION} "_TEST_VERSION_DIR" )
					if( _DEBUG_OUTPUT )
						message( STATUS "comparing( \"${_DIR_VERSION}\" ) with \"${_BEST_DIFF}\"" )
					endif()
					if( NOT _BEST_DIFF OR _DIR_VERSION VERSION_GREATER _BEST_DIFF )
						if( _DEBUG_OUTPUT )
							message( STATUS "now prefering version \"${_DIR_VERSION}\"" )
						endif()
						list( GET ${_PACKAGE_NAME_UPPER}_CANDIDATE_DIRS ${_INDEX} _FOUND_DIR )
						set( _FOUND_VERSION ${_DIR_VERSION} )
						set( _BEST_DIFF ${_DIR_VERSION} ) 
					endif( NOT _BEST_DIFF OR _DIR_VERSION VERSION_GREATER _BEST_DIFF )
				endforeach( _INDEX RANGE ${_COUNT} )		
			endif()
		endif()

		if( _NO_CACHE )
			set( ${_PACKAGE_NAME_UPPER}_ROOT_DIR ${_FOUND_DIR} )
		else( _NO_CACHE )
			set( ${_PACKAGE_NAME_UPPER}_ROOT_DIR ${_FOUND_DIR} CACHE PATH "${_PACKAGE_NAME} package root dir" FORCE )
			set( ${_PACKAGE_NAME_UPPER}_LAST_CACHED_PATCH ${${_PACKAGE_NAME_UPPER}_ROOT_DIR} CACHE INTERNAL "" FORCE )
			if( _ADVANCED )
				mark_as_advanced( ${_PACKAGE_NAME_UPPER}_ROOT_DIR )
			endif( _ADVANCED )
		endif( _NO_CACHE )

		if( _FOUND_VERSION )
			set( ${_PACKAGE_NAME_UPPER}_VERSION_STRING ${_FOUND_VERSION} CACHE INTERNAL "" )
			set( ${_PACKAGE_NAME}_VERSION ${${_PACKAGE_NAME_UPPER}_VERSION_STRING} )
			set( ${_PACKAGE_NAME_UPPER}_VERSION ${${_PACKAGE_NAME_UPPER}_VERSION_STRING} )
			vista_string_to_version( "${${_PACKAGE_NAME_UPPER}_VERSION_STRING}" "${_PACKAGE_NAME_UPPER}" )
			if( _DEBUG_OUTPUT )
				message( STATUS "vista_find_package_root( \"${_PACKAGE_NAME}\" ) found match" )
			endif()
		else()
			if( _DEBUG_OUTPUT )
				message( STATUS "vista_find_package_root( \"${_PACKAGE_NAME}\" ) found no match" )
			endif()
		endif( _FOUND_VERSION )
		
		

	endif()

endmacro()

# vista_add_package_dependency( PACKAGE_NAME DEPENDENCY ... )
# where DEPENDENCY is the equivalent of the vista_use_package_call that should ideally be called
macro( vista_add_package_dependency _PACKAGE_NAME )
	string( TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_UPPER )
	list( APPEND ${_PACKAGE_NAME_UPPER}_DEPENDENCIES package ${ARGN} )
endmacro()

# vista_find_library_uncached( ...find_library_parameters... )
# usage is a little special: call exactly as find_library, but WITHOUT the target variable
# the output will be stored in the (uncached) variable VISTA_UNCACHED_LIBRARY
# Use this to quickly find libraries without cluttering the cache
macro( vista_find_library_uncached )
	set( VISTA_UNCACHED_LIB_SEARCH_VARIABLE "DIR-NOTFOUND" CACHE INTERNAL "" FORCE )
	find_library( VISTA_UNCACHED_LIB_SEARCH_VARIABLE ${ARGV} )
	set( VISTA_UNCACHED_LIBRARY ${VISTA_UNCACHED_LIB_SEARCH_VARIABLE} )
	set( VISTA_UNCACHED_LIB_SEARCH_VARIABLE "DIR-NOTFOUND" CACHE INTERNAL "" FORCE )
endmacro()

# vista_find_library_uncached( ...find_library_parameters... )
# same syntax as find_library, but does not store variable in cache 
macro( vista_find_library_uncached_var _OUT_VAR )
	set( VISTA_UNCACHED_LIB_SEARCH_VARIABLE "DIR-NOTFOUND" CACHE INTERNAL "" FORCE )
	find_library( VISTA_UNCACHED_LIB_SEARCH_VARIABLE ${ARGN} )
	set( ${_OUT_VAR} ${VISTA_UNCACHED_LIB_SEARCH_VARIABLE} )
	set( VISTA_UNCACHED_LIB_SEARCH_VARIABLE "DIR-NOTFOUND" CACHE INTERNAL "" FORCE )
endmacro()

endif( NOT VISTA_FIND_UTILS_INCLUDED )
