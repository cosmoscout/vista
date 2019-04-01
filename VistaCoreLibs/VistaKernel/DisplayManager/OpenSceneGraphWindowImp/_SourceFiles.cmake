

set( RelativeDir "./DisplayManager/OpenSceneGraphWindowImp" )
set( RelativeSourceGroup "Source Files\\DisplayManager\\OpenSceneGraphWindowImp" )

set( DirFiles
	VistaOSGWindowingToolkit.cpp
	VistaOSGWindowingToolkit.h
	_SourceFiles.cmake
)
if( NOT VISTACORELIBS_BUILD_WINDOWIMP_GLUT )
	# workaround, since we are currently still using the glut text imp
	list( APPEND DirFiles "../GlutWindowImp/VistaGlutTextEntity.h" "../GlutWindowImp/VistaGlutTextEntity.cpp" )
endif()
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

