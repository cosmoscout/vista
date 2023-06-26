

set( RelativeDir "./DisplayManager/GlutWindowImp" )
set( RelativeSourceGroup "Source Files\\DisplayManager\\GlutWindowImp" )

set( DirFiles
	VistaGlutTextEntity.cpp
	VistaGlutTextEntity.h
	VistaGlutWindowingToolkit.cpp
	VistaGlutWindowingToolkit.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

