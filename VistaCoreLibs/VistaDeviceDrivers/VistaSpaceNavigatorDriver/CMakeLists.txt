IF (WIN32)
    LINK_DIRECTORIES("${OPENSG_ROOT}/lib")
    INCLUDE_DIRECTORIES("${OPENSG_ROOT}/Include")
ENDIF (WIN32)

#Actual Driver Module
add_library(VistaSpaceNavigatorDriver
        VistaSpaceNavigatorDriver.h
        VistaSpaceNavigatorDriver.cpp
)

TARGET_LINK_LIBRARIES(VistaSpaceNavigatorDriver
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
        ${GLUT_glut_LIBRARY}
)
INSTALL(TARGETS VistaSpaceNavigatorDriver
        LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})


#Transcoder
add_library(VistaSpaceNavigatorTranscoder
        VistaSpaceNavigatorTranscoder.cpp
)
TARGET_LINK_LIBRARIES(VistaSpaceNavigatorTranscoder
        VistaBase
        VistaAspects
        VistaInterProcComm
        VistaDeviceDriversBase
)
INSTALL(TARGETS VistaSpaceNavigatorTranscoder
        LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})

#Driver Plugin

add_library(VistaSpaceNavigatorPlugin
        VistaSpaceNavigatorPlugin.cpp
)
TARGET_LINK_LIBRARIES(VistaSpaceNavigatorPlugin
        VistaSpaceNavigatorDriver
        VistaDeviceDriversBase
        VistaKernel
        ${LIBRARIES}
        ${GLUT_glut_LIBRARY}
)

INSTALL(TARGETS VistaSpaceNavigatorPlugin
        LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})

IF (WIN32)
    SET_TARGET_PROPERTIES(VistaSpaceNavigatorDriver PROPERTIES COMPILE_DEFINITIONS "VISTASPACENAVIGATORAPI_EXPORTS")
    SET_TARGET_PROPERTIES(VistaSpaceNavigatorPlugin PROPERTIES COMPILE_DEFINITIONS "VISTASPACENAVIGATORPLUGIN_EXPORTS")
    SET_TARGET_PROPERTIES(VistaSpaceNavigatorTranscoder PROPERTIES COMPILE_DEFINITIONS "VISTASPACENAVIGATORTRANSCODER_EXPORTS")
ENDIF (WIN32)

SET_TARGET_PROPERTIES(VistaSpaceNavigatorDriver PROPERTIES FOLDER "Drivers")
SET_TARGET_PROPERTIES(VistaSpaceNavigatorPlugin PROPERTIES FOLDER "Drivers")
SET_TARGET_PROPERTIES(VistaSpaceNavigatorTranscoder PROPERTIES FOLDER "Drivers")


INSTALL(DIRECTORY .
        DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaDeviceDrivers/VistaSpaceNavigator
        FILES_MATCHING PATTERN "*.h"
        PATTERN "build" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE)
