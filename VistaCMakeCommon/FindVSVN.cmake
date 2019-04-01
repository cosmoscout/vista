

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )
include( VistaHWArchSettings )

if( NOT SVN_FOUND )
	find_program( SVN_EXECUTABLE svn DOC "SVN command line client" )
	mark_as_advanced( SVN_EXECUTABLE )
	
	if( NOT SVN_EXECUTABLE )
		find_program( TORTOISESVN_EXECUTABLE SubWCRev )
		mark_as_advanced( TORTOISESVN_EXECUTABLE )
	endif()
	
endif( NOT SVN_FOUND )

# vista_get_svn_info( REVISION_VARIABLE REPOS_VARIABLE DATE_VARIABLE [DIRECTORY] )
# extracts the svn info (revision, repository, and last change date) of the current source dir
#  and stores it in the target variables. If the current directory is not under svn versioning, the
# variables will be empty. If available, svn is used directly to query the info, otherwise,
# a hand-taylored file parsing is used -- however, this may not work correctly with all versions
# by default, the svn of the current source directory is parsed. However, the optional DIRECTORY
# parameter can be used to specify another directory
macro( vista_get_svn_info _REVISION_VAR _REPOS_VAR _DATE_VAR )
	set( ${_REVISION_VAR} )
	set( ${_REPOS_VAR} )
	set( ${_DATE_VAR} )
	
	if( ${ARGC} GREATER 3 )
		set( _DIRECTORY ${ARGV3} )
	else( ${ARGC} GREATER 3 )
		set( _DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
	endif( ${ARGC} GREATER 3 )	
		
	if( SVN_EXECUTABLE )
		# standard SVN CLI		
		
		# this is an adoption of the official svn macro, to avoid the SEND_ERROR stuff
		
		# the subversion commands should be executed with the C locale, otherwise
		# the message (which are parsed) may be translated, Alex
		
		set( _TMP_SVN_WC_URL )
		set( _Subversion_SAVED_LC_ALL "$ENV{LC_ALL}" )
		set( ENV{LC_ALL} C )

		execute_process( COMMAND ${SVN_EXECUTABLE} info ${_DIRECTORY}
							OUTPUT_VARIABLE _SVN_WC_INFO
							ERROR_VARIABLE Subversion_svn_info_error
							RESULT_VARIABLE Subversion_svn_info_result
							OUTPUT_STRIP_TRAILING_WHITESPACE )

		if( ${Subversion_svn_info_result} EQUAL 0 )
			string( REGEX REPLACE "^(.*\n)?URL: ([^\n]+).*"
					"\\2" ${_REPOS_VAR} "${_SVN_WC_INFO}")
			string( REGEX REPLACE "^(.*\n)?Repository Root: ([^\n]+).*"
					"\\2" _VOID_OUTPUT "${_SVN_WC_INFO}")
			string( REGEX REPLACE "^(.*\n)?Revision: ([^\n]+).*"
					"\\2" ${_REVISION_VAR} "${_SVN_WC_INFO}")
			string( REGEX REPLACE "^(.*\n)?Last Changed Author: ([^\n]+).*"
					"\\2" _VOID_OUTPUT "${_SVN_WC_INFO}")
			string( REGEX REPLACE "^(.*\n)?Last Changed Rev: ([^\n]+).*"
					"\\2" _VOID_OUTPUT "${_SVN_WC_INFO}")
			string( REGEX REPLACE "^(.*\n)?Last Changed Date: ([^\n]+).*"
					"\\2" ${_DATE_VAR} "${_SVN_WC_INFO}")
		endif()

		# restore the previous LC_ALL
		set( ENV{LC_ALL} ${_Subversion_SAVED_LC_ALL} )
		
	elseif( TORTOISESVN_EXECUTABLE )
		# check with tortoisesvn
		set( _SVNINFO_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/svninfo.txt" )
		file( WRITE "${_SVNINFO_FILENAME}" "$WCREV$\n$WCDATE$\n$WCURL$" )			
		execute_process( COMMAND ${TORTOISESVN_EXECUTABLE} ${_DIRECTORY}
							"${_SVNINFO_FILENAME}" "${_SVNINFO_FILENAME}" )
							
		file( STRINGS "${_SVNINFO_FILENAME}" _SVNINFO_TEXT )
		
		list( GET _SVNINFO_TEXT 0 ${_REVISION_VAR} )
		if( ${_REVISION_VAR} EQUAL 0 )
			set( _DATE_VAR "" )
			set( _REPOS_VAR "" )
		else()
			list( GET _SVNINFO_TEXT 1 ${_DATE_VAR} )
			list( GET _SVNINFO_TEXT 2 ${_REPOS_VAR} )
		endif()		

	endif() 

endmacro()


# vista_get_svn_revision( TARGET_VARIABLE [DIRECTORY] )
# extracts the svn revision from the file system and stores it in the specified target variable
# for details, see vista_get_svn_info
macro( vista_get_svn_revision _TARGET_VAR )
	vista_get_svn_info( ${_TARGET_VAR} _TMP_SVN_REPOS _TMP_SVN_DATE ${ARGN} )
endmacro( vista_get_svn_revision )

#find_package_handle_standard_args( VOPENCL "OPENCL could not be found" OPENCL_ROOT_DIR )
