# $Id: GenerateCMakeProjectFiles.py 21462 2011-05-24 08:20:01Z dr165799 $

cmake_minimum_required( VERSION 2.8 )
project( ImageTest )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )

include( VistaCommon )

vista_use_package( VistaCoreLibs REQUIRED VistaKernel VistaKernelOpenSGExt FIND_DEPENDENCIES )

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( ImageTest ${ProjectSources} )
target_link_libraries( ImageTest
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

set( CMAKE_DEBUG_POSTFIX "" )
vista_set_outdir( ImageTest "${CMAKE_CURRENT_BINARY_DIR}/libs" USE_CONFIG_SUBDIRS )

vista_configure_app( ImageTest )
vista_create_default_info_file( ImageTest )

add_custom_command( TARGET ImageTest
                    POST_BUILD
                    COMMAND "${CMAKE_COMMAND}"
					ARGS -E copy_if_different $<CONFIGURATION>/ImageTest.exe $<CONFIGURATION>/ImageTest.pyd
                     WORKING_DIRECTORY "${IMAGETEST_TARGET_OUTDIR}"
                     COMMENT "blah" 
					)

vista_add_external_msvc_project_of_package( VistaCoreLibs "ExternalLibs" DEPENDENT ImageTest )

