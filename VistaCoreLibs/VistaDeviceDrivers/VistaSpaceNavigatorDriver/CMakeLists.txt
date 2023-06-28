IF (WIN32)
    LINK_DIRECTORIES("${OPENSG_ROOT}/lib")
	INCLUDE_DIRECTORIES( "${OPENSG_ROOT}/Include" )
ENDIF(WIN32)

#Actual Driver Module
SET (dirFiles_driver
    VistaSpaceNavigatorDriver.h
    VistaSpaceNavigatorDriver.cpp
)
SOURCE_GROUP ("Source Files" FILES ${dirFiles_driver})
ADD_LIBRARY (VistaSpaceNavigatorDriver
    ${dirFiles_driver}
)

TARGET_LINK_LIBRARIES (VistaSpaceNavigatorDriver
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
	${GLUT_glut_LIBRARY}
)
INSTALL (TARGETS VistaSpaceNavigatorDriver
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})

		 
#Transcoder
SET (dirFiles_transcoder
    VistaSpaceNavigatorTranscoder.cpp
)
SOURCE_GROUP ("Source Files" FILES ${dirFiles_transcoder})
ADD_LIBRARY (VistaSpaceNavigatorTranscoder
    ${dirFiles_transcoder}
)
TARGET_LINK_LIBRARIES (VistaSpaceNavigatorTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
INSTALL (TARGETS VistaSpaceNavigatorTranscoder
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})
	 
#Driver Plugin

SET (dirFiles_plugin
    VistaSpaceNavigatorPlugin.cpp
)
SOURCE_GROUP ("Source Files" FILES ${dirFiles_plugin})
ADD_LIBRARY (VistaSpaceNavigatorPlugin
    ${dirFiles_plugin}
) 
TARGET_LINK_LIBRARIES (VistaSpaceNavigatorPlugin
	VistaSpaceNavigatorDriver
	VistaDeviceDriversBase
	VistaKernel
	${LIBRARIES}
	${GLUT_glut_LIBRARY}
)

INSTALL (TARGETS VistaSpaceNavigatorPlugin
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})	

IF (WIN32)
	SET_TARGET_PROPERTIES( VistaSpaceNavigatorDriver PROPERTIES COMPILE_DEFINITIONS "VISTASPACENAVIGATORAPI_EXPORTS" )
	SET_TARGET_PROPERTIES( VistaSpaceNavigatorPlugin PROPERTIES COMPILE_DEFINITIONS "VISTASPACENAVIGATORPLUGIN_EXPORTS" )
	SET_TARGET_PROPERTIES( VistaSpaceNavigatorTranscoder PROPERTIES COMPILE_DEFINITIONS "VISTASPACENAVIGATORTRANSCODER_EXPORTS" )
ENDIF(WIN32)

SET_TARGET_PROPERTIES( VistaSpaceNavigatorDriver PROPERTIES FOLDER "Drivers" )
SET_TARGET_PROPERTIES( VistaSpaceNavigatorPlugin PROPERTIES FOLDER "Drivers" )
SET_TARGET_PROPERTIES( VistaSpaceNavigatorTranscoder PROPERTIES FOLDER "Drivers" )
	 
		 
INSTALL (DIRECTORY .
         DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaDeviceDrivers/VistaSpaceNavigator
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
