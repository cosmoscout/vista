# $Id:

# This provides cpack configuration for Vista projects
# This file is executed once per generator defined in CMakelists.txt

set( CPACK_PACKAGE_RELOCATABLE FALSE )
set( CPACK_PACKAGE_VENDOR "VR-group RWTH Aachen" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Virtual reality software toolkit for scientific and technical applications." )

if( NOT EXISTS "${CPACK_CMAKE_CURRENT_SOURCE_DIR}/package_description.txt" )
	file( WRITE "${CPACK_CMAKE_CURRENT_SOURCE_DIR}/package_description.txt" "ViSTA is a software platform that allows integration of VR technology and interactive, 3D visualization into technical and scientific applications." )
endif()
set( CPACK_PACKAGE_DESCRIPTION_FILE "${CPACK_CMAKE_CURRENT_SOURCE_DIR}/package_description.txt" )

if( ${CPACK_CMAKE_BUILD_TYPE} MATCHES "Debug" )
	set( CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-devel" )
endif()

if( ${CPACK_GENERATOR} MATCHES "RPM" )

	set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CPACK_SYSTEM_PROCESSOR}" )

	set( CPACK_RPM_PACKAGE_SUMMARY ${CPACK_PACKAGE_DESCRIPTION_SUMMARY} )
	set( CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME} )
	set( CPACK_RPM_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION} )
	set( CPACK_RPM_PACKAGE_RELEASE 1 )
	set( CPACK_RPM_PACKAGE_LICENSE "GPL/LGPL" )
	if( ${CPACK_TARGET_TYPE} MATCHES "APP" )
		set( CPACK_RPM_PACKAGE_GROUP "Applications/VR" )
	else()
		set( CPACK_RPM_PACKAGE_GROUP "Development/Libraries" )
	endif()
	set( CPACK_RPM_PACKAGE_URL "http://sourceforge.net/projects/vistavrtoolkit/" )
	set( CPACK_RPM_PACKAGE_PROVIDES ${CPACK_PACKAGE_NAME} )
	set( CPACK_RPM_PACKAGE_OBSOLETES ${CPACK_PACKAGE_NAME} )

	file( WRITE "${CPACK_CMAKE_CURRENT_SOURCE_DIR}/package_post_script" "%run_ldconfig" )
	set( CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CPACK_CMAKE_CURRENT_SOURCE_DIR}/package_post_script" )
	set( CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CPACK_CMAKE_CURRENT_SOURCE_DIR}/package_post_script" )

elseif( ${CPACK_GENERATOR} MATCHES "TGZ" )
	set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}" )

elseif( ${CPACK_GENERATOR} MATCHES "ZIP" )
	set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}" )

elseif( ${CPACK_GENERATOR} MATCHES "NSIS" )
endif()	

