file(GLOB_RECURSE AdditionalSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

file(GLOB_RECURSE UtilsSources
		"${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.h"
)
list(APPEND AdditionalSources ${UtilsSources})

if (VISTACORELIBS_USE_ZEROMQ)
    file(GLOB_RECURSE ZeroMQSources
            "${CMAKE_CURRENT_SOURCE_DIR}/ZeroMQExt/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/ZeroMQExt/*.h"
    )
    list(APPEND AdditionalSources ${ZeroMQSources})
endif ()

list(APPEND ProjectSources ${AdditionalSources})