

# This file contains common settings and macros for setting up Vista projects
#
# TEST SETUP:
# vista_configure_test( TEST_NAME ... )

#####################################################
# GLOBAL SETTINGS
#####################################################
set( CTEST_OUTPUT_ON_FAILURE TRUE )

# create additional test targets with varying output that produces verbose output
if( MSVC )
	set( SILENT_TEST_NAME "RUN_TESTS_SILENT" )
	set( FAILUREOUTPUT_TEST_NAME "RUN_TESTS_OUTPUT_ON_FAILURE" )
	set( VERBOSE_TEST_NAME "RUN_TESTS_VERBOSE" )
	if( MSVC80 OR MSVC90 ) 
		set( CTEST_CONFIG_DIR "$(OutDir)" )
	else()
		set( CTEST_CONFIG_DIR "$(Configuration)" )
	endif()
else()
	set( SILENT_TEST_NAME "test_silent" )
	set( FAILUREOUTPUT_TEST_NAME "test_outputonfailure" )
	set( VERBOSE_TEST_NAME "test_verbose" )
	set( CTEST_CONFIG_DIR "${CMAKE_BUILD_TYPE}" )
endif()
	
get_property( _TEST_TARGETS_HAVE_BEEN_CONFIGURED GLOBAL PROPERTY VISTA_TEST_TARGETS_HAVE_BEEN_CONFIGURED ) 
if( NOT _TEST_TARGETS_HAVE_BEEN_CONFIGURED )

	add_custom_target( ${SILENT_TEST_NAME} 
						COMMAND ${CMAKE_CTEST_COMMAND} -C "${CTEST_CONFIG_DIR}" --force-new-ctest-process
						WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" )
	set_target_properties( ${SILENT_TEST_NAME} PROPERTIES FOLDER "Tests" )
	add_custom_target( ${FAILUREOUTPUT_TEST_NAME} 
						COMMAND ${CMAKE_CTEST_COMMAND} -C "${CTEST_CONFIG_DIR}" --force-new-ctest-process --output-on-failure
						WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" )
	set_target_properties( ${FAILUREOUTPUT_TEST_NAME} PROPERTIES FOLDER "Tests" )
	add_custom_target( ${VERBOSE_TEST_NAME} 
						COMMAND ${CMAKE_CTEST_COMMAND} -C "${CTEST_CONFIG_DIR}" --force-new-ctest-process -V
						WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" )
	set_target_properties( ${VERBOSE_TEST_NAME} PROPERTIES FOLDER "Tests" )
	
	set_property( GLOBAL PROPERTY VISTA_TEST_TARGETS_HAVE_BEEN_CONFIGURED TRUE )
endif()

#####################################################
# MACROS
#####################################################

# vista_configure_test( TEST_NAME ... )
# adds a new target which is configured as a local test application with the given test name.
# Use in your local testing directory's cmake file as follows:
#
##	configure the test itself and create the respective target(s)
#	vista_configure_test( TEST_NAME <list of all required .cpp/.h files>)
##	add input libraries as needed
#	target_link_libraries( TEST_NAME
#			<lib under test>
#			${VISTA_USE_PACKAGE_LIBRARIES}
#	)
# 	add_dependencies( TEST_NAME <dependencies> )
#
# Note that the list of required input files has to contain at least one cpp file 
# which features a standard main routine.
# In addtion, you have to use the follow-up target_link_libraries(...) and
# add_dependencies calls in order to specifically add all input libraries. 
# This will NOT HAPPEN automagically.
macro( vista_configure_test TEST_NAME )
	add_executable( ${TEST_NAME} ${ARGN} )
	vista_set_outdir( ${TEST_NAME} ${CMAKE_CURRENT_BINARY_DIR} )
	vista_configure_app( ${TEST_NAME} DONT_COPY_EXECUTABLE )
	vista_create_default_info_file( ${TEST_NAME} )
	# FOLDER property groups the executable's project into the "Tests" folder in the Visual Studio solution
	set_target_properties( ${TEST_NAME} PROPERTIES FOLDER "Tests" )
	
	add_test( NAME ${TEST_NAME}Execute 
		COMMAND ${TEST_NAME} 
		"--gtest_output=xml:${PROJECT_BINARY_DIR}/TestResults/${TEST_NAME}_results.xml" )
	
	# configure add_test with sufficient dll path envvar
	# VISTA_TARGET_LINK_DIRS filled via vista_use_package macro
	# VISTA_ADDITIONAL_PATHENTRIES filled via vista_add_pathscript_dynamic_lib_path macro
	set( _DYNAMIC_LIB_DIRS ${VISTA_TARGET_LINK_DIRS} ${VISTA_ADDITIONAL_PATHENTRIES} )
	if( _DYNAMIC_LIB_DIRS )
		if( WIN32 )
			string( REPLACE ";" "\\;" _ENVIRONMENT "${_DYNAMIC_LIB_DIRS}" )
			set_property( TEST ${TEST_NAME}Execute APPEND PROPERTY ENVIRONMENT "PATH=${_ENVIRONMENT}\\;%PATH%" )	
		else()
			string( REPLACE ";" ":" _ENVIRONMENT "${_DYNAMIC_LIB_DIRS}" )
			set( "_VAR" "\$ENV{LD_LIBRARY_PATH}" )
			# @TODO: for now we do not set the LD_LIBRARY_PATH, because cmake escaping prevents entering the correct syntax
			# thus, we'll have to rely on RPATH for now
			# set_property( TEST ${TEST_NAME}Execute APPEND PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${_ENVIRONMENT}:${_VAR}" )	
		endif()
	endif()
	
	add_dependencies( ${SILENT_TEST_NAME} ${TEST_NAME} )
	add_dependencies( ${FAILUREOUTPUT_TEST_NAME} ${TEST_NAME} )
	add_dependencies( ${VERBOSE_TEST_NAME} ${TEST_NAME} )
	
	# put all source files into the same folder
	source_group( "Source Files" FILES ${ARGN} )
endmacro()