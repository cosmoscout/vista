file(GLOB_RECURSE AdditionalSources
		"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

list(APPEND ProjectSources ${AdditionalSources})
set(ProjectSources ${ProjectSources} PARENT_SCOPE)
