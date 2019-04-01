#!/usr/bin/python 
# $Id$

import os, sys, re, string

localSourceFileName = "_SourceFiles.cmake"
backupExtension = ".BAK"
excludeDirs = [ "cvs", ".svn", "build", "built", "cmake" ]
sourceExtensions = [ ".c", ".cpp", ".h", ".inl" ]
addSourceFilesListToSources = True
defaultCoreLibsVersion = "PLUTO"

findCommented = re.compile( r'\s*#\s*.*' )
findSetListRegEx = re.compile( r'set\(\s*(\S+)\s*\Z' )
findSetVarRegEx = re.compile( r'set\(\s*(\S+)\s+(\S+)\s*\)' )
findForEachRegEx = re.compile( r'foreach\(\s+File\s+\$\{(\w+)\}' )
findSourceGroupRegEx = re.compile( r'source_group\(\s*"*\$\{(\w+)\}"*\s*FILES.*\)' )

MODE_APP = 1
MODE_LIB = 2
MODE_SRC = 3

def CheckIsAdditionalInfoArgument( argcount ):
	if( len( sys.argv ) <= argcount ):
		return False
	argument = sys.argv[argcount]
	return ( argument[0] != "-" )

def CheckIsSourceFile( entry ):
	name, ext = os.path.splitext( entry )
	if ext in sourceExtensions:
		return True
	else:
		return False

def CheckIsCommented( entry ):
	return( re.match( findCommented, entry ) )

def Backup( fileName ):
	if os.path.exists( fileName ):
		iCount = 1
		backupFile = fileName + backupExtension
		while os.path.exists( backupFile ):
			iCount = iCount + 1
			backupFile = fileName + backupExtension + str(iCount)
		os.rename( fileName, backupFile )
		print( "Backing up " + fileName )

def GetSourceFilesAndDirs( path ):
	files = []
	dirs = []
	for item in os.listdir( path ):
		if( os.path.isdir( os.path.join( path, item ) ) ):
			dirs.append( item )
		elif CheckIsSourceFile( item ):
				files.append( item )
	files.sort()
	dirs.sort()
	return files, dirs

def GenSourceListForSubdir( dirName, parentDir, renew, bIsToplevel = False, relDir = "", relSourceGroup = "" ):
	if( dirName in excludeDirs ):
		return False

	sourceSubDirs = []
	fullDirName = os.path.join( parentDir, dirName )

	# we dont want to parse the cmake directories
	testCacheFile = os.path.join( fullDirName, "CMakeCache.txt" )
	if( os.path.exists( testCacheFile ) ):
		return
		
	#we also don't want to parse folders which contain a CMakeLists.txt, since these are new cmake projects
	testCacheFile = os.path.join( fullDirName, "CMakeLists.txt" )
	if( bIsToplevel == False and os.path.exists( testCacheFile ) ):
		print( "skipping directory \"" + fullDirName + "\" - contains CMakeLists.txt" )
		return

	if( relDir == "" ):
		relDir = dirName
	else:
		relDir = relDir + "/" + dirName
	localSourceGroup = dirName
	if( dirName == "" or dirName == "src" or dirName == "Src" or dirName == "source" or dirName == "Source" ):
		localSourceGroup = "Source Files"
	if( relSourceGroup == "" ):
		relSourceGroup = localSourceGroup
	else:
		relSourceGroup = relSourceGroup + "\\\\" + localSourceGroup

	if( relDir == "" ):
		relDir = "."

	sourceFiles, subDirs = GetSourceFilesAndDirs( fullDirName )

	# recursively generate sourcefiles for subdirs
	for dir in subDirs:
		if GenSourceListForSubdir( dir, fullDirName, renew, False, relDir, relSourceGroup ):
			sourceSubDirs.append( dir )

	if( len( sourceSubDirs ) == 0 and len( sourceFiles ) == 0 ):
		return False # no source directory
		
	if( addSourceFilesListToSources ):
		sourceFiles.append( localSourceFileName )


	fileName = os.path.join( fullDirName, "_SourceFiles.cmake" )

	if( os.path.exists( fileName ) and not renew ):
		# the file already exists, we just need to update it
		#first,we just read the file
		fileHandle = open( fileName, "r" )
		origLines = fileHandle.readlines()
		fileHandle.close()

		SourceFileGroups = {}
		existingSourceFiles = []
		excludedSourceFiles = {}
		SourceFileGroupsNames = {}
		dictVariables = {}

		inSet = False
		currentSet = ""
		setEntries = []

		#first, we parse the file once and check for all source files already in there
		for line in origLines:
			line = line.strip()

			if inSet:
				# check if the set closes
				if line == ")":
					inSet = False
					if len( setEntries ) > 0:
						SourceFileGroups[currentSet] = setEntries
				else:
					setEntries.append( line )
					existingSourceFiles.append( line )
			else:
				result = re.match( findSetListRegEx, line )
				if result:
					inSet = True
					currentSet = result.group(1)
					setEntries = []
				else:
					result = re.match( findSetVarRegEx, line )
					if result:
						dictVariables[ result.group(1) ] = result.group(2)
					else:
						# we also need to check if it defines a sourcegroup's name
						# therefore, we first observe all foreach() loops to find the name
						result = re.match( findForEachRegEx, line )
						if result:
							currentSet = result.group(1)
						else:
							result = re.match( findSourceGroupRegEx, line )
							if result:
								SourceFileGroupsNames[currentSet] = result.group(1)

		#now, we check which files are not in the file yet
		missingFiles = []
		for file in sourceFiles:
			if( file in existingSourceFiles ):
				existingSourceFiles.remove( file )
			elif( "#" + file in existingSourceFiles ):
				existingSourceFiles.remove( "#" + file )
			else:
				missingFiles.append( file )

		# check if anything changed at all
		if( len( missingFiles ) == 0 and len( existingSourceFiles ) == 0 ):
			return True

		#now, files in missingFiles need to be added to the default source group
		if( len( missingFiles ) > 0 ):
			if( not "DirFiles" in SourceFileGroups ):
				SourceFileGroups["DirFiles"] = []
			for file in missingFiles:
				SourceFileGroups["DirFiles"].append( file )
			SourceFileGroups["DirFiles"].sort()

		#we also need to remove files that don't exist anymore
		for file in existingSourceFiles:
			for group in SourceFileGroups.values():
				if file in group:
					group.remove( file )

		# additionally, we default sourcegroups with no prior name
		for name in SourceFileGroups.keys():
			if not name in SourceFileGroupsNames:
				SourceFileGroupsNames[name] = name + "_SourceGroup"

		# we ensure that the SourceFileGroupsNames[name] variable exists, by initializing
		# non-existing ones to RelativeSourceGroup
		for name, entry in SourceFileGroupsNames.items():
			if not entry in dictVariables:
				dictVariables[entry] = "\"RelativeSourceGroup\""

		# make sure we dont break anything permanently: backup
		Backup( fileName )

		# no file there yet, just create a new one
		fileHandle = open( fileName, "w" )

		# write source files info
		fileHandle.write( "# $I" + "d:$\n\n" )
		fileHandle.write( "set( RelativeDir \"" + relDir + "\" )\n" )
		fileHandle.write( "set( RelativeSourceGroup \"" + relSourceGroup + "\" )" )
		fileHandle.write( "\n" )
		if( len( sourceSubDirs ) >  0 ):
			fileHandle.write( "set( SubDirs " )
			for dir in sourceSubDirs:
				fileHandle.write( dir + " " )
			fileHandle.write( ")\n" )
		fileHandle.write( "\n" )
		for name, list in SourceFileGroups.items():
			fileHandle.write( "set( " + name + "\n" )
			for file in list:
				fileHandle.write( "\t" + file + "\n" )
			fileHandle.write( ")\n" )
			fileHandle.write( "set( " + SourceFileGroupsNames[name] + " " + dictVariables[SourceFileGroupsNames[name]] + " )\n" )
			fileHandle.write( "\n" )
		for name, list in SourceFileGroups.items():
			fileHandle.write( "set( LocalSourceGroupFiles "" )\n" )
			fileHandle.write( "foreach( File ${" + name + "} )\n" )
			fileHandle.write( "\tlist( APPEND LocalSourceGroupFiles \"${RelativeDir}/${File}\" )\n" )
			fileHandle.write( "\tlist( APPEND ProjectSources \"${RelativeDir}/${File}\" )\n" )
			fileHandle.write( "endforeach()\n" )
			fileHandle.write( "source_group( ${" + SourceFileGroupsNames[name] + "} FILES ${LocalSourceGroupFiles} )\n" )
			fileHandle.write( "\n" )
		if( len( sourceSubDirs ) >  0 ):
			fileHandle.write( "set( SubDirFiles \"\" )\n" )
			fileHandle.write( "foreach( Dir ${SubDirs} )\n" )
			fileHandle.write( "\tlist( APPEND SubDirFiles \"${RelativeDir}/${Dir}/_SourceFiles.cmake\" )\n" )
			fileHandle.write( "endforeach()\n" )
			fileHandle.write( "\n" )
			fileHandle.write( "foreach( SubDirFile ${SubDirFiles} )\n" )
			fileHandle.write( "\tinclude( ${SubDirFile} )\n" )
			fileHandle.write( "endforeach()\n" )
			fileHandle.write( "\n" )


	else:
		# make sure we dont break anything permanently: backup
		Backup( fileName )

		# no file there yet, or we should overwrite it -> just create a new one
		fileHandle = open( fileName, "w" )

		# write source files info
		fileHandle.write( "# $I" + "d:$\n\n" )
		fileHandle.write( "set( RelativeDir \"" + relDir + "\" )\n" )
		fileHandle.write( "set( RelativeSourceGroup \"" + relSourceGroup + "\" )" )
		fileHandle.write( "\n" )
		if( len( sourceSubDirs ) >  0 ):
			fileHandle.write( "set( SubDirs " )
			for dir in sourceSubDirs:
				fileHandle.write( dir + " " )
			fileHandle.write( ")\n" )
		fileHandle.write( "\n" )
		if( len( sourceFiles ) > 0 ):
			fileHandle.write( "set( DirFiles\n" )
			for file in sourceFiles:
				fileHandle.write( "\t" + file + "\n" )
			fileHandle.write( ")\n" )
			fileHandle.write( "set( DirFiles_SourceGroup \"${RelativeSourceGroup}\" )\n" )
			fileHandle.write( "\n" )
			fileHandle.write( "set( LocalSourceGroupFiles "" )\n" )
			fileHandle.write( "foreach( File ${DirFiles} )\n" )
			fileHandle.write( "\tlist( APPEND LocalSourceGroupFiles \"${RelativeDir}/${File}\" )\n" )
			fileHandle.write( "\tlist( APPEND ProjectSources \"${RelativeDir}/${File}\" )\n" )
			fileHandle.write( "endforeach()\n" )
			fileHandle.write( "source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )\n" )
			fileHandle.write( "\n" )
		if( len( sourceSubDirs ) >  0 ):
			fileHandle.write( "set( SubDirFiles \"\" )\n" )
			fileHandle.write( "foreach( Dir ${SubDirs} )\n" )
			fileHandle.write( "\tlist( APPEND SubDirFiles \"${RelativeDir}/${Dir}/_SourceFiles.cmake\" )\n" )
			fileHandle.write( "endforeach()\n" )
			fileHandle.write( "\n" )
			fileHandle.write( "foreach( SubDirFile ${SubDirFiles} )\n" )
			fileHandle.write( "\tinclude( ${SubDirFile} )\n" )
			fileHandle.write( "endforeach()\n" )
			fileHandle.write( "\n" )

	return True


def GenSourceLists( startDir, renew ):
	sourceSubDirs = [] # should usually be just src, but oh well
	sourceFiles, subDirs = GetSourceFilesAndDirs( startDir )

	#check if there are toplevel sourcefiles
	if( len( sourceFiles ) > 0 ):
		GenSourceListForSubdir( "", startDir, renew, True )
		sourceSubDirs.append( "." )
	else:
		for dir in subDirs:
			isSourceDir = GenSourceListForSubdir( dir, startDir, renew, False )
			if isSourceDir:
				sourceSubDirs.append( dir )

	return sourceSubDirs


def GenCMakeForLib( startDir, projectName, renew, version, linkVistaCoreLibs, multiProjectParent = "" ):
	sourceSubDirs = GenSourceLists( startDir, renew )

	if( len( sourceSubDirs ) == 0 ):
		if( multiProjectParent != "" ):
			print( "Project Directory " + startDir + " contains no sources" )
		return False

	listsFile = os.path.join( startDir, "CMakeLists.txt" )
	Backup( listsFile )

	fileHandle = open( listsFile, "w" )

	fileHandle.write( "# $I" + "d:$\n\n" )
	fileHandle.write( "cmake_minimum_required( VERSION 2.8 )\n" )
	if( multiProjectParent == "" ):
		fileHandle.write( "project( " + projectName + " )\n" )
		fileHandle.write( "\n" )
		fileHandle.write( "list( APPEND CMAKE_MODULE_PATH \"$ENV{VISTA_CMAKE_COMMON}\" )\n" )
		fileHandle.write( "\n" )
		fileHandle.write( "include( VistaCommon )\n" )
	else:
		fileHandle.write( "if( NOT " + str.upper(multiProjectParent) + "_COMMON_BUILD )\n" )
		fileHandle.write( "\tproject( " + projectName + " )\n" )
		fileHandle.write( "\n" )
		fileHandle.write( "\tlist( APPEND CMAKE_MODULE_PATH \"$ENV{VISTA_CMAKE_COMMON}\" )\n" )
		fileHandle.write( "\tinclude( VistaCommon )\n" )
		fileHandle.write( "endif( NOT " + str.upper(multiProjectParent) + "_COMMON_BUILD )\n" )
	fileHandle.write( "\n" )
	if version:
		fileHandle.write( "vista_set_version( " + projectName + " " + version + " )\n" )
		fileHandle.write( "\n" )
	if linkVistaCoreLibs:
		fileHandle.write( "vista_use_package( VistaCoreLibs \"" + linkVistaCoreLibs + "\" REQUIRED FIND_DEPENDENCIES )\n" )
		fileHandle.write( "\n" )
	fileHandle.write( "\n" )
	fileHandle.write( "# Including the source files of all source subfolders recursively\n" )
	for dir in sourceSubDirs:
		fileHandle.write( "include( \"" + dir + "/" + localSourceFileName + "\" )\n" )
	fileHandle.write( "\n" )
	fileHandle.write( "add_library( " + projectName + " ${ProjectSources} )\n" )
	if( multiProjectParent != "" ):
		fileHandle.write( "#The following line prevent CMake from adding all depencies to other projects that link it from within the same cmake build\n" )
		fileHandle.write( "set_property( TARGET " + projectName + " PROPERTY LINK_INTERFACE_LIBRARIES \"\" )\n" )
	fileHandle.write( "target_link_libraries( " + projectName + "\n" )
	fileHandle.write( "\t${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls\n" )
	fileHandle.write( ")\n" )
	fileHandle.write( "\n" )
	if( multiProjectParent == "" ):
		fileHandle.write( "vista_configure_lib( " + projectName + " )\n" )
		fileHandle.write( "vista_install( " + projectName + " )\n" )
		fileHandle.write( "vista_create_cmake_configs( " + projectName + " )\n" )
		fileHandle.write( "vista_create_default_info_file( " + projectName + " )\n" )
	else:
		fileHandle.write( "vista_configure_lib( " + projectName + " )\n" )
		fileHandle.write( "if( " + str.upper(multiProjectParent) + "_COMMON_BUILD )\n" )
		fileHandle.write( "\tvista_install( " + projectName + " " + projectName + " )\n" )
		fileHandle.write( "else( " + str.upper(multiProjectParent) + "_COMMON_BUILD )\n" )
		fileHandle.write( "\tvista_install( " + projectName + " )\n" )
		fileHandle.write( "endif( " + str.upper(multiProjectParent) + "_COMMON_BUILD )\n" )
		fileHandle.write( "vista_create_cmake_configs( " + projectName + " )\n" )
		fileHandle.write( "vista_create_default_info_file( " + projectName + " )\n" )
	fileHandle.write( "\n" )

	return True


def GenCMakeForApp( startDir, projectName, renew, version, linkVistaCoreLibs, multiProjectParent = "" ):
	sourceSubDirs = GenSourceLists( startDir, renew )

	if( len( sourceSubDirs ) == 0 ):
		if( multiProjectParent != "" ):
			print( "Project Directory " + startDir + " contains no sources" )
		return False

	listsFile = os.path.join( startDir, "CMakeLists.txt" )
	Backup( listsFile )

	fileHandle = open( listsFile, "w" )

	fileHandle.write( "# $I" + "d:$\n\n" )
	fileHandle.write( "cmake_minimum_required( VERSION 2.8 )\n" )
	if( multiProjectParent == "" ):
		fileHandle.write( "project( " + projectName + " )\n" )
		fileHandle.write( "\n" )
		fileHandle.write( "list( APPEND CMAKE_MODULE_PATH \"$ENV{VISTA_CMAKE_COMMON}\" )\n" )
		fileHandle.write( "\n" )
		fileHandle.write( "include( VistaCommon )\n" )
	else:
		fileHandle.write( "if( NOT " + str.upper(multiProjectParent) + "_COMMON_BUILD )\n" )
		fileHandle.write( "\tproject( " + projectName + " )\n" )
		fileHandle.write( "\n" )
		fileHandle.write( "\tlist( APPEND CMAKE_MODULE_PATH \"$ENV{VISTA_CMAKE_COMMON}\" )\n" )
		fileHandle.write( "\tinclude( VistaCommon )\n" )
		fileHandle.write( "endif( NOT " + str.upper(multiProjectParent) + "_COMMON_BUILD )\n" )
	fileHandle.write( "\n" )
	if version:
		fileHandle.write( "vista_set_version( " + projectName + " " + version + " )" )
		fileHandle.write( "\n" )
	if linkVistaCoreLibs:
		fileHandle.write( "vista_use_package( VistaCoreLibs \"" + linkVistaCoreLibs + "\" REQUIRED FIND_DEPENDENCIES )\n" )
		fileHandle.write( "\n" )
	fileHandle.write( "\n" )
	fileHandle.write( "# Including the source files of all source subfolders recursively\n" )
	for dir in sourceSubDirs:
		fileHandle.write( "include( \"" + dir + "/" + localSourceFileName + "\" )\n" )		
	if os.path.isdir( startDir + "/configfiles" ):
		fileHandle.write( "vista_add_files_to_sources( ProjectSources \"configfiles\" SOURCE_GROUP \"configfiles\" \"ini\" )\n" )
	if os.path.isdir( startDir + "/configfiles/xml" ):
		fileHandle.write( "vista_add_files_to_sources( ProjectSources \"configfiles/xml\" SOURCE_GROUP \"configfiles\\\\\\\\xml\" \"xml\" )\n" )
	elif os.path.isdir( startDir + "/xml" ):
		fileHandle.write( "vista_add_files_to_sources( ProjectSources \"xml\" SOURCE_GROUP \"xml\" \"xml\" )\n" )
	fileHandle.write( "\n" )
	fileHandle.write( "add_executable( " + projectName + " ${ProjectSources} )\n" )
	fileHandle.write( "target_link_libraries( " + projectName + "\n" )
	fileHandle.write( "\t${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls\n" )
	fileHandle.write( ")\n" )
	fileHandle.write( "\n" )
	fileHandle.write( "vista_configure_app( " + projectName + " )\n" )
	fileHandle.write( "vista_create_default_info_file( " + projectName + " )\n" )
	fileHandle.write( "\n" )	

	return True

def GenMultiProject( mode, startDir, projectName, renew, version, linkVistaCoreLibs ):

	projectSubDirs = []

	files, dirs = GetSourceFilesAndDirs( startDir )

	for dir in dirs:
		fullDir = os.path.join( startDir, dir )
		if mode == MODE_SRC:
			GenSourceLists( fullDir, renew )
		elif mode == MODE_APP:
			if( GenCMakeForApp( fullDir, dir, renew, version, linkVistaCoreLibs, projectName ) ):
				projectSubDirs.append( dir )
		else:
			if( GenCMakeForLib( fullDir, dir, renew, version, linkVistaCoreLibs, projectName ) ):
				projectSubDirs.append( dir )
				
	if mode == MODE_SRC:
		return

	if( len( projectSubDirs ) == 0 ):
		print( "No sub-projects found in folder " + startDir )
		return

	projectNameUpper = str.upper( projectName )


	listsFile = os.path.join( startDir, "CMakeLists.txt" )
	Backup( listsFile )

	fileHandle = open( listsFile, "w" )

	fileHandle.write( "# $I" + "d:$\n\n" )
	fileHandle.write( "cmake_minimum_required( VERSION 2.8 )\n" )
	fileHandle.write( "project( " + projectName + " )\n" )
	fileHandle.write( "\n" )
	fileHandle.write( "include_directories( ${" + projectName + "_SOURCE_DIR} )\n" )
	fileHandle.write( "\n" )
	fileHandle.write( "list( APPEND CMAKE_MODULE_PATH \"$ENV{VISTA_CMAKE_COMMON}\" )\n" )
	fileHandle.write( "include( VistaCommon )\n" )
	fileHandle.write( "\n" )
	if( version ):
		fileHandle.write( "vista_set_version( " + projectName + " " + version + " )\n" )
		fileHandle.write( "\n" )
	fileHandle.write( "\n" )
	fileHandle.write( "# this variable indicates to sub-projects that they are build all together\n" )
	fileHandle.write( "set( " + projectNameUpper + "_COMMON_BUILD TRUE )\n" )
	fileHandle.write( "\n" )
	for subproject in projectSubDirs:
		fileHandle.write( "vista_conditional_add_subdirectory( " + projectNameUpper + "_BUILD_" + str.upper(subproject) + " " + subproject + " ON )\n" )
	fileHandle.write( "\n" )

	
if len( sys.argv ) >= 2 and sys.argv[1] != "-h" and sys.argv[1] != "--help" :
	if sys.argv[1][0] == '-':
		startDir = os.getcwd()
		argcount = 1
	else:
		startDir = sys.argv[1]
		argcount = 2
	mode = MODE_SRC
	onlyBuildSourceLists = False
	projectName = os.path.basename( startDir )
	renew = False
	linkVistaCoreLibs = False
	multiProject = False
	version = False
	while( argcount < len( sys.argv ) ):
		arg = sys.argv[argcount]
		if( arg == "-app" or arg == "-application" ):
			mode = MODE_APP
		elif( arg == "-lib" or arg == "-library" ):
			mode = MODE_LIB
		elif( arg == "-source" or arg == "-src" ):
			mode = MODE_SRC
		elif( arg == "-renew" ):
			renew = True
		elif( arg == "-multiproj" ):
			multiProject = True
		elif( arg == "-linkcorelibs" ):
			linkVistaCoreLibs = True
			if( CheckIsAdditionalInfoArgument( argcount + 1 ) ):
				argcount = argcount + 1
				linkVistaCoreLibs = sys.argv[argcount]
			else:
				linkVistaCoreLibs = defaultCoreLibsVersion
		elif( arg == "-name" ):
			argcount = argcount + 1
			projectName = sys.argv[argcount]
		elif( arg == "-version" ):
			argcount = argcount + 1
			version_type = sys.argv[argcount]
			argcount = argcount + 1
			version_name = sys.argv[argcount]
			version = version_type + " " + version_name;
			if( CheckIsAdditionalInfoArgument( argcount + 1 ) ):
				argcount = argcount + 1
				version = version + " " + str( sys.argv[argcount] )
			if( CheckIsAdditionalInfoArgument( argcount + 1 ) ):
				argcount = argcount + 1
				version = version + " " + str( sys.argv[argcount] )
			if( CheckIsAdditionalInfoArgument( argcount + 1 ) ):
				argcount = argcount + 1
				version = version + " " + str( sys.argv[argcount] )
			if( CheckIsAdditionalInfoArgument( argcount + 1 ) ):
				argcount = argcount + 1
				version = version + " " + str( sys.argv[argcount] )
		else:
			print( "unknown parameter: " + arg )
		argcount = argcount + 1


	if multiProject :
		GenMultiProject( mode, startDir, projectName, renew, version, linkVistaCoreLibs )
	elif mode == MODE_SRC:
		GenSourceLists( startDir, renew )
	elif mode == MODE_APP:
		GenCMakeForApp( startDir, projectName, renew, version, linkVistaCoreLibs )
	else:
		GenCMakeForLib( startDir, projectName, renew, version, linkVistaCoreLibs )
else:
	print( "Usage:" )
	print( "GenerateCMakeProjectFiles.py [MainDir] [Options]" )
	print( "Options" )
	print( "  -app                     : the project will be configured as an application" )
	print( "  -lib                     : the project will be configured as a library " )
	print( "  -src                     : if set, only the " + localSourceFileName + "-files will be updated  [default]" )
	print( "  -name                    : specify name of the project. if omitted, the directory name will be used instead" )
	print( "  -multiproj               : if set, the specified directory will be seen as a base for several sub-projects in individual" )
	print( "                           : subfolders, which will be configured as individual projects of the specified type" )
	print( "                           : if a name is specified, it is used for the compound project - sub-projects get their dir's name" )
	print( "  -renew                   : if set, no file will be updated, but instead all files are created completely new" )
	print( "  -linkcorelibs [version]  : the project will be configured to link the VistaCoreLibs (takes optional version, defaults to latest release \" , defaultCoreLibsVersion, \")" )
	print( "  -version [...]           : Sets the version of the projects to the specified version, which has to be")
	print( "                             TYPE NAME [MAJOR [MINOR [REVISION [PATCH]]]]")

